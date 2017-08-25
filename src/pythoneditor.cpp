#include "pythoneditor.h"
#include "pythonhighlighter.h"

PythonEditor::PythonEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    m_highlighter = new PyEditor::Internal::PythonHighlighter(document());
}

void PythonEditor::setFormatStyle(PythonEditor::Format fmt, const QColor &color, PythonEditor::FontStyle style)
{ m_highlighter->setFormatStyle(fmt, color, style); }
