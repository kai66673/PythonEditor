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
};

%End