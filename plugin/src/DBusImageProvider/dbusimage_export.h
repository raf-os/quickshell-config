#pragma once
#include <QtCore/QtGlobal>

#if defined(DBUSPROVIDER_SHARED_LIB)
#define DBUSPROVIDER_EXPORT Q_DECL_EXPORT
#else
#define DBUSPROVIDER_EXPORT Q_DECL_IMPORT
#endif
