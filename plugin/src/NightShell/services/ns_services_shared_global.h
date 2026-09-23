#pragma once
#include <QtCore/QtGlobal>

#if defined(NS_SERVICES_SHARED_LIB)
#define NS_SERVICES_EXPORT Q_DECL_EXPORT
#else
#define NS_SERVICES_EXPORT Q_DECL_IMPORT
#endif
