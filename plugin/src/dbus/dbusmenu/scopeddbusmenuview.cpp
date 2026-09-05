#include "scopeddbusmenuview.h"

#include <qabstractitemmodel.h>
#include <qloggingcategory.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qvariant.h>

#include "dbusmenumodel.h"

namespace ns::dbusmenu {
ScopedDBusMenuView::ScopedDBusMenuView(QObject *parent) : QObject(parent) {}

ScopedDBusMenuView::~ScopedDBusMenuView() {
  if (m_model) {
    m_model->removeRef();
  }
}

DBusMenuModel *ScopedDBusMenuView::model() { return m_model; }
void           ScopedDBusMenuView::setModel(DBusMenuModel *newModel) {
  if (m_model == newModel) return;

  if (m_model) {
    m_model->removeRef();
    QObject::disconnect(m_model, nullptr, this, nullptr);
  }

  if (newModel) {
    m_model = newModel;
    newModel->addRef();

    QObject::connect(newModel, &QObject::destroyed, this,
        &ScopedDBusMenuView::onModelDestroyed);

    emit modelChanged();

    tryAttach();
  }
}

void ScopedDBusMenuView::setModelIndex(const QVariant &index) {
  b_loaded  = false;
  m_isValid = false;
  if (!index.canConvert<QModelIndex>()) {
    qCWarning(logNSDBusMenu) << "ScopedDBusMenuView::setModelIndex: variant is "
                                "not convertible to QModelIndex.";
    return;
  }

  m_isValid = true;
  auto idx  = index.toModelIndex();
  if (idx.isValid()) {
    m_menu = static_cast<DBusMenuModelItem *>(idx.internalPointer());
  }

  tryAttach();
}

void ScopedDBusMenuView::tryAttach() {
  if (!m_isValid || !m_model || m_isPending) return;
  if (b_loaded.value()) return;

  if (!m_menu) {
    // menu is implicit, using root item
    m_menu = m_model->rootItem();
    if (!m_menu) return; // how?
  }

  QObject::connect(
      m_menu, &QObject::destroyed, this, &ScopedDBusMenuView::onItemDestroyed);

  m_isPending = true;
  m_model->prepareToShowWithCallback(
      m_menu->id(), this, [this](bool shouldUpdate) {
        m_isPending = false;
        if (!shouldUpdate) {
          this->onLoadedComplete();
          return;
        }

        if (m_menu) {
          QObject::connect(m_menu, &DBusMenuModelItem::layoutUpdated, this,
              &ScopedDBusMenuView::onLoadedComplete);
          return;
        }
      });
}

void ScopedDBusMenuView::onLoadedComplete() {
  b_loaded    = true;
  m_isPending = false;
}

void ScopedDBusMenuView::onModelDestroyed() {
  m_menu   = nullptr;
  m_model  = nullptr;
  b_loaded = false;
  emit modelChanged();
}

void ScopedDBusMenuView::onItemDestroyed() {
  if (m_menu) {
    QObject::disconnect(m_menu, nullptr, this, nullptr);
  }
  m_menu = nullptr;

  if (b_loaded) {
    m_model  = nullptr;
    b_loaded = false;
  }
}
} // namespace ns::dbusmenu
