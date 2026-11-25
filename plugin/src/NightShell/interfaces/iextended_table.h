#include <QByteArrayList>
#include <QHash>
#include <QVariant>
#include <QtPlugin>

class IExtendedTable {
public:
  virtual ~IExtendedTable() = default;

  [[nodiscard]] virtual QByteArrayList props() const            = 0;
  [[nodiscard]] virtual QVariant       valueAt(int idx) const & = 0;
};

Q_DECLARE_INTERFACE(IExtendedTable, "org.nightshell.IExtendedTable")
