#pragma once

#include <QtCore/qglobal.h>

#if defined(PYTHONEDITOR_LIBRARY)
#  define PYTHONEDITORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define PYTHONEDITORSHARED_EXPORT Q_DECL_IMPORT
#endif
