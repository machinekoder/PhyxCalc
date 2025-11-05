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

#include "lineparser.h"

LineParser::LineParser(QObject *)
{
    m_loading = true;
    m_phyxCalculator = new PhyxCalculator(this);
    connect(m_phyxCalculator, &PhyxCalculator::outputResult,
            this, &LineParser::outputResult);
    connect(m_phyxCalculator, &PhyxCalculator::outputError,
            this, &LineParser::outputError);
    connect(m_phyxCalculator, &PhyxCalculator::outputConverted,
            this, &LineParser::outputConverted);
    connect(m_phyxCalculator, &PhyxCalculator::outputText,
            this, &LineParser::outputText);
    connect(m_phyxCalculator, &PhyxCalculator::variablesChanged,
            this, &LineParser::showVariables);
    connect(m_phyxCalculator, &PhyxCalculator::constantsChanged,
            this, &LineParser::showConstants);
    connect(m_phyxCalculator, &PhyxCalculator::unitsChanged,
            this, &LineParser::updateUnits);
    connect(m_phyxCalculator, &PhyxCalculator::prefixesChanged,
            this, &LineParser::updatePrefixes);
    connect(m_phyxCalculator, &PhyxCalculator::functionsChanged,
            this, &LineParser::updateFunctions);
    connect(m_phyxCalculator, &PhyxCalculator::datasetsChanged,
            this, &LineParser::updateDatasets);
    connect(m_phyxCalculator, &PhyxCalculator::datasetsChanged,
            this, &LineParser::showPlotWindow);
}

LineParser::~LineParser()
{
}

void LineParser::parseLine(bool linebreak)
{
    replaceSymbols();     //replace greek units
    int previousPosition = m_calculationEdit->textCursor().position();  //save cursor position

    QString curLineText = getCurrentLine();     //read current line
    if (curLineText.isEmpty() || !(curLineText.at(0) == '='))
    {
        if (!commentLineSelected())
        {
            m_phyxCalculator->setExpression(curLineText);
            if (!m_phyxCalculator->expression().isEmpty())
                m_phyxCalculator->evaluate();
        }
    }

    if (linebreak)
        insertNewLine();
    else //when Ctrl was pressed, restore cursor
    {
        QTextCursor textCursor = m_calculationEdit->textCursor();
        textCursor.setPosition(previousPosition);
        m_calculationEdit->setTextCursor(textCursor);
    }
}

void LineParser::parseAll()
{
    QTextCursor textCursor = m_calculationEdit->textCursor();
    textCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    m_calculationEdit->setTextCursor(textCursor);

    parseFromCurrentPosition();
}

void LineParser::parseFromCurrentPosition()
{
    QTextCursor textCursor = m_calculationEdit->textCursor();
    textCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
    m_calculationEdit->setTextCursor(textCursor);

    while (!m_calculationEdit->textCursor().atEnd())
    {
        if (commentLineSelected())
            insertNewLine(false);
        else
            parseLine(true);
    }
}

void LineParser::replaceSymbols()
{
    QString curLineText,
            oldLine;

    curLineText = getCurrentLine();
    oldLine = curLineText;

    for (int i = 0; i < m_unitLoader->symbols()->size(); i++)
    {
        curLineText.replace(m_unitLoader->symbols()->at(i).name, m_unitLoader->symbols()->at(i).symbol, Qt::CaseSensitive);
    }

    //if line not unchanged replace current line
    if (curLineText != oldLine)
    {
        replaceCurrentLine(curLineText);
    }
}

QString LineParser::getCurrentLine()
{
    return m_calculationEdit->textCursor().block().text().trimmed();
}

bool LineParser::resultLineSelected()
{
    QString curLineText = getCurrentLine();

    if (!curLineText.isEmpty() && (curLineText.at(0) == '='))
        return true;
    else
        return false;
}

bool LineParser::commentLineSelected()
{
    QRegularExpression commentStart("/\\*");
    QRegularExpression commentEnd("\\*/");
    QString line = getCurrentLine();

    //check for multi line comment
    QString text = m_calculationEdit->toPlainText();

    if ((line.indexOf(commentStart) != -1) || (line.indexOf(commentEnd)))
        return false;

    int cursorPos = m_calculationEdit->textCursor().position();
    int startPos = 0;
    int endPos;

    while (text.indexOf(commentStart,startPos))
    {
        endPos = text.indexOf(commentEnd, startPos);
        if (((cursorPos >= startPos) && (cursorPos < endPos+1)) || (endPos == -1))
            return true;
    }
    return false;
}

QString LineParser::variableToolTip(QString name)
{
    QString output;
    PhyxVariable *variable = m_phyxCalculator->variable(name);
    PhyxCalculator::ResultVariable outputVariable;

    outputVariable = m_phyxCalculator->formatVariable(variable,
                                                (PhyxCalculator::OutputMode)m_appSettings->output.unitMode,
                                                (PhyxCalculator::PrefixMode)m_appSettings->output.prefixMode,
                                                m_appSettings->output.numbers.decimalPrecision,
                                                m_appSettings->output.numbers.format,
                                                m_appSettings->output.imaginaryUnit,
                                                m_appSettings->output.numbers.useFractions);

    output.append(tr("<b>Variable %1</b><br>").arg(name));
    output.append(tr("Value: %1%2").arg(outputVariable.value).arg(outputVariable.unit));
    if (!variable->unit()->isDimensionlessUnit())
        output.append(tr("<br>Dimension: %1").arg(variable->unit()->dimensionString()));

    return output;
}

QString LineParser::constantToolTip(QString name)
{
    QString output;
    name.chop(1);
    PhyxVariable *variable = m_phyxCalculator->constant(name);
    PhyxCalculator::ResultVariable outputVariable;

    outputVariable = m_phyxCalculator->formatVariable(variable,
                                                (PhyxCalculator::OutputMode)m_appSettings->output.unitMode,
                                                (PhyxCalculator::PrefixMode)m_appSettings->output.prefixMode,
                                                m_appSettings->output.numbers.decimalPrecision,
                                                m_appSettings->output.numbers.format,
                                                m_appSettings->output.imaginaryUnit,
                                                m_appSettings->output.numbers.useFractions);

    output.append(tr("<b>Constant %1</b><br>").arg(name));
    output.append(tr("Value: %1%2").arg(outputVariable.value).arg(outputVariable.unit));
    if (!variable->unit()->isDimensionlessUnit())
        output.append(tr("<br>Dimension: %1").arg(variable->unit()->dimensionString()));


    return output;
}

QString LineParser::functionToolTip(QString name)
{
    QString output;
    PhyxVariableManager::PhyxFunction * function = m_phyxCalculator->function(name);

    output.append(tr("<b>Function %1</b>").arg(name));
    if (function != NULL)
    {
        QString parameters;
        for (int i = function->parameterCount()-1; i >= 0; i--)
        {
            if (i < (function->parameterCount()-1))
                parameters.append(", ");
            parameters.append(function->parameters.at(i));
        }
        output.append(tr("<br>Parameters: %1<br>").arg(parameters));
        output.append(tr("Expression: %1").arg(function->expression));
    }

    return output;
}

void LineParser::replaceCurrentLine(QString text)
{
    QTextCursor textCursor = m_calculationEdit->textCursor();
    textCursor.clearSelection();
    textCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
    textCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    textCursor.removeSelectedText();
    textCursor.insertText(text);
    textCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
    m_calculationEdit->setTextCursor(textCursor);
}

void LineParser::insertOutput(QString text)
{
    QTextCursor textCursor = m_calculationEdit->textCursor();

    textCursor.clearSelection();
    textCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
    if (textCursor.atEnd())
    {
        if (!textCursor.block().text().isEmpty())
            textCursor.insertText("\n");
    }
    else
    {
        textCursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);       // move cursor to the next line
        m_calculationEdit->setTextCursor(textCursor);
        QString curLineText = getCurrentLine();

        if (resultLineSelected())   //handle result line
        {
            deleteLine();
            insertNewLine(true);
        }
        else if(!curLineText.isEmpty()) //handle any other line
        {
            textCursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor);
            m_calculationEdit->setTextCursor(textCursor);
            insertNewLine(true);
        }
        textCursor = m_calculationEdit->textCursor();
    }

    //textCursor.insertText(outputString);
    textCursor.insertHtml(text);
    m_calculationEdit->setTextCursor(textCursor);
}

void LineParser::insertNewLine(bool force)
{
    QTextCursor textCursor = m_calculationEdit->textCursor();

    if (!force)
        textCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);

    if (textCursor.atEnd() || force)
        textCursor.insertText("\n");
    else
        textCursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);


    m_calculationEdit->setTextCursor(textCursor);
}

void LineParser::deleteLine()
{
    QTextCursor textCursor = m_calculationEdit->textCursor();
    textCursor.clearSelection();
    textCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
    textCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    textCursor.removeSelectedText();
    textCursor.deletePreviousChar();    //delete newline character
    m_calculationEdit->setTextCursor(textCursor);
}

void LineParser::appendLine(const QString &line)
{
    QTextCursor textCursor = m_calculationEdit->textCursor();
    textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor); //move to the end
    m_calculationEdit->setTextCursor(textCursor);

    insertOutput(line); //insert line

    parseFromCurrentPosition(); //parse the line
}

QString LineParser::replaceVariables(QString expression, bool insertValue, bool insertUnit)
{
    QMapIterator<QString, PhyxVariable*> i(*m_phyxCalculator->variables());

    //get the size of the biggest item
    int maxSize = 0;
    while (i.hasNext()) {
        i.next();
        if (i.key().size() > maxSize)
            maxSize = i.key().size();
    }
    i.toFront();

    //replace vars, beginning with the biggest
    for (int j = maxSize; j > 0; j--)
    {
        while (i.hasNext()) {
             i.next();
             if (i.key().size() == j) {
                 QString variableName = i.key();
                 int pos = expression.indexOf(variableName);
                 if (pos != -1)
                 {
                    QString variableValue;
                    if (insertValue)
                    {
                        variableValue.append(PhyxCalculator::complexToString(i.value()->value(),
                                                                             m_appSettings->output.numbers.decimalPrecision,
                                                                             m_appSettings->output.numbers.format,
                                                                             m_appSettings->output.imaginaryUnit));
                    }
                    if (insertUnit)
                        variableValue.append(i.value()->unit()->symbol());
                    expression.replace(variableName, variableValue);
                 }
             }
         }
        i.toFront();
    }

    return expression;
}

void LineParser::updateSettings()
{
    m_calculationEdit->setFont(m_appSettings->textEditor.font);
    if (m_appSettings->textEditor.useSyntaxHighlighter)
        m_syntaxHighlighter->setDocument(m_calculationEdit->document());
    else
        m_syntaxHighlighter->setDocument(NULL);
    m_syntaxHighlighter->setAppSettings(m_appSettings);
    m_syntaxHighlighter->updateFormats();
    showConstants();
    showVariables();
    updateUnits();
    updatePrefixes();
    updateFunctions();
    updateDatasets();
}

QString LineParser::removeWhitespace(QString string)
{
    string = string.simplified();
    string.replace(" ", "");
    return string;
}

QString LineParser::exportFormelEditor()
{
    QString text;
    QStringList textLines;
    int pos,
        depth;
    QRegularExpression regExp;

    enum ExportTarget {
        MathTarget,
        LatexTarget
    };

    QString subscriptStart,
            subscriptEnd,
            invisibleBraceStart,
            invisibleBraceEnd,
            fractionBrace1Start,
            fractionBrace1End,
            fractionBrace2Start,
            fractionBrace2End,
            fractionOver,
            curlyBraceStart,
            curlyBraceEnd,
            functionPattern;

    ExportTarget target = LatexTarget;

    if ((target == LatexTarget) || (target == MathTarget))
    {
        subscriptStart = "_{";
        subscriptEnd = "}";
        invisibleBraceStart = "{";
        invisibleBraceEnd = "}";
        fractionBrace1Start = "{";
        fractionBrace1End = "}";
        fractionBrace2Start = "{";
        fractionBrace2End = "}";
    }
    if (target == LatexTarget)
    {
        fractionOver = " \\over ";
        curlyBraceStart = " \\{ ";
        curlyBraceEnd = " \\} ";
        functionPattern = "\\operatorname{%1}";
    }
    else if (target == MathTarget)
    {
        fractionOver = " over ";
        curlyBraceStart = " lbrace ";
        curlyBraceEnd = " rbrace ";
        functionPattern = "%1";
    }

    text = m_calculationEdit->toPlainText().trimmed();

    //replace curly braces
    text.replace("{", curlyBraceStart);
    text.replace("}", curlyBraceEnd);

    //append results to the end of line
    textLines = text.split('\n');
    for (int i = 0; i < textLines.size(); i++)
    {
        pos = textLines.at(i).indexOf("->");          //"->" löschen
        if (pos != -1)
            textLines[i] = textLines.at(i).left(pos);
    }
    for (int i = textLines.size()-1; i > -1; i--)
    {
        if (!textLines.at(i).isEmpty() && textLines.at(i).at(0) == '=')
        {
            textLines[i-1].append(textLines.at(i));
            textLines.removeAt(i);
        }
    }
    text = textLines.join("\n");


    //replace functions
    QStringList functions = m_phyxCalculator->functions();
    functions.sort();
    int maxSize = 0;
    for (int i = 0; i < functions.size(); i++)
     {
        if (functions.at(i).size() > maxSize)
            maxSize = functions.at(i).size();
    }
    for (int size = maxSize; size > 0 ; size--)
    {
        for (int i = functions.size()-1; i >= 0; i--)
        {
            QString function = functions.at(i);
            if (function.size() == size)
            {
                if (!(function.contains("sqr",Qt::CaseInsensitive) || function.contains("root", Qt::CaseInsensitive)))
                    text.replace(function, functionPattern.arg(function));
            }
        }
    }

    //Add the _ to Units
    QMapIterator<QString, PhyxVariable*> mapIterator(*m_phyxCalculator->variables());

     //get the size of the biggest item
     maxSize = 0;
     while (mapIterator.hasNext()) {
         mapIterator.next();
         if (mapIterator.key().size() > maxSize)
             maxSize = mapIterator.key().size();
     }
     mapIterator.toFront();

     //replace vars, beginning with the biggest
     for (int j = maxSize; j > 1; j--)
     {
         while (mapIterator.hasNext()) {
              mapIterator.next();
              if (mapIterator.key().size() == j) {
                  QString variableName = mapIterator.key();

                  variableName.insert(1,subscriptStart);            //here the variable names get replaced
                  variableName.append(subscriptEnd);
                  text.replace(mapIterator.key(), variableName);
              }
          }
         mapIterator.toFront();
     }

     //Replace fractions
     textLines = text.split('\n');
     for (int n = 0; n < textLines.size(); n++)
     {
        QString line = textLines.at(n);
        //search for / and replace it with regexp
        regExp.setPattern("[)][^)]*[/][^(]*[(]");
        pos = line.indexOf(regExp);
        while (pos != -1)
        {
            depth = 0;
            for (int i = pos; i >= 0; i--)
            {
                if (line.at(i) == ')')
                {
                    if (depth == 0)
                    {
                        //line[i] = '}';
                        line.replace(i,1,fractionBrace1End);
                        pos+=fractionBrace1End.size()-1;
                    }

                    depth++;
                }
                else if (line.at(i) == '(')
                {
                    if (depth == 1)
                    {
                        line.replace(i,1,fractionBrace1Start);
                        pos+=fractionBrace1Start.size()-1;
                        break;
                    }
                    else
                        depth--;
                }
            }
            depth = 0;
            for (int i = pos+1; i < line.size(); i++)
            {
                if (line.at(i) == '(')
                {
                    if (depth == 0)
                        line.replace(i,1,fractionBrace2Start);

                    depth++;
                }
                else if (line.at(i) == ')')
                {
                    if (depth == 1)
                    {
                        line.replace(i,1,fractionBrace2End);
                        break;
                    }
                    else
                        depth--;
                }
            }

            /*if (target == LatexTarget)
            {
                int secondPos = line.indexOf("/",pos);
                line[secondPos] = ' ';
            }*/
            pos = line.indexOf(regExp, pos+1);
        }

        //complete it with replacing /
        line.replace(QRegularExpression("[ ]*[/][ ]*"), fractionOver);

        textLines[n] = line;
     }
    text = textLines.join("\n");

    //replace greek letters with the formel edit ones
    for (int k = 0; k < m_unitLoader->symbols()->size(); k++)
    {
        if (target == LatexTarget)
        {
            QString latexString = m_unitLoader->symbols()->at(k).name;
            QChar first = latexString.at(0);
            if (first.isUpper())
                latexString = latexString.toLower();
            latexString[0] = first;
            latexString.prepend("\\");
            latexString.append(" ");
            text.replace(m_unitLoader->symbols()->at(k).symbol, latexString, Qt::CaseSensitive);
        }
        else if (target == MathTarget)
        {
            text.replace(m_unitLoader->symbols()->at(k).symbol, "%" + m_unitLoader->symbols()->at(k).name + " ", Qt::CaseSensitive);
        }
    }

    //replace functions
    if (target == LatexTarget)
    {

        text.replace(QRegularExpression("[sS][qQ][rR][tT]?"), "\\sqrt");

        regExp = QRegularExpression("[rR][oO][oO][tT]([\\d\\.]+)");
        int pos = 0;
        QRegularExpressionMatch match = regExp.match(text, pos);
        while (match.hasMatch()) {
            QString replacement = QString("\\sqrt[%1]").arg(match.captured(1));
            text.replace(match.capturedStart(), match.capturedLength(), replacement);
            // Advance position to just after the replaced substring to avoid skipping overlapping matches
            pos = match.capturedStart() + replacement.length();
            match = regExp.match(text, pos);
        }
    }

    //replace the #
    if (target == LatexTarget)
    {
        text.replace("#","\\#");
    }

    //Append the formel editor newline to every line
    textLines = text.split('\n');
    for (int i = 0; i < textLines.size(); i++)
    {
        if (target == LatexTarget)
        {
            textLines[i].prepend("$$ ");
            textLines[i].append(" $$");
        }
        else if (target == MathTarget)
        {
            if (i != textLines.size()-1)
            {

                textLines[i].append(" newline");
                textLines[i] = textLines.at(i).trimmed();
            }
        }
    }
    text = textLines.join("\n");

    return text;
}

void LineParser::showVariables()
{
    if (isLoading())
        return;

    int row = 0;
    QTableWidgetItem *newItem;
    QStringList     variableNames;

    m_variableTable->clearContents();
    m_variableTable->setRowCount(0);

    QMapIterator<QString, PhyxVariable*> i(*m_phyxCalculator->variables());
     while (i.hasNext()) {
         i.next();
         variableNames.append(i.key());

         if (i.key() == "#") //ignore special variable #
             continue;

         PhyxCalculator::ResultVariable variable;
         variable = m_phyxCalculator->formatVariable(i.value(),
                                                     (PhyxCalculator::OutputMode)m_appSettings->output.unitMode,
                                                     (PhyxCalculator::PrefixMode)m_appSettings->output.prefixMode,
                                                     m_appSettings->output.numbers.decimalPrecision,
                                                     m_appSettings->output.numbers.format,
                                                     m_appSettings->output.imaginaryUnit,
                                                     m_appSettings->output.numbers.useFractions);

         m_variableTable->setRowCount(row+1);
         newItem = new QTableWidgetItem(i.key());   // name
         m_variableTable->setItem(row, 0, newItem);
         newItem = new QTableWidgetItem(variable.value);
         m_variableTable->setItem(row, 1, newItem);
         newItem = new QTableWidgetItem(variable.unit);
         m_variableTable->setItem(row, 2, newItem);

         //QCheckBox *checkBox = new QCheckBox();
         //checkBox->setChecked(true);
         //m_variableTable->setCellWidget(0, 0, checkBox);
         row++;
     }

     if (m_syntaxHighlighter != NULL)
         m_syntaxHighlighter->setVariableHighlightingRules(variableNames);
}

void LineParser::showConstants()
{
    if (isLoading())
        return;

    int row = 0;
    QTableWidgetItem *newItem;
    QStringList     constantNames;

    m_constantsTable->clearContents();
    m_constantsTable->setRowCount(0);


    QMapIterator<QString, PhyxVariable*> i(*m_phyxCalculator->constants());
     while (i.hasNext()) {
         i.next();
         PhyxCalculator::ResultVariable variable;
         variable = m_phyxCalculator->formatVariable(i.value(),
                                                     (PhyxCalculator::OutputMode)m_appSettings->output.unitMode,
                                                     (PhyxCalculator::PrefixMode)m_appSettings->output.prefixMode,
                                                     m_appSettings->output.numbers.decimalPrecision,
                                                     m_appSettings->output.numbers.format,
                                                     m_appSettings->output.imaginaryUnit,
                                                     m_appSettings->output.numbers.useFractions);

         m_constantsTable->setRowCount(row+1);
         newItem = new QTableWidgetItem(i.key());   // name
         m_constantsTable->setItem(row, 0, newItem);
         newItem = new QTableWidgetItem(variable.value);
         m_constantsTable->setItem(row, 1, newItem);
         newItem = new QTableWidgetItem(variable.unit);
         m_constantsTable->setItem(row, 2, newItem);

         row++;

         constantNames.append(i.key());
     }

     if (m_syntaxHighlighter != NULL)
         m_syntaxHighlighter->setConstantHighlightingRules(constantNames);
}

void LineParser::updateUnits()
{
    if (isLoading())
        return;

    int row = 0;
    QTableWidgetItem *newItem;
    QStringList     unitNames;

    m_unitsTable->clearContents();
    m_unitsTable->setRowCount(0);

    QMapIterator<QString, PhyxUnit*> i(m_phyxCalculator->units());
     while (i.hasNext()) {
         i.next();
         unitNames.append(i.key());

        QString scaleFactor = PhyxCalculator::complexToString(PhyxValueDataType(i.value()->scaleFactor(), PHYX_FLOAT_NULL),
                                                              m_appSettings->output.numbers.decimalPrecision,
                                                              m_appSettings->output.numbers.format,
                                                              m_appSettings->output.imaginaryUnit,
                                                              false,
                                                              m_appSettings->output.numbers.useFractions);
         QString offset = PhyxCalculator::complexToString(PhyxValueDataType(i.value()->offset(), PHYX_FLOAT_NULL),
                                                          m_appSettings->output.numbers.decimalPrecision,
                                                          m_appSettings->output.numbers.format,
                                                          m_appSettings->output.imaginaryUnit,
                                                          false,
                                                          m_appSettings->output.numbers.useFractions);

         m_unitsTable->setRowCount(row+1);
         newItem = new QTableWidgetItem(i.value()->preferedPrefix() + i.key());   // name
         m_unitsTable->setItem(row, 0, newItem);
         newItem = new QTableWidgetItem(i.value()->dimensionString()); //dimension
         m_unitsTable->setItem(row, 1, newItem);
         newItem = new QTableWidgetItem(scaleFactor); //scale factor
         m_unitsTable->setItem(row, 2, newItem);
         newItem = new QTableWidgetItem(offset); //offset
         m_unitsTable->setItem(row, 3, newItem);
         newItem = new QTableWidgetItem(i.value()->unitGroup()); //unit system
         m_unitsTable->setItem(row, 4, newItem);

         row++;
     }

     if (m_syntaxHighlighter != NULL)
         m_syntaxHighlighter->setUnitHighlightingRules(unitNames);
}

void LineParser::updatePrefixes()
{
    if (isLoading())
        return;

    int row = 0;
    QTableWidgetItem *newItem;
    //QStringList     unitNames;

    m_prefixesTable->clearContents();
    m_prefixesTable->setRowCount(0);

    QList<PhyxUnitSystem::PhyxPrefix> prefixList = m_phyxCalculator->prefixes();
    for (int i = 0; i < prefixList.size(); i++)
    {
         //unitNames.append(i.key());

        QString value = PhyxCalculator::complexToString(PhyxValueDataType(prefixList.at(i).value, PHYX_FLOAT_NULL),
                                                              m_appSettings->output.numbers.decimalPrecision,
                                                              m_appSettings->output.numbers.format,
                                                              m_appSettings->output.imaginaryUnit,
                                                              false);

         m_prefixesTable->setRowCount(row+1);
         newItem = new QTableWidgetItem(prefixList.at(i).symbol);   // symbol
         m_prefixesTable->setItem(row, 0, newItem);
         newItem = new QTableWidgetItem(value); //value
         m_prefixesTable->setItem(row, 1, newItem);
         newItem = new QTableWidgetItem(prefixList.at(i).unitGroup); //unit system
         m_prefixesTable->setItem(row, 2, newItem);

         row++;
     }

     //if (m_syntaxHighlighter != NULL)
     //    m_syntaxHighlighter->setUnitHighlightingRules(unitNames);
}

void LineParser::updateFunctions()
{
    if (isLoading() || m_syntaxHighlighter == NULL)
        return;

    QStringList functionList = m_phyxCalculator->functions();
    m_syntaxHighlighter->setFunctionHighlightinhRules(functionList);

    //remove duplicates
    for (int i = 0; i < functionList.size(); i++)
    {
        if (functionList.at(i).at(0).isLower())
        {
        QString name = functionList.at(i);
        name[0] = name.at(0).toUpper();
        functionList.removeAll(name);
        name = name.toUpper();
        functionList.removeAll(name);
        }

        functionList[i] = tr("%1 function;%1;%1").arg(functionList.at(i));
    }
    functionList.sort();
    listWidgetUpdate(m_functionsList, functionList);
}

void LineParser::updateDatasets()
{
    m_plotWindow->setDatasets(m_phyxCalculator->datasets());
}

void LineParser::showPlotWindow()
{
    if (m_appSettings->plot.autoShowPlotWindow)
        m_plotWindow->show();
}

void LineParser::clearAllVariables()
{
    m_phyxCalculator->clearVariables();
}

void LineParser::outputResult()
{
    PhyxCalculator::ResultVariable result = m_phyxCalculator->formatVariable(m_phyxCalculator->result(),
                                                                             (PhyxCalculator::OutputMode)m_appSettings->output.unitMode,
                                                                             (PhyxCalculator::PrefixMode)m_appSettings->output.prefixMode,
                                                                             m_appSettings->output.numbers.decimalPrecision,
                                                                             m_appSettings->output.numbers.format,
                                                                             m_appSettings->output.imaginaryUnit,
                                                                             m_appSettings->output.numbers.useFractions);
    QString output;
    output.append("=");
    output.append(result.value);
    output.append(result.unit);
    insertOutput(output);
}

void LineParser::outputError()
{
    QString output;
    output.append("=");
    output.append("<font color=red>");
    output.append(m_phyxCalculator->errorString());
    output.append("</font> ");
    insertOutput(output);
}

void LineParser::outputText(QString text)
{
    text.prepend("=");
    insertOutput(text);
}

void LineParser::outputConverted(QString text)
{
    PhyxCalculator::ResultVariable result = m_phyxCalculator->formatVariable(m_phyxCalculator->result(),
                                                                             (PhyxCalculator::OutputMode)m_appSettings->output.unitMode,
                                                                             (PhyxCalculator::PrefixMode)m_appSettings->output.prefixMode,
                                                                             m_appSettings->output.numbers.decimalPrecision,
                                                                             m_appSettings->output.numbers.format,
                                                                             m_appSettings->output.imaginaryUnit,
                                                                             m_appSettings->output.numbers.useFractions);
    QString output;
    output.append("=");
    output.append(result.value);
    output.append(text);
    insertOutput(output);
}
