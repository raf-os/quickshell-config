#pragma once

#include <qtmetamacros.h>

#define AUTO_PROP_IMPL_DECLARE(Type, Name)                                     \
public:                                                                        \
  Type Name() const { return m_##Name; }                                       \
  void set##Name(const Type &value) {                                          \
    if (m_##Name == value)                                                     \
      return;                                                                  \
    m_##Name = value;                                                          \
    emit Name##Changed(value);                                                 \
  }                                                                            \
  Q_SIGNAL void Name##Changed(const Type &value);

#define AUTO_PROP(Type, Name)                                                  \
  AUTO_PROP_IMPL_DECLARE(Type, Name)                                           \
  Q_PROPERTY(Type Name READ Name WRITE set##Name NOTIFY Name##Changed)         \
private:                                                                       \
  Type m_##Name;

#define AUTO_PROP_DEFAULT(Type, Name, Default)                                 \
  AUTO_PROP_IMPL_DECLARE(Type, Name)                                           \
  Q_PROPERTY(Type Name READ Name WRITE set##Name RESET reset##Name NOTIFY      \
                 Name##Changed)                                                \
public:                                                                        \
  Q_INVOKABLE void reset##Name() {                                             \
    if (m_##Name == m_default##Name)                                           \
      return;                                                                  \
    m_##Name = m_default##Name;                                                \
    emit Name##Changed(m_default##Name);                                       \
  }                                                                            \
  Type default##Name() const { return m_default##Name; }                       \
                                                                               \
private:                                                                       \
  const Type m_default##Name = Default;                                        \
  Type m_##Name = Default;
