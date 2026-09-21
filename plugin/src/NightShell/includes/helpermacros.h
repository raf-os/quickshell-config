#pragma once

#include <qjsengine.h>
#include <qproperty.h>
#include <qqmlengine.h>
#include <qtmetamacros.h>

#define AUTO_BINDABLE_IMPL_DECLARE(Type, Name)                                 \
  [[nodiscard]] QBindable<Type> bindable_##Name() const { return &b_##Name; }; \
  Q_SIGNAL void                 Name##Changed();

#define AUTO_BINDABLE(Class, Type, Name)                                       \
  Q_PROPERTY(                                                                  \
      Type Name READ default NOTIFY Name##Changed BINDABLE bindable_##Name)    \
public:                                                                        \
  AUTO_BINDABLE_IMPL_DECLARE(Type, Name)                                       \
private:                                                                       \
  Q_OBJECT_BINDABLE_PROPERTY(Class, Type, b_##Name, &Class::Name##Changed)

#define AUTO_BINDABLE_DEFAULT(Class, Type, Name, DefaultValue)                 \
  Q_PROPERTY(Type Name READ default NOTIFY Name##Changed BINDABLE              \
          bindable_##Name RESET reset_##Name)                                  \
public:                                                                        \
  AUTO_BINDABLE_IMPL_DECLARE(Type, Name)                                       \
  void reset_##Name() { b_##Name = DefaultValue; };                            \
                                                                               \
private:                                                                       \
  Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(                                        \
      Class, Type, b_##Name, DefaultValue, &Class::Name##Changed)

#define AUTO_MEYERS_SINGLETON(Class)                                           \
public:                                                                        \
  static Class *instance() {                                                   \
    static Class *s_instance = new Class();                                    \
    return s_instance;                                                         \
  }

#define AUTO_MEYERS_SINGLETON_QML(Class)                                       \
  AUTO_MEYERS_SINGLETON(Class)                                                 \
  static Class *create(QQmlEngine *qmlEngine, QJSEngine * /*unused*/) {        \
    auto inst = instance();                                                    \
    if (qmlEngine)                                                             \
      qmlEngine->setObjectOwnership(inst, QQmlEngine::CppOwnership);           \
    return inst;                                                               \
  }
