#include "hyprland.h"

#include <functional>
#include <utility>

#include <qbytearrayview.h>
#include <qcontainerfwd.h>
#include <qfileinfo.h>
#include <qhash.h>
#include <qjsengine.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qjsonvalue.h>
#include <qlist.h>
#include <qlocalsocket.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qprocess.h>
#include <qscopeguard.h>
#include <qstringview.h>
#include <qtenvironmentvariables.h>

#include "hyprevents.h"
#include "hyprinputconfig.h"
#include "hyprmonitorsmodel.h"
#include "toplevelmanager.h"
#include "toplevelmodel.h"
#include "workspacesmodel.h"

namespace ns::hyprland {
Q_LOGGING_CATEGORY(logNSHyprland,
                   "nightshell.hyprland")

Hyprland::Hyprland(QObject *parent)
    : QObject(parent),
      m_eventHandler(new HyprEvents(this)),
      m_toplevelModel(new ToplevelModel(this)),
      m_monitorsModel(new HyprMonitorsModel(this)),
      m_workspacesModel(new WorkspacesModel(this)),
      m_inputConfig(new HyprInputConfig(this)) {
  auto his = qEnvironmentVariable("HYPRLAND_INSTANCE_SIGNATURE");
  if (his.isEmpty()) {
    qCWarning(logNSHyprland) << "$HYPRLAND_INSTANCE_SIGNATURE is unset, unable "
                                "to connect to hyprland's sockets.";
    return;
  }

  auto runDir  = qEnvironmentVariable("XDG_RUNTIME_DIR");
  auto hyprDir = runDir + "/hypr/" + his;
  if (!QFileInfo(hyprDir).isDir()) {
    hyprDir = "/tmp/hypr/" + his;
  }
  if (!QFileInfo(hyprDir).isDir()) {
    qCWarning(logNSHyprland) << "Unable to find hyprland sockets.";
    return;
  }

  m_requestSocketPath = hyprDir + "/.socket.sock";
  m_eventHandler->connectSocket();

  this->queryActiveDevices();
  this->queryHyprInputConfigs();
  this->queryMonitors(); // explicitly doing this one first so the newly created
                         // instances can automatically fill up its workspace
                         // child list
  this->queryWorkspaces();

  QObject::connect(m_eventHandler,
                   &HyprEvents::configReloaded,
                   this,
                   &Hyprland::queryActiveDevices);
  QObject::connect(m_eventHandler,
                   &HyprEvents::keyboardLayoutChanged,
                   this,
                   [this](QString /* unused */, QString /* unused */) {
                     this->queryActiveDevices();
                   });
  QObject::connect(m_eventHandler,
                   &HyprEvents::activeWindowChanged,
                   m_toplevelModel,
                   &ToplevelModel::onAddressActivated);
  QObject::connect(m_eventHandler,
                   &HyprEvents::workspacesChanged,
                   this,
                   &Hyprland::queryWorkspaces);

  QObject::connect(toplevels::ToplevelManager::instance(),
                   &toplevels::ToplevelManager::toplevelsChanged,
                   this,
                   &Hyprland::queryHyprClients);
}

void Hyprland::hyprctl(const QByteArray                      &request,
                       const std::function<void(bool,
                                                QByteArray)> &callback) {
  if (m_requestSocketPath.isEmpty()) return;

  auto requestSocket = new QLocalSocket(this);

  auto onConnectedCallback = [this, request, requestSocket, callback]() {
    auto responseCallback = [requestSocket, callback]() {
      auto response = requestSocket->readAll();
      callback(true, std::move(response));
      delete requestSocket;
    };

    QObject::connect(
        requestSocket, &QLocalSocket::readyRead, this, responseCallback);

    requestSocket->write(request);
    requestSocket->flush();
  };

  auto onErrorCallback = [=](QLocalSocket::LocalSocketError error) {
    qCWarning(logNSHyprland)
        << "Error making hyprland request:" << error << "request:" << request;
    requestSocket->deleteLater();
    callback(false, {});
  };

  QObject::connect(
      requestSocket, &QLocalSocket::connected, this, onConnectedCallback);
  QObject::connect(
      requestSocket, &QLocalSocket::errorOccurred, this, onErrorCallback);

  requestSocket->connectToServer(m_requestSocketPath);
}

HyprEvents        *Hyprland::eventHandler() { return m_eventHandler; }
ToplevelModel     *Hyprland::toplevelModel() { return m_toplevelModel; }
WorkspacesModel   *Hyprland::workspacesModel() { return m_workspacesModel; }
HyprMonitorsModel *Hyprland::monitorsModel() { return m_monitorsModel; }

int  Hyprland::keyboardLayoutIndex() const { return m_keyboardLayoutIndex; }
void Hyprland::setKeyboardLayoutIndex(const int &value) {
  if (m_keyboardLayoutIndex == value) return;

  m_keyboardLayoutIndex = value;
  emit keyboardLayoutIndexChanged();
}

void Hyprland::queryHyprInputConfigs() {
  if (m_requestSocketPath.isEmpty()) return;

  if (m_requestingInputConfig) return;
  m_requestingInputConfig = true;

  auto requestSocket = new QLocalSocket(this);

  InputQueryPayload                      payload;
  QList<QPair<QByteArray, QByteArray *>> propMap = {
      {"input.kb_layout",  &payload.kbLayout },
      {"input.kb_variant", &payload.kbVariant},
      {"input.kb_model",   &payload.kbModel  },
      {"input.kb_options", &payload.kbOptions},
      {"input.kb_rules",   &payload.kbRules  }
  };
  auto fetchHyprData = [this, payload, requestSocket](QByteArrayView opt,
                                                      QByteArray    *target) {
    requestSocket->connectToServer(this->m_requestSocketPath);
    requestSocket->waitForConnected(5000);

    if (!requestSocket->isValid()) return false;

    requestSocket->write("j/getoption " + opt);
    requestSocket->waitForBytesWritten(1000);
    requestSocket->waitForReadyRead(1000);
    auto response = requestSocket->readAll();
    // hyprland will forcefully disconnect us, but just to be sure we'll attempt
    // to disconnect
    requestSocket->disconnectFromServer();
    if (requestSocket->state() != QLocalSocket::UnconnectedState) {
      requestSocket->waitForDisconnected(1000);
    }
    target->assign(response);
    return true;
  };

  bool isError = false;
  for (auto &prop : propMap) {
    auto res = fetchHyprData(prop.first, prop.second);
    if (!res || requestSocket->state() != QLocalSocket::UnconnectedState) {
      isError = true;
      qCWarning(logNSHyprland)
          << "error fetching input data from hyprland socket";
      break;
    }
  }

  delete requestSocket;
  if (!isError) this->onInputQueryReadyToRead(std::move(payload));
}

void Hyprland::onInputQueryReadyToRead(InputQueryPayload payload) {
  auto getStringValueFromResponse = [](QByteArray &chars) {
    QJsonParseError parseError;
    auto            jDoc = QJsonDocument::fromJson(chars, &parseError);

    if (parseError.error != QJsonParseError::NoError) return QString();
    if (!jDoc.isObject()) return QString();

    auto jObj = jDoc.object();
    return jObj.value("str").toString();
  };

  auto parsedModel   = getStringValueFromResponse(payload.kbModel);
  auto parsedOptions = getStringValueFromResponse(payload.kbOptions);
  auto parsedRules   = getStringValueFromResponse(payload.kbRules);
  auto parsedLayouts = getStringValueFromResponse(payload.kbLayout).split(",");
  auto parsedVariants =
      getStringValueFromResponse(payload.kbVariant).split(",");

  if (parsedLayouts.isEmpty()) {
    parsedLayouts.append("us");
  }

  m_inputConfig->b_kbModel().setValue(parsedModel);
  m_inputConfig->b_kbOptions().setValue(parsedOptions);
  m_inputConfig->b_kbRules().setValue(parsedRules);
  m_inputConfig->setLayouts(parsedLayouts, parsedVariants);
}

void Hyprland::queryActiveDevices() {
  if (m_requestingDevices) return;
  m_requestingDevices = true;

  this->hyprctl("j/devices", [this](bool success, const QByteArray &response) {
    m_requestingDevices = false;

    if (!success) return;
    if (response.isEmpty()) return;

    QJsonParseError parseError;
    QJsonDocument   jDoc = QJsonDocument::fromJson(response, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
      qCWarning(logNSHyprland)
          << "Device query process: error parsing json data from hyprctl!";
      return;
    }

    if (!jDoc.isObject()) {
      qCWarning(logNSHyprland)
          << "Device query: hyprland returned invalid json.";
      return;
    }

    QJsonObject jObj = jDoc.object();

    auto it = jObj.find("keyboards");
    if (it == jObj.end()) {
      qCWarning(logNSHyprland)
          << "Device query: hyprctl is missing the 'keyboards' field.";
      return;
    }

    if (auto val = it.value(); val.isArray()) {
      QJsonArray keyboards = val.toArray();
      for (const QJsonValue &value : keyboards) {
        if (!value.isObject()) continue;

        auto kbInfo = value.toObject();
        if (!kbInfo.value("main").toBool(false)) continue;

        auto layoutIdx = kbInfo.value("active_layout_index").toInt(0);
        if (layoutIdx != m_keyboardLayoutIndex) {
          m_keyboardLayoutIndex = layoutIdx;
          emit keyboardLayoutIndexChanged();
        }
        break;
      }
    }
  });
}

void Hyprland::queryHyprClients() {
  if (m_requestingToplevels) return;
  m_requestingToplevels = true;

  this->hyprctl("j/clients", [this](bool success, const QByteArray &result) {
    m_requestingToplevels = false;
    m_toplevelModel->handleHyprClientsPayload(result);
  });
}

void Hyprland::queryWorkspaces() {
  if (m_requestingWorkspaces) return;
  m_requestingWorkspaces = true;

  this->hyprctl("j/workspaces", [this](bool success, const QByteArray &result) {
    m_requestingWorkspaces = false;
    m_workspacesModel->updateFromPayload(result);
  });
}

void Hyprland::queryMonitors() {
  if (m_requestingMonitors) return;
  m_requestingMonitors = true;

  this->hyprctl("j/monitors", [this](bool success, const QByteArray &result) {
    m_requestingMonitors = false;
    m_monitorsModel->processMonitorData(result);
  });
}
} // namespace ns::hyprland
