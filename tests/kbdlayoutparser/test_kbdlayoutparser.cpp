#include <QTest>
#include <qabstractitemmodel.h>
#include <qdir.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtestcase.h>
#include <qtmetamacros.h>
#include <qxmlstream.h>

#include "kbd_layouts_model.h"
#include "keyboard_layout_serializer.h"

using namespace ns::utils;

class KbdLayoutTest : public QObject {
  Q_OBJECT

private:
  QString                      evdevFile;
  KeyboardLayoutXMLSerializer *m_serializer = nullptr;
  KbdLayoutsModel             *m_model      = nullptr;

private slots:
  void initTestCase() {
    QString sourceDir{SOURCE_DIR};
    evdevFile = sourceDir + "/evdev.xml";
    QFile evdev{evdevFile};

    QVERIFY(evdev.exists());
  }

  void readData() {
    m_serializer =
        new KeyboardLayoutXMLSerializer(evdevFile.toLocal8Bit().data());

    auto table = m_serializer->getLayoutTable();

    QVERIFY(table.size() == 3);
  }

  void evaluateLayout() {
    const auto layout = m_serializer->getLayoutByName("us");

    QVERIFY(layout != nullptr);

    const auto item = layout->item;

    QVERIFY(item.name == "us");
    QVERIFY(item.shortDescription == "en");
    QVERIFY(item.description == "English (US)");

    const auto variants = layout->variants;

    QVERIFY(variants.size() == 4);
  }

  void evaluateVariants() {
    const auto variant1_opt = m_serializer->getConfigItem("us", "euro");

    QVERIFY(variant1_opt.has_value());

    const auto variant1 = variant1_opt.value();

    QVERIFY(variant1.name == "us" && variant1.shortDescription == "en" &&
            variant1.description == "English (US, euro on 5)");
  }

  void evaluateInvalid() {
    const auto nonExisting = m_serializer->getConfigItem("non", "existing");

    QVERIFY(!nonExisting.has_value());
  }

  void evaluateModel() {
    // TODO: move to separate test unit
    m_model = new KbdLayoutsModel(m_serializer);

    QVERIFY(m_model->rowCount(QModelIndex()) == 3);

    QModelIndex firstChildIdx = m_model->index(0, 0, {});
    auto firstName = m_model->data(firstChildIdx, KbdLayoutsModel::Roles::Name);
    auto description =
        m_model->data(firstChildIdx, KbdLayoutsModel::Roles::Description);
    auto shortDescription =
        m_model->data(firstChildIdx, KbdLayoutsModel::Roles::ShortDescription);
    QVERIFY(firstName == "aaa" && shortDescription == "aa" &&
            description == "Alphabetically First");

    QModelIndex secondChildIdx = m_model->index(1, 0, {});
    auto        secondName =
        m_model->data(secondChildIdx, KbdLayoutsModel::Roles::Name);
    QVERIFY(secondName == "fak");
  }
};

QTEST_MAIN(KbdLayoutTest)
#include "test_kbdlayoutparser.moc"
