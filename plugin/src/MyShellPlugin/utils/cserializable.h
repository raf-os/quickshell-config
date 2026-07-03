#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin {
namespace configs {
class CSerializable : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("This is an abstract class.")

protected:
  QObject *m_rootObject = nullptr;
  explicit CSerializable(QObject *root, QObject *parent)
      : QObject(parent), m_rootObject(root) {};
  explicit CSerializable(QObject *parent = nullptr)
      : CSerializable{parent, parent} {};

public:
  virtual ~CSerializable() = default;
  QObject *getRoot() const { return m_rootObject; }
};
} // namespace configs
} // namespace myqmlplugin
