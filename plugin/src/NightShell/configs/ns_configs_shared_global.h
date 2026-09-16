#pragma once
#include <QtCore/QtGlobal>

#if defined(NS_CONFIGS_SHARED_LIB)
#define NS_CONFIGS_EXPORT Q_DECL_EXPORT
#else
#define NS_CONFIGS_EXPORT Q_DECL_IMPORT
#endif
