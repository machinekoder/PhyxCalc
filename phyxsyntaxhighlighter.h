/**************************************************************************
**
** This file is part of PhyxCalc.
**
** PhyxCalc is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** PhyxCalc is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with PhyxCalc.  If not, see <http://www.gnu.org/licenses/>.
**
***************************************************************************/

#ifndef PHYXSYNTAXHIGHLIGHTER_H
#define PHYXSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include "global.h"

class PhyxSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
    Q_PROPERTY(AppSettings *appSettings READ appSettings WRITE setAppSettings)

    typedef struct {
        int line;
        int pos;
        int length;
    } Error;

public:
    PhyxSyntaxHighlighter(QTextDocument *parent = 0);

    void setVariableHighlightingRules(QStringList variableList);
    void setConstantHighlightingRules(QStringList variableList);
    void setUnitHighlightingRules(QStringList unitList);
    void setFunctionHighlightinhRules(QStringList functionList);
    void addError(int line, int pos, int length);
    void removeError(int line, int pos);
    void updateFormats();
    
    AppSettings * appSettings() const
    {
        return m_appSettings;
    }

public slots:
    void setAppSettings(AppSettings * arg)
    {
        m_appSettings = arg;
    }

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRulesPriority1;
    QVector<HighlightingRule> highlightingRulesPriority2;
    QVector<HighlightingRule> variableHighlightingRules;
    QVector<HighlightingRule> constantHighlightingRules;
    QVector<HighlightingRule> unitHighlightingRules;
    QVector<HighlightingRule> functionHighlightingRules;
    QList<Error>              errorList;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat textFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat keywordFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat variablesFormat;
    QTextCharFormat constantsFormat;
    QTextCharFormat errorFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat selectionFormat;
    QTextCharFormat unitFormat;
    
    AppSettings * m_appSettings;

    void highlightRules( const QString &text, const QVector<HighlightingRule> &highlightingRules);
};

#endif // PHYXSYNTAXHIGHLIGHTER_H
