#pragma once
#include <QtCore/QtGlobal>

#if defined(NS_WL_INPUT_INHIBIT_SHARED)
#define NSWLII_EXPORT Q_DECL_EXPORT
#else
#define NSWLII_EXPORT Q_DECL_IMPORT
#endif
