from PyQt5.QtGui import QFont
from PyQt5.QtWidgets import QApplication
from PyPythonEditor import PythonEditor
        
if __name__=="__main__":
    import sys
    
    a=QApplication(sys.argv)
    w=PythonEditor()
    font = QFont()
    font.setFamily('Courier')
    font.setFixedPitch(True)
    font.setPointSize(10)
    w.setFont(font)
    w.show()
    sys.exit(a.exec_())