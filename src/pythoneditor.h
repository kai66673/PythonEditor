#pragma once

#include "pythoneditor_global.h"

#include <QPlainTextEdit>

namespace PyEditor {
    namespace Internal {
        class PythonHighlighter;
    }
}

class PYTHONEDITORSHARED_EXPORT PythonEditor : public QPlainTextEdit
{
public:
    PythonEditor(QWidget *parent = 0);

    enum Format {
        Number = 0,
        String,
        Keyword,
        Type,
        ClassField,
        MagicAttr, // magic class attribute/method, like __name__, __init__
        Operator,
        Braces,
        Comment,
        Doxygen,
        Identifier,
        Whitespace,
        ImportedModule,
        Unknown,

        ClassDef,
        FunctionDef,

        FormatsAmount
    };

    enum FontStyle {
        Normal = 0,
        Bold = 1,
        Italic = 2,
        BoldItalic = 3
    };

    void setFormatStyle(PythonEditor::Format fmt, const QColor &color, PythonEditor::FontStyle style = PythonEditor::Normal);

private:
    PyEditor::Internal::PythonHighlighter *m_highlighter;
};
