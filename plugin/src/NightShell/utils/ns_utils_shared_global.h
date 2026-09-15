#pragma once
#include <QtCore/QtGlobal>

#if defined(NS_UTILS_SHARED_LIB)
#define NS_UTILS_EXPORT Q_DECL_EXPORT
#else
#define NS_UTILS_EXPORT Q_DECL_IMPORT
#endif
