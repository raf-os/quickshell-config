#include "metaiterate.h"
#include <functional>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qmetaobject.h>
#include <qobject.h>

namespace myqmlplugin {
namespace utils {
void iterateMetaObj(QObject *obj,
                    std::function<void(QMetaProperty *)> callback) {
  auto meta = obj->metaObject();
  for (auto i = meta->superClass()->propertyCount(); i < meta->propertyCount();
       ++i) {
    QMetaProperty prop = meta->property(i);
    callback(&prop);
  }
}

QJsonObject serializeMetaObjToJson(QObject *obj) {
  QJsonObject js;
  iterateMetaObj(obj, [obj, &js](QMetaProperty *meta) {
    js.insert(meta->name(), QJsonValue::fromVariant(meta->read(obj)));
  });
  return js;
}
} // namespace utils
} // namespace myqmlplugin
