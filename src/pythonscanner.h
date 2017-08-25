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

#pragma once

#include "pythonformattoken.h"

#include <QString>

namespace PyEditor {
namespace Internal {

/**
 * @brief The Scanner class - scans source code for highlighting only
 */
class Scanner
{
    Scanner(const Scanner &other) = delete;
    void operator=(const Scanner &other) = delete;

public:
    enum State {
        Default = 0,
        StringSingleQuote = 1,
        StringDoubleQuote = 2,
        MultiLineStringSingleQuote = 3,
        MultiLineStringDoubleQuote = 4
    };

    Scanner(const QChar *text, const int length);

    void setState(int state);
    int state() const;
    FormatToken read();
    QString value(const FormatToken& tk) const;

    enum SpecialKeyword {
        ImportOrFrom = 0,
        Class = 1,
        Def = 2,
        Other = 3
    };

    SpecialKeyword keywordKind(const FormatToken &tk) const {
        QString text(m_text + tk.begin(), tk.length());
        if (text == "import" || text == "from")
            return ImportOrFrom;
        if (text == "class")
            return Class;
        if (text == "def")
            return Def;
        return Other;
    }

private:
    FormatToken onDefaultState();

    void checkEscapeSequence(QChar quoteChar);
    FormatToken readStringLiteral(QChar quoteChar);
    FormatToken readMultiLineStringLiteral(QChar quoteChar);
    FormatToken readIdentifier();
    FormatToken readNumber();
    FormatToken readFloatNumber();
    FormatToken readComment();
    FormatToken readDoxygenComment();
    FormatToken readWhiteSpace();
    FormatToken readOther();

    void clearState();

    void setAnchor() { m_markedPosition = m_position; }
    void move() { ++m_position; }
    int length() const { return m_position - m_markedPosition; }
    int anchor() const { return m_markedPosition; }
    bool isEnd() const { return m_position >= m_textLength; }

    QChar peek(int offset = 0) const
    {
        int pos = m_position + offset;
        if (pos >= m_textLength)
            return QLatin1Char('\0');
        return m_text[pos];
    }

    const QChar *m_text;
    const int m_textLength;
    int m_position = 0;
    int m_markedPosition = 0;

    int m_state;
};

} // namespace Internal
} // namespace PythonEditor
