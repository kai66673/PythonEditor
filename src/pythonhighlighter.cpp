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

static void fillFormat(QTextCharFormat &format, const QColor &color, PythonEditor::FontStyle style = PythonEditor::Normal)
{
    format.setForeground(color);

    switch (style) {
        case PythonEditor::Normal:
            break;
        case PythonEditor::Bold:
            format.setFontWeight(QFont::Bold);
            break;
        case PythonEditor::Italic:
            format.setFontItalic(true);
            break;
        case PythonEditor::BoldItalic:
            format.setFontWeight(QFont::Bold);
            format.setFontItalic(true);
            break;
    }
}

PythonHighlighter::PythonHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    fillFormat(formats[PythonEditor::Number],          "brown");
    fillFormat(formats[PythonEditor::String],          "magenta");
    fillFormat(formats[PythonEditor::Keyword],         "blue");
    fillFormat(formats[PythonEditor::Type],            "blueviolet",       PythonEditor::Bold);
    fillFormat(formats[PythonEditor::ClassField],      "black",            PythonEditor::Italic);
    fillFormat(formats[PythonEditor::MagicAttr],       "black",            PythonEditor::BoldItalic);
    fillFormat(formats[PythonEditor::Operator],        "saddlebrown");
    fillFormat(formats[PythonEditor::Braces],          "sandybrown");
    fillFormat(formats[PythonEditor::Comment],         "green");
    fillFormat(formats[PythonEditor::Doxygen],         "darkgreen",        PythonEditor::Bold);
    fillFormat(formats[PythonEditor::Identifier],      "lightslategray");
    fillFormat(formats[PythonEditor::Whitespace],      "gray");
    fillFormat(formats[PythonEditor::ImportedModule],  "darkmagenta",      PythonEditor::Italic);
    fillFormat(formats[PythonEditor::Unknown],         "red",              PythonEditor::BoldItalic);
    fillFormat(formats[PythonEditor::ClassDef],        "olivedrab",        PythonEditor::BoldItalic);
    fillFormat(formats[PythonEditor::FunctionDef],     "olive",            PythonEditor::BoldItalic);
}

void PythonHighlighter::setFormatStyle(PythonEditor::Format fmt, const QColor &color, PythonEditor::FontStyle style)
{
    if (fmt != PythonEditor::FormatsAmount)
        fillFormat(formats[fmt], color, style);
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
        PythonEditor::Format format = tk.format();
        setFormat(tk.begin(), tk.length(), formats[format]);

        if (format == PythonEditor::Keyword && hasOnlyWhitespace) {
            switch (scanner.keywordKind(tk)) {
                case Scanner::ImportOrFrom:
                    highlightImport(scanner);
                    break;
                case  Scanner::Class:
                    highlightDeclarationIdentifier(scanner, formats[PythonEditor::ClassDef]);
                    break;
                case Scanner::Def:
                    highlightDeclarationIdentifier(scanner, formats[PythonEditor::FunctionDef]);
                    break;
            }
        }

        if (format != PythonEditor::Whitespace)
            hasOnlyWhitespace = false;
    }


    return scanner.state();
}

void PythonHighlighter::highlightDeclarationIdentifier(Scanner &scanner, const QTextCharFormat &format)
{
    FormatToken tk = scanner.read();
    while (tk.format() == PythonEditor::Whitespace) {
        setFormat(tk.begin(), tk.length(), formats[PythonEditor::Whitespace]);
        tk = scanner.read();
    }

    if (tk.isEndOfBlock())
        return;
    if (tk.format() == PythonEditor::Identifier)
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
        PythonEditor::Format format = tk.format();
        if (tk.format() == PythonEditor::Identifier)
            format = PythonEditor::ImportedModule;
        setFormat(tk.begin(), tk.length(), formats[format]);
    }
}

} // namespace Internal
} // namespace PythonEditor
