#pragma once
#include <QtCore/QtGlobal>

#if defined(NS_ICONPROVIDER_SHARED_LIB)
#define NS_ICONPROVIDER_EXPORT Q_DECL_EXPORT
#else
#define NS_ICONPROVIDER_EXPORT Q_DECL_IMPORT
#endif
