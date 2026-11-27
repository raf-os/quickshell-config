#include <QByteArray>
#include <QFile>
#include <QIODevice>
#include <QObject>
#include <QTest>

#include "hypr_configs.h"
#include "hyprctl.h"
#include "keyboard_layout_serializer.h"

using namespace ns::wayland::hyprland;
using namespace ns::utils;

class HyprConfigsTest : public QObject {
  Q_OBJECT

private:
  QString                      evdevFile;
  KeyboardLayoutXMLSerializer *m_kbdSerializer = nullptr;
  Hyprctl                     *m_hyprctl       = nullptr;
  HyprConfigs                 *m_hyprConfigs   = nullptr;

private slots:

  void initTestCase() {
    QString sourceDir{TEST_DATA_DIR};
    evdevFile = sourceDir + "/evdev.xml";
    QFile evdev{evdevFile};

    QVERIFY(evdev.exists());

    m_kbdSerializer =
        new KeyboardLayoutXMLSerializer(evdevFile.toLocal8Bit().data(), this);

    m_hyprctl     = new Hyprctl(QString(), this);
    m_hyprConfigs = new HyprConfigs(m_hyprctl, m_kbdSerializer, this);
  };

  void parseDescriptions() {
    QFile descriptionsFile{
        QStringLiteral(TEST_DATA_DIR) +
        QStringLiteral("/hypr_descriptions.json")};

    QVERIFY(descriptionsFile.exists());
    QVERIFY(descriptionsFile.open(QIODevice::ReadOnly | QIODevice::Text));

    QByteArray data = descriptionsFile.readAll();

    m_hyprConfigs->processConfigData(data);

    QVERIFY(m_hyprConfigs->m_keyboardLayouts.size() == 3);
  };

  void checkIndividualFields() {
    const auto &layouts = m_hyprConfigs->m_keyboardLayouts;

    const auto &firstLayout = layouts.at(0);

    HyprKeyboardLayout firstCompare{
        .layout = "us", .variant = "", .description = "English (US)"};

    QVERIFY(firstLayout == firstCompare);

    const auto &secondLayout = layouts.at(1);

    HyprKeyboardLayout secondCompare{
        .layout      = "us",
        .variant     = "euro",
        .description = "English (US, euro on 5)"};

    QVERIFY(secondLayout == secondCompare);

    const auto        &thirdLayout = layouts.at(2);
    HyprKeyboardLayout thirdCompare{
        .layout = "fak", .variant = "", .description = "Fake Layout"};

    QVERIFY(thirdLayout == thirdCompare);
  }

  void checkActiveDevice() {
    QFile devicesFile{QStringLiteral(TEST_DATA_DIR "/hypr_devices.json")};

    QVERIFY(devicesFile.exists() && devicesFile.open(QIODevice::ReadOnly));

    auto data = devicesFile.readAll();

    m_hyprConfigs->processDeviceData(data);

    QVERIFY(m_hyprConfigs->m_kbdIdx == 1);

    HyprKeyboardLayout activeLayoutTest{
        .layout      = "us",
        .variant     = "euro",
        .description = "English (US, euro on 5)"};

    QVERIFY(m_hyprConfigs->activeKeyboardLayout() == activeLayoutTest);
  }

  // TODO: benchmark parser
};

QTEST_MAIN(HyprConfigsTest)
#include "test_hyprconfigsparser.moc"
