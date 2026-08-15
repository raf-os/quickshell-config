#pragma once
#include <QtCore/QtGlobal>

#if defined(NS_WAYLAND_SHARED_LIB)
#define NSWAYLAND_EXPORT Q_DECL_EXPORT
#else
#define NSWAYLAND_EXPORT Q_DECL_IMPORT
#endif
