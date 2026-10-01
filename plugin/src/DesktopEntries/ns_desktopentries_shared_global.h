#pragma once
#include <QtCore/QtGlobal>

#if defined(NS_DESKTOPENTRIES_SHARED_LIB)
#define NS_DESKTOPENTRIES_EXPORT Q_DECL_EXPORT
#else
#define NS_DESKTOPENTRIES_EXPORT Q_DECL_IMPORT
#endif
