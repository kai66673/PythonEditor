#pragma once

#include "pythoneditor_global.h"

#include <QPlainTextEdit>

class PYTHONEDITORSHARED_EXPORT PythonEditor : public QPlainTextEdit
{
public:
    PythonEditor(QWidget *parent = 0);
};
