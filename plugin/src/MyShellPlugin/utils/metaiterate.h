#pragma once

#include <functional>
#include <qjsonobject.h>
#include <qmetaobject.h>
#include <qobject.h>
namespace myqmlplugin {
namespace utils {
void iterateMetaObj(QObject *obj, std::function<void(QMetaProperty *)>);
QJsonObject serializeMetaObjToJson(QObject *obj);

} // namespace utils
} // namespace myqmlplugin
