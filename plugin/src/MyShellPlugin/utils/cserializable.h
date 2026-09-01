#pragma once

#include <qobject.h>
#include <qtmetamacros.h>

namespace myqmlplugin {
namespace configs {
class CSerializable : public QObject {
  Q_OBJECT

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
