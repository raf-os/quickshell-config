#include "objectmodel.h"

#include <qhash.h>
#include <qnamespace.h>
#include <qstringview.h>

namespace ns::utils {
QHash<int, QByteArray> UntypedObjectModel::roleNames() const {
  return {
      {Qt::UserRole + 1, "modelData"}
  };
}
} // namespace ns::utils
