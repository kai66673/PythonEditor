/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

/**
 * @brief The Highlighter class pre-highlights Python source using simple scanner.
 *
 * Highlighter doesn't highlight user types (classes and enumerations), syntax
 * and semantic errors, unnecessary code, etc. It's implements only
 * basic highlight mechanism.
 *
 * Main highlight procedure is highlightBlock().
 */

#include "pythonhighlighter.h"
#include "pythonscanner.h"

namespace PyEditor {
namespace Internal {

/**
 * @class PythonEditor::Internal::PythonHighlighter
 * @brief Handles incremental lexical highlighting, but not semantic
 *
 * Incremental lexical highlighting works every time when any character typed
 * or some text inserted (i.e. copied & pasted).
 * Each line keeps associated scanner state - integer number. This state is the
 * scanner context for next line. For example, 3 quotes begin a multiline
 * string, and each line up to next 3 quotes has state 'MultiLineString'.
 *
 * @code
 *  def __init__:               # Normal
 *      self.__doc__ = """      # MultiLineString (next line is inside)
 *                     banana   # MultiLineString
 *                     """      # Normal
 * @endcode
 */

enum FontStyle {
    Normal = 0,
    Bold = 1,
    Italic = 2,
    BoldItalic = 3
};

static void fillFormat(QTextCharFormat &format, const QColor &color, FontStyle style = Normal)
{
    format.setForeground(color);

    switch (style) {
        case Normal:
            break;
        case Bold:
            format.setFontWeight(QFont::Bold);
            break;
        case Italic:
            format.setFontItalic(true);
            break;
        case BoldItalic:
            format.setFontWeight(QFont::Bold);
            format.setFontItalic(true);
            break;
    }
}

PythonHighlighter::PythonHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    fillFormat(formats[Format_Number],          "brown");
    fillFormat(formats[Format_String],          "magenta");
    fillFormat(formats[Format_Keyword],         "blue");
    fillFormat(formats[Format_Type],            "blueviolet",       Bold);
    fillFormat(formats[Format_ClassField],      "black",            Italic);
    fillFormat(formats[Format_MagicAttr],       "black",            BoldItalic);
    fillFormat(formats[Format_Operator],        "saddlebrown");
    fillFormat(formats[Format_Braces],          "sandybrown");
    fillFormat(formats[Format_Comment],         "green");
    fillFormat(formats[Format_Doxygen],         "darkgreen",        Bold);
    fillFormat(formats[Format_Identifier],      "lightslategray");
    fillFormat(formats[Format_Whitespace],      "gray");
    fillFormat(formats[Format_ImportedModule],  "darkmagenta",      Italic);
    fillFormat(formats[Format_Unknown],         "red",              BoldItalic);
    fillFormat(formats[Format_ClassDef],        "olivedrab",        BoldItalic);
    fillFormat(formats[Format_FunctionDef],     "olive",            BoldItalic);
}

/**
 * @brief PythonHighlighter::highlightBlock highlights single line of Python code
 * @param text is single line without EOLN symbol. Access to all block data
 * can be obtained through inherited currentBlock() function.
 *
 * This function receives state (int number) from previously highlighted block,
 * scans block using received state and sets initial highlighting for current
 * block. At the end, it saves internal state in current block.
 */
void PythonHighlighter::highlightBlock(const QString &text)
{
    int initialState = previousBlockState();
    if (initialState == -1)
        initialState = 0;
    setCurrentBlockState(highlightLine(text, initialState));
}

/**
 * @brief Highlight line of code, returns new block state
 * @param text Source code to highlight
 * @param initialState Initial state of scanner, retrieved from previous block
 * @return Final state of scanner, should be saved with current block
 */
int PythonHighlighter::highlightLine(const QString &text, int initialState)
{
    Scanner scanner(text.constData(), text.size());
    scanner.setState(initialState);

    FormatToken tk;
    bool hasOnlyWhitespace = true;
    while (!(tk = scanner.read()).isEndOfBlock()) {
        Format format = tk.format();
        setFormat(tk.begin(), tk.length(), formats[format]);

        if (format == Format_Keyword && hasOnlyWhitespace) {
            switch (scanner.keywordKind(tk)) {
                case Scanner::ImportOrFrom:
                    highlightImport(scanner);
                    break;
                case  Scanner::Class:
                    highlightDeclarationIdentifier(scanner, formats[Format_ClassDef]);
                    break;
                case Scanner::Def:
                    highlightDeclarationIdentifier(scanner, formats[Format_FunctionDef]);
                    break;
                default:
                    setFormat(tk.begin(), tk.length(), formats[format]);
                    break;
            }
        }

        if (format == Format_Whitespace)
            hasOnlyWhitespace = false;
    }


    return scanner.state();
}

void PythonHighlighter::highlightDeclarationIdentifier(Scanner &scanner, const QTextCharFormat &format)
{
    FormatToken tk = scanner.read();
    while (tk.format() == Format_Whitespace) {
        setFormat(tk.begin(), tk.length(), formats[Format_Whitespace]);
        tk = scanner.read();
    }

    if (tk.isEndOfBlock())
        return;
    if (tk.format() == Format_Identifier)
        setFormat(tk.begin(), tk.length(), format);
    else
        setFormat(tk.begin(), tk.length(), formats[tk.format()]);
}

/**
 * @brief Highlights rest of line as import directive
 */
void PythonHighlighter::highlightImport(Scanner &scanner)
{
    FormatToken tk;
    while (!(tk = scanner.read()).isEndOfBlock()) {
        Format format = tk.format();
        if (tk.format() == Format_Identifier)
            format = Format_ImportedModule;
        setFormat(tk.begin(), tk.length(), formats[format]);
    }
}

} // namespace Internal
} // namespace PythonEditor
