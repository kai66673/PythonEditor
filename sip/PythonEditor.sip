%Import QtGui/QtGuimod.sip
%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

class PythonEditor : public QPlainTextEdit
{

%TypeHeaderCode
#include "pythoneditor.h"
%End

public:
    PythonEditor(QWidget *parent /TransferThis/ = 0);
    
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
};

%End