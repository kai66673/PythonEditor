#include "pythoneditor.h"
#include "pythonhighlighter.h"

PythonEditor::PythonEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    new PyEditor::Internal::PythonHighlighter(document());
}
