#ifndef CANLIB_GLOBAL_H
#define CANLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CANLIB_LIBRARY)
#  define CANLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CANLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CANLIB_GLOBAL_H
