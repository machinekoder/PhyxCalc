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

#include "phyxcalculator.h"

PhyxCalculator::PhyxCalculator(QObject *parent) :
    QObject(parent)
{
    initialize();
}

void PhyxCalculator::initialize()
{
    //initialize variables
    m_expression = "";
    expressionIsParsable = false;
    valueBuffer = PHYX_FLOAT_ONE;
    prefixBuffer = "";
    unitBuffer = "";
    flagBuffer = 0;
    stackLevel = 0;
    listModeActive = false;
    noGuiUpdate = false;
    m_error = false;
    m_errorNumber = 0;
    m_errorStartPosition = 0;
    m_resultValue = 0;
    m_resultUnit = "";
    m_result = NULL;

    //append 3 empty pointers, currently max 3 variables can be loaded
    variableList.append(NULL);
    variableList.append(NULL);
    variableList.append(NULL);

    //initialize random number generator
    qsrand(QDateTime::currentMSecsSinceEpoch());

    // create earley parser
    earleyParser = new QEarleyParser(this);

    //map functions
    functionMap.insert("valueCheckComplex",     &PhyxCalculator::valueCheckComplex);
    functionMap.insert("valueCheckComplex2",    &PhyxCalculator::valueCheckComplex2);
    functionMap.insert("valueCheckComplex3",    &PhyxCalculator::valueCheckComplex3);
    functionMap.insert("valueCheckComplex2th",  &PhyxCalculator::valueCheckComplex2th);
    functionMap.insert("valueCheckComplex3th",  &PhyxCalculator::valueCheckComplex3th);
    functionMap.insert("valueCheckPositive",    &PhyxCalculator::valueCheckPositive);
    functionMap.insert("valueCheckInteger",     &PhyxCalculator::valueCheckInteger);
    functionMap.insert("valueCheckInteger2",    &PhyxCalculator::valueCheckInteger2);
    functionMap.insert("valueCheckInteger2th",  &PhyxCalculator::valueCheckInteger2th);
    functionMap.insert("valueCheckInteger3th",  &PhyxCalculator::valueCheckInteger3th);
    functionMap.insert("valuePush1",    &PhyxCalculator::valuePush1);
    functionMap.insert("valuePush2",    &PhyxCalculator::valuePush2);
    functionMap.insert("valuePush3",    &PhyxCalculator::valuePush3);
    functionMap.insert("valueCopy",     &PhyxCalculator::valueCopy);
    functionMap.insert("valueAdd",      &PhyxCalculator::valueAdd);
    functionMap.insert("valueSub",      &PhyxCalculator::valueSub);
    functionMap.insert("valueMul",      &PhyxCalculator::valueMul);
    functionMap.insert("valueDiv",      &PhyxCalculator::valueDiv);
    functionMap.insert("valueMod",      &PhyxCalculator::valueMod);
    functionMap.insert("valueNeg",      &PhyxCalculator::valueNeg);
    functionMap.insert("valueNoPow",    &PhyxCalculator::valueNoPow);
    functionMap.insert("valuePow",      &PhyxCalculator::valuePow);
    functionMap.insert("valueSin",      &PhyxCalculator::valueSin);
    functionMap.insert("valueArcsin",   &PhyxCalculator::valueArcsin);
    functionMap.insert("valueCos",      &PhyxCalculator::valueCos);
    functionMap.insert("valueArccos",   &PhyxCalculator::valueArccos);
    functionMap.insert("valueTan",      &PhyxCalculator::valueTan);
    functionMap.insert("valueArctan",   &PhyxCalculator::valueArctan);
    functionMap.insert("valueCot",      &PhyxCalculator::valueCot);
    functionMap.insert("valueArccot",   &PhyxCalculator::valueArccot);
    functionMap.insert("valueSec",      &PhyxCalculator::valueSec);
    functionMap.insert("valueArcsec",   &PhyxCalculator::valueArcsec);
    functionMap.insert("valueCsc",      &PhyxCalculator::valueCsc);
    functionMap.insert("valueArccsc",   &PhyxCalculator::valueArccsc);
    functionMap.insert("valueSinh",     &PhyxCalculator::valueSinh);
    functionMap.insert("valueArcsinh",  &PhyxCalculator::valueArcsinh);
    functionMap.insert("valueCosh",     &PhyxCalculator::valueCosh);
    functionMap.insert("valueArccosh",  &PhyxCalculator::valueArccosh);
    functionMap.insert("valueTanh",     &PhyxCalculator::valueTanh);
    functionMap.insert("valueArctanh",  &PhyxCalculator::valueArctanh);
    functionMap.insert("valueCoth",      &PhyxCalculator::valueCoth);
    functionMap.insert("valueArccoth",   &PhyxCalculator::valueArccoth);
    functionMap.insert("valueSech",      &PhyxCalculator::valueSech);
    functionMap.insert("valueArcsech",   &PhyxCalculator::valueArcsech);
    functionMap.insert("valueCsch",      &PhyxCalculator::valueCsch);
    functionMap.insert("valueArccsch",   &PhyxCalculator::valueArccsch);
    functionMap.insert("valueExp",      &PhyxCalculator::valueExp);
    functionMap.insert("valueLn",       &PhyxCalculator::valueLn);
    functionMap.insert("valueLog10",    &PhyxCalculator::valueLog10);
    functionMap.insert("valueLog2",     &PhyxCalculator::valueLog2);
    functionMap.insert("valueLogn",     &PhyxCalculator::valueLogn);
    functionMap.insert("valueRoot",     &PhyxCalculator::valueRoot);
    functionMap.insert("valueSqrt",     &PhyxCalculator::valueSqrt);
    functionMap.insert("valueAbs",      &PhyxCalculator::valueAbs);
    functionMap.insert("valueMax",      &PhyxCalculator::valueMax);
    functionMap.insert("valueMin",      &PhyxCalculator::valueMin);
    functionMap.insert("valueAvg",      &PhyxCalculator::valueAvg);
    functionMap.insert("valuePi",       &PhyxCalculator::valuePi);
    functionMap.insert("valueE",        &PhyxCalculator::valueE);
    functionMap.insert("valueInt",      &PhyxCalculator::valueInt);
    functionMap.insert("valueTrunc",    &PhyxCalculator::valueTrunc);
    functionMap.insert("valueFloor",    &PhyxCalculator::valueFloor);
    functionMap.insert("valueRound",    &PhyxCalculator::valueRound);
    functionMap.insert("valueCeil",     &PhyxCalculator::valueCeil);
    functionMap.insert("valueSign",     &PhyxCalculator::valueSign);
    functionMap.insert("valueHeaviside",&PhyxCalculator::valueHeaviside);
    functionMap.insert("valueRand",     &PhyxCalculator::valueRand);
    functionMap.insert("valueRandint",  &PhyxCalculator::valueRandint);
    functionMap.insert("valueRandg",    &PhyxCalculator::valueRandg);
    functionMap.insert("valueFaculty",  &PhyxCalculator::valueFaculty);
    functionMap.insert("valueBcd",      &PhyxCalculator::valueBcd);
    functionMap.insert("valueToBcd",    &PhyxCalculator::valueToBcd);
    functionMap.insert("valueAns",      &PhyxCalculator::valueAns);
    functionMap.insert("valueGcd",      &PhyxCalculator::valueGcd);
    functionMap.insert("valueLcm",      &PhyxCalculator::valueLcm);

    functionMap.insert("complexReal",   &PhyxCalculator::complexReal);
    functionMap.insert("complexImag",   &PhyxCalculator::complexImag);
    functionMap.insert("complexArg",    &PhyxCalculator::complexArg);
    functionMap.insert("complexNorm",   &PhyxCalculator::complexNorm);
    functionMap.insert("complexConj",   &PhyxCalculator::complexConj);
    functionMap.insert("complexPolar",  &PhyxCalculator::complexPolar);

    functionMap.insert("logicAnd",      &PhyxCalculator::logicAnd);
    functionMap.insert("logicOr",       &PhyxCalculator::logicOr);
    functionMap.insert("logicNand",     &PhyxCalculator::logicNand);
    functionMap.insert("logicNor",      &PhyxCalculator::logicNor);
    functionMap.insert("logicXand",     &PhyxCalculator::logicXand);
    functionMap.insert("logicXor",      &PhyxCalculator::logicXor);
    functionMap.insert("logicNot",      &PhyxCalculator::logicNot);
    functionMap.insert("logicEqual",    &PhyxCalculator::logicEqual);
    functionMap.insert("logicNotEqual", &PhyxCalculator::logicNotEqual);
    functionMap.insert("logicGreater",  &PhyxCalculator::logicGreater);
    functionMap.insert("logicGreaterOrEqual",      &PhyxCalculator::logicGreaterOrEqual);
    functionMap.insert("logicSmaller",  &PhyxCalculator::logicSmaller);
    functionMap.insert("logicSmallerOrEqual",      &PhyxCalculator::logicSmallerOrEqual);

    functionMap.insert("bitAnd",        &PhyxCalculator::bitAnd);
    functionMap.insert("bitOr",         &PhyxCalculator::bitOr);
    functionMap.insert("bitXor",        &PhyxCalculator::bitXor);
    functionMap.insert("bitInv",        &PhyxCalculator::bitInv);
    functionMap.insert("bitShiftLeft",      &PhyxCalculator::bitShiftLeft);
    functionMap.insert("bitShiftRight",     &PhyxCalculator::bitShiftRight);

    functionMap.insert("conditionIfElse",   &PhyxCalculator::conditionIfElse);

    functionMap.insert("unitCheckDimensionless",    &PhyxCalculator::unitCheckDimensionless);
    functionMap.insert("unitCheckDimensionless2",   &PhyxCalculator::unitCheckDimensionless2);
    functionMap.insert("unitCheckDimensionless2th", &PhyxCalculator::unitCheckDimensionless2th);
    functionMap.insert("unitCheckDimensionless3th", &PhyxCalculator::unitCheckDimensionless3th);
    functionMap.insert("unitCheckConvertible",      &PhyxCalculator::unitCheckConvertible);
    functionMap.insert("unitConvert",   &PhyxCalculator::unitConvert);
    functionMap.insert("unitMul",       &PhyxCalculator::unitMul);
    functionMap.insert("unitDiv",       &PhyxCalculator::unitDiv);
    functionMap.insert("unitPow",       &PhyxCalculator::unitPow);
    functionMap.insert("unitSqrt",      &PhyxCalculator::unitSqrt);
    functionMap.insert("unitRoot",      &PhyxCalculator::unitRoot);
    functionMap.insert("unitClear",     &PhyxCalculator::unitClear);
    functionMap.insert("unitAdd",       &PhyxCalculator::unitAdd);
    functionMap.insert("unitRemove",    &PhyxCalculator::unitRemove);

    functionMap.insert("listAddStart",  &PhyxCalculator::listAddStart);
    functionMap.insert("listSubStart",  &PhyxCalculator::listSubStart);
    functionMap.insert("listMulStart",  &PhyxCalculator::listMulStart);
    functionMap.insert("listDivStart",  &PhyxCalculator::listDivStart);
    functionMap.insert("listModStart",  &PhyxCalculator::listModStart);
    functionMap.insert("listPowStart",  &PhyxCalculator::listPowStart);
    functionMap.insert("listBAndStart", &PhyxCalculator::listBAndStart);
    functionMap.insert("listBOrStart",  &PhyxCalculator::listBOrStart);
    functionMap.insert("listBXorStart", &PhyxCalculator::listBXorStart);
    functionMap.insert("listXandStart", &PhyxCalculator::listXandStart);
    functionMap.insert("listAndStart",  &PhyxCalculator::listAndStart);
    functionMap.insert("listNandStart", &PhyxCalculator::listNandStart);
    functionMap.insert("listXorStart",  &PhyxCalculator::listXorStart);
    functionMap.insert("listOrStart",   &PhyxCalculator::listOrStart);
    functionMap.insert("listNorStart",  &PhyxCalculator::listNorStart);
    functionMap.insert("listValueSave", &PhyxCalculator::listValueSave);
    functionMap.insert("listValueLoad", &PhyxCalculator::listValueLoad);
    functionMap.insert("listValueSwap", &PhyxCalculator::listValueSwap);
    functionMap.insert("listEnd",       &PhyxCalculator::listEnd);

    functionMap.insert("variableAdd",   &PhyxCalculator::variableAdd);
    functionMap.insert("variableRemove",&PhyxCalculator::variableRemove);
    functionMap.insert("variableLoad",  &PhyxCalculator::variableLoad);
    functionMap.insert("variablePreDec",&PhyxCalculator::variablePreDec);
    functionMap.insert("variablePreInc",&PhyxCalculator::variablePreInc);
    functionMap.insert("variablePostDec",&PhyxCalculator::variablePostDec);
    functionMap.insert("variablePostInc",&PhyxCalculator::variablePostInc);
    functionMap.insert("constantAdd",   &PhyxCalculator::constantAdd);
    functionMap.insert("constantRemove",&PhyxCalculator::constantRemove);
    functionMap.insert("constantLoad",  &PhyxCalculator::constantLoad);

    functionMap.insert("functionAdd",   &PhyxCalculator::functionAdd);
    functionMap.insert("functionRemove",&PhyxCalculator::functionRemove);
    functionMap.insert("functionRun",   &PhyxCalculator::functionRun);

    functionMap.insert("bufferUnit",            &PhyxCalculator::bufferUnit);
    functionMap.insert("bufferValue",           &PhyxCalculator::bufferValue);
    functionMap.insert("bufferHex",             &PhyxCalculator::bufferHex);
    functionMap.insert("bufferOct",             &PhyxCalculator::bufferOct);
    functionMap.insert("bufferBin",             &PhyxCalculator::bufferBin);
    functionMap.insert("bufferHexString",       &PhyxCalculator::bufferHexString);
    functionMap.insert("bufferOctString",       &PhyxCalculator::bufferOctString);
    functionMap.insert("bufferBinString",       &PhyxCalculator::bufferBinString);
    functionMap.insert("bufferPrefix",          &PhyxCalculator::bufferPrefix);
    functionMap.insert("bufferString",          &PhyxCalculator::bufferString);
    functionMap.insert("bufferExpression",      &PhyxCalculator::bufferExpression);
    functionMap.insert("bufferUnitGroup",       &PhyxCalculator::bufferUnitGroup);
    functionMap.insert("pushVariable",          &PhyxCalculator::pushVariable);
    functionMap.insert("pushFunctionParameter", &PhyxCalculator::pushFunctionParameter);

    functionMap.insert("setInputOnlyFlag",      &PhyxCalculator::setInputOnlyFlag);

    functionMap.insert("outputVariable",        &PhyxCalculator::outputVariable);
    functionMap.insert("outputString",          &PhyxCalculator::outputString);
    functionMap.insert("outputConvertedUnit",   &PhyxCalculator::outputConvertedUnit);
    functionMap.insert("unitGroupAdd",          &PhyxCalculator::unitGroupAdd);
    functionMap.insert("unitGroupRemove",       &PhyxCalculator::unitGroupRemove);
    functionMap.insert("prefixAdd",             &PhyxCalculator::prefixAdd);
    functionMap.insert("prefixRemove",          &PhyxCalculator::prefixRemove);

    functionMap.insert("lowLevelAdd",                   &PhyxCalculator::lowLevelAdd);
    functionMap.insert("lowLevelRun",                   &PhyxCalculator::lowLevelRun);
    functionMap.insert("lowLevelAssignment",            &PhyxCalculator::lowLevelAssignment);
    functionMap.insert("lowLevelAssignmentRemove",      &PhyxCalculator::lowLevelAssignmentRemove);
    functionMap.insert("lowLevelCombinedAssignmentAdd", &PhyxCalculator::lowLevelCombinedAssignmentAdd);
    functionMap.insert("lowLevelCombinedAssignmentSub", &PhyxCalculator::lowLevelCombinedAssignmentSub);
    functionMap.insert("lowLevelCombinedAssignmentMul", &PhyxCalculator::lowLevelCombinedAssignmentMul);
    functionMap.insert("lowLevelCombinedAssignmentDiv", &PhyxCalculator::lowLevelCombinedAssignmentDiv);
    functionMap.insert("lowLevelCombinedAssignmentMod", &PhyxCalculator::lowLevelCombinedAssignmentMod);
    functionMap.insert("lowLevelCombinedAssignmentAnd", &PhyxCalculator::lowLevelCombinedAssignmentAnd);
    functionMap.insert("lowLevelCombinedAssignmentOr",  &PhyxCalculator::lowLevelCombinedAssignmentOr);
    functionMap.insert("lowLevelCombinedAssignmentXor", &PhyxCalculator::lowLevelCombinedAssignmentXor);
    functionMap.insert("lowLevelCombinedAssignmentShiftLeft",  &PhyxCalculator::lowLevelCombinedAssignmentShiftLeft);
    functionMap.insert("lowLevelCombinedAssignmentShiftRight", &PhyxCalculator::lowLevelCombinedAssignmentShiftRight);
    functionMap.insert("lowLevelOutput",                &PhyxCalculator::lowLevelOutput);

    functionMap.insert("datasetCreate",         &PhyxCalculator::datasetCreate);
    functionMap.insert("datasetCreateStep",     &PhyxCalculator::datasetCreateStep);
    functionMap.insert("datasetLogCreate",      &PhyxCalculator::datasetLogCreate);
    functionMap.insert("datasetLogCreateStep",  &PhyxCalculator::datasetLogCreateStep);

    loadGrammar(":/settings/grammar");
    earleyParser->setStartSymbol("S");

    standardFunctionList.append("sin");
    standardFunctionList.append("arcsin");
    standardFunctionList.append("asin");
    standardFunctionList.append("cos");
    standardFunctionList.append("arccos");
    standardFunctionList.append("acos");
    standardFunctionList.append("tan");
    standardFunctionList.append("arctan");
    standardFunctionList.append("atan");
    standardFunctionList.append("cot");
    standardFunctionList.append("arccot");
    standardFunctionList.append("acot");
    standardFunctionList.append("sec");
    standardFunctionList.append("arcsec");
    standardFunctionList.append("asec");
    standardFunctionList.append("csc");
    standardFunctionList.append("arccsc");
    standardFunctionList.append("acsc");
    standardFunctionList.append("sinh");
    standardFunctionList.append("arcsinh");
    standardFunctionList.append("asinh");
    standardFunctionList.append("cosh");
    standardFunctionList.append("arccosh");
    standardFunctionList.append("acosh");
    standardFunctionList.append("tanh");
    standardFunctionList.append("arctanh");
    standardFunctionList.append("atanh");
    standardFunctionList.append("coth");
    standardFunctionList.append("arccoth");
    standardFunctionList.append("acoth");
    standardFunctionList.append("sech");
    standardFunctionList.append("arcsech");
    standardFunctionList.append("asech");
    standardFunctionList.append("csch");
    standardFunctionList.append("arccsch");
    standardFunctionList.append("acsch");
    standardFunctionList.append("exp");
    standardFunctionList.append("ln");
    standardFunctionList.append("log");
    standardFunctionList.append("root");
    standardFunctionList.append("sqrt");
    standardFunctionList.append("sqr");
    standardFunctionList.append("abs");
    standardFunctionList.append("max");
    standardFunctionList.append("min");
    standardFunctionList.append("avg");
    standardFunctionList.append("int");
    standardFunctionList.append("trunc");
    standardFunctionList.append("floor");
    standardFunctionList.append("round");
    standardFunctionList.append("ceil");
    standardFunctionList.append("sign");
    standardFunctionList.append("heaviside");
    standardFunctionList.append("rand");
    standardFunctionList.append("rnd");
    standardFunctionList.append("randi");
    standardFunctionList.append("rndi");
    standardFunctionList.append("randg");
    standardFunctionList.append("rndg");
    standardFunctionList.append("re");
    standardFunctionList.append("im");
    standardFunctionList.append("arg");
    standardFunctionList.append("norm");
    standardFunctionList.append("conj");
    standardFunctionList.append("polar");
    standardFunctionList.append("bcd");
    standardFunctionList.append("tobcd");
    standardFunctionList.append("ans");
    standardFunctionList.append("gcd");
    standardFunctionList.append("lcm");

    standardFunctionList.append("data");
    standardFunctionList.append("datalog");

    for (int i = standardFunctionList.size()-1; i >= 0 ; i--)
    {
        QString name = standardFunctionList.at(i);
        name[0] = name.at(0).toUpper();
        standardFunctionList.append(name);
        name = name.toUpper();
        standardFunctionList.append(name);
    }

    //initialize unit system
    unitSystem = new PhyxUnitSystem(this);
    connect(unitSystem, &PhyxUnitSystem::unitAdded,
            this, &PhyxCalculator::addUnitRule);
    connect(unitSystem, &PhyxUnitSystem::unitRemoved,
            this, &PhyxCalculator::removeUnitRule);
    connect(unitSystem, &PhyxUnitSystem::prefixAdded,
            this, &PhyxCalculator::addPrefixRule);
    connect(unitSystem, &PhyxUnitSystem::prefixRemoved,
            this, &PhyxCalculator::removePrefixRule);
    connect(unitSystem, &PhyxUnitSystem::unitGroupAdded,
            this, &PhyxCalculator::addUnitGroupRule);
    connect(unitSystem, &PhyxUnitSystem::unitGroupRemoved,
            this, &PhyxCalculator::removeUnitGroupRule);

    //initialize variable manager
    variableManager = new PhyxVariableManager(this);
    connect(variableManager, &PhyxVariableManager::variableAdded,
            this, &PhyxCalculator::addVariableRule);
    connect(variableManager, &PhyxVariableManager::variableRemoved,
            this, &PhyxCalculator::removeVariableRule);
    connect(variableManager, &PhyxVariableManager::constantAdded,
            this, &PhyxCalculator::addConstantRule);
    connect(variableManager, &PhyxVariableManager::constantRemoved,
            this, &PhyxCalculator::removeConstantRule);
    connect(variableManager, &PhyxVariableManager::functionAdded,
            this, &PhyxCalculator::addFunctionRule);
    connect(variableManager, &PhyxVariableManager::functionRemoved,
            this, &PhyxCalculator::removeFunctionRule);

    //initialize special variable #
    PhyxVariable *variable = new PhyxVariable(this);
    variable->setValue(PhyxValueDataType(PHYX_FLOAT_NULL,PHYX_FLOAT_NULL));
    variableManager->addVariable("#", variable);
}

void PhyxCalculator::loadGrammar(QString fileName)
{
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QStringList lines = QString::fromUtf8(file.readAll()).split('\n');
        foreach (QString line, lines)
        {
            if (line.trimmed().isEmpty() || (line.trimmed().at(0) == '#'))
                continue;

            if (line.contains("//"))
                line.truncate(line.indexOf("//"));

            QStringList ruleData = line.split(';');
            QString rule;
            QString functions;

            for (int i = ruleData.size()-2; i >= 0; i--)    // handle termination of ;
            {
                if (ruleData.at(i).at(ruleData.at(i).size()-1) == '\\')
                {
                    if (ruleData.size() > (i+1))
                    {
                        ruleData[i].chop(1);
                        ruleData[i].append(';');
                        ruleData[i].append(ruleData.at(i+1));
                        ruleData.removeAt(i+1);
                    }
                }
            }

            rule = ruleData.at(0).trimmed();
            if (ruleData.size() > 1)
                functions = ruleData.at(1).trimmed();

            addRule(rule, functions);
        }
    }
    else
        qFatal("Can't open file");
}

QString PhyxCalculator::stripComments(QString text)
{
    //strip singe line comment
    int pos = text.indexOf("//");
    if (pos != -1)
        text = text.left(pos);
    //strip multi line comments
    pos = text.indexOf("/*");
    if (pos != -1)
        text = text.left(pos);
    pos = text.indexOf("*/");
    if (pos != -1)
        text = text.mid(pos+2);
    return text;
}

QString PhyxCalculator::removeWhitespace(QString text, QList<int> *whiteSpaceList)
{
    int whitespaceCount = 0;
    QString output;

    whiteSpaceList->clear();
    for (int i = text.size() - 1; i >= 0; i--)
    {
        if (text.at(i).isSpace())
            whitespaceCount++;
        else
        {
            output.prepend(text.at(i));
            whiteSpaceList->prepend(whitespaceCount);
        }
    }

    for (int i = 0; i < whiteSpaceList->size(); i++)
        (*whiteSpaceList)[i] = whitespaceCount - whiteSpaceList->at(i);

    return output;
}

int PhyxCalculator::restoreErrorPosition(int pos, QList<int> whiteSpaceList)
{
    return pos + whiteSpaceList.at(pos);
}

void PhyxCalculator::raiseException(int errorNumber)
{
    m_error = true;
    m_errorNumber = errorNumber;
    clearStack();
    if (!noGuiUpdate)
        emit outputError();
#ifdef QT_DEBUG
    qDebug() << "error occured:" << m_errorNumber << errorString();
#endif
}

QString PhyxCalculator::errorString() const
{
    QString output;

    switch (m_errorNumber)
    {
    case SyntaxError:       output.append(tr("Syntax Error!"));
                            break;
    case ValueComplexError: output.append(tr("Value is complex"));
                            break;
    case ValueNotPositiveError: output.append(tr("Value is negative"));
                            break;
    case ValueNotIntegerError: output.append(tr("Only integer values"));
                            break;
    case UnitNotDimensionlessError: output.append(tr("Unit is not dimensionless"));
                            break;
    case UnitsNotConvertibleError: output.append(tr("Units not convertible"));
                            break;
    case PrefixError:       output.append(tr("Prefix does not fit unit"));
                            break;
    case UnknownVariableError:  output.append(tr("Unknown variable"));
                            break;
    case ProgramError:      output.append(tr("Calculation error"));
                            break;
    }

    return tr("error at position %1-%2: %3").arg(m_errorStartPosition).arg(m_errorEndPosition).arg(output);
}

void PhyxCalculator::addRule(QString rule, QString functions)
{
    PhyxRule phyxRule;
    if (!functions.isEmpty())
        phyxRule.functions = functions.split(',');
    phyxRules.insert(rule, phyxRule);

    QStringList ruleFunctions;
    foreach (QString function, phyxRule.functions)
        ruleFunctions.append(function.trimmed());
    earleyParser->loadRule(rule, ruleFunctions);
}

void PhyxCalculator::addUnitRule(QString symbol)
{
    addRule(QString("unit=%1").arg(symbol), QString("bufferParameter, bufferUnit"));
    if (!noGuiUpdate)
        emit unitsChanged();
}

void PhyxCalculator::removeUnitRule(QString symbol)
{
    earleyParser->removeRule(QString("unit=%1").arg(symbol));
    if (!noGuiUpdate)
        emit unitsChanged();
}

void PhyxCalculator::addVariableRule(QString name)
{
    addRule(QString("variable=%1").arg(name), QString("bufferParameter, variableLoad"));
    if (!noGuiUpdate)
        emit variablesChanged();
}

void PhyxCalculator::removeVariableRule(QString name)
{
    earleyParser->removeRule(QString("variable=%1").arg(name));
    if (!noGuiUpdate)
        emit variablesChanged();
}

void PhyxCalculator::addConstantRule(QString name)
{
    addRule(QString("constant=%1").arg(name), QString("bufferParameter, constantLoad"));
    if (!noGuiUpdate)
        emit constantsChanged();
}

void PhyxCalculator::removeConstantRule(QString name)
{
    earleyParser->removeRule(QString("constant=%1").arg(name));
    if (!noGuiUpdate)
        emit constantsChanged();
}

void PhyxCalculator::addPrefixRule(QString symbol)
{
    addRule(QString("prefix=%1").arg(symbol), QString("bufferParameter, bufferPrefix"));
    if (!noGuiUpdate)
        emit prefixesChanged();
}

void PhyxCalculator::removePrefixRule(QString symbol)
{
    earleyParser->removeRule(QString("prefix=%1").arg(symbol));
    if (!noGuiUpdate)
        emit prefixesChanged();
}

void PhyxCalculator::addUnitGroupRule(QString name)
{
    addRule(QString("unitGroup=%1").arg(name), QString("bufferParameter, bufferUnitGroup"));
}

void PhyxCalculator::removeUnitGroupRule(QString name)
{
    earleyParser->removeRule(QString("unitGroup=%1").arg(name));
}

void PhyxCalculator::addFunctionRule(QString name, int parameterCount)
{
    QString ruleString;
    ruleString.append(QString("custom_function=%1").arg(name));
    if (parameterCount == 1)
    {
        ruleString.append("|funcParam|");
    }
    else if (parameterCount > 1)
    {
        ruleString.append("(");
        for (int i = 0; i < parameterCount; i++)
        {
            if (i > 0)
                ruleString.append(",");
            ruleString.append("|p3|");
        }
        ruleString.append(")");
    }
    addRule(ruleString, QString("bufferParameter, functionRun"));
    if (!noGuiUpdate)
        emit functionsChanged();
}

void PhyxCalculator::removeFunctionRule(QString name, int parameterCount)
{
    QString ruleString;
    ruleString.append(QString("custom_function=%1").arg(name));
    if (parameterCount == 1)
    {
        ruleString.append("|funcParam|");
    }
    else if (parameterCount > 1)
    {
        ruleString.append("(");
        for (int i = 0; i < parameterCount; i++)
        {
            if (i > 0)
                ruleString.append(",");
            ruleString.append("|p3|");
        }
        ruleString.append(")");
    }
    earleyParser->removeRule(ruleString);
    if (!noGuiUpdate)
        emit functionsChanged();
}

void PhyxCalculator::clearStack()
{
    foreach (PhyxVariable *variable, variableStack)
    {
        if (variable)
        {
            variable->deleteLater();
            variable = NULL;
        }
    }
    variableStack.clear();
    lowLevelStack.clear();
    functionParameterStack.clear();
}

void PhyxCalculator::clearResult()
{
    m_resultValue = PhyxValueDataType(PHYX_FLOAT_NULL,PHYX_FLOAT_NULL);
    m_resultUnit = "";
}

void PhyxCalculator::clearFlags()
{
    flagBuffer = 0;
}

bool PhyxCalculator::setExpression(QString expression)
{
    expression = stripComments(expression);
    expression = removeWhitespace(expression, &expressionWhitespaceList);

    if (expression.isEmpty())
    {
        earleyParser->clearWord();
        expressionIsParsable = false;
        if (listModeActive)
        {
            listValueLoad();
            listEnd();
            stackLevel = 1; //enable output
            outputVariable();
            stackLevel = 0;
        }
    }
    else if (!m_expression.isEmpty() && expression.indexOf(m_expression) == 0)      //new expression is old expression + string
    {
        QString string = expression.mid(m_expression.size());
        foreach (QChar character, string)
        {
            expressionIsParsable = earleyParser->addSymbol(character);
        }
    }
    else if (m_expression.indexOf(expression) == 0) //new expression is old expression - string
    {
        int  count = m_expression.size() - expression.size();
        for (int i = 0; i < count; i++)
        {
            expressionIsParsable = earleyParser->removeSymbol();
        }
    }
    else
    {
        expressionIsParsable = earleyParser->parseWord(expression);
    }

    m_expression = expression;
    return expressionIsParsable;
}

bool PhyxCalculator::evaluate()
{
    if (expressionIsParsable)
    {
        clearResult();
        m_error = false;
        return evaluate(earleyParser->getTree(), m_expression, expressionWhitespaceList);
    }
    else
    {
        raiseException(SyntaxError);
        return false;
    }
}

bool PhyxCalculator::evaluate(QList<EarleyTreeItem> earleyTree, const QString expression, const QList<int> whiteSpaceList)
{
    stackLevel++;
#ifdef QT_DEBUG
        qDebug() << "evaluating expression:" << expression;
        qDebug() << "stack level:" << stackLevel;
#endif
    for (int i = (earleyTree.size()-1); i >= 0; i--)
    {
        EarleyTreeItem *earleyTreeItem = &earleyTree[i];
        //PhyxRule phyxRule = phyxRules.value(earleyTreeItem->rule);

        m_errorStartPosition = restoreErrorPosition(earleyTreeItem->startPos, whiteSpaceList);     //just in case
        m_errorEndPosition   = restoreErrorPosition(earleyTreeItem->endPos, whiteSpaceList)+1;
        //if (!phyxRule.functions.isEmpty())
        //if (!earleyTreeItem->rule->functions.isEmpty())
        //{
            foreach (QString function, earleyTreeItem->rule->functions)
            {
                if (this->hasError())
                {
                    stackLevel--;
#ifdef QT_DEBUG
                    qDebug() << "returned to stack level:" << stackLevel;
#endif
                    return false;
                }

                if (function == "bufferParameter")
                    parameterBuffer = expression.mid(earleyTreeItem->startPos, earleyTreeItem->endPos - earleyTreeItem->startPos + 1);
                else
                {
#ifdef QT_DEBUG
                    qDebug() << function;
                    if (functionMap.value(function, NULL) != NULL)
#endif
                        (this->*functionMap.value(function))();
#ifdef QT_DEBUG
                    else
                        qFatal("Function %s not found!", function.toLatin1().constData());
#endif
                }
            }
        //}
    }
    stackLevel--;
#ifdef QT_DEBUG
    qDebug() << "returned to stack level:" << stackLevel;
#endif
    return true;
}

bool PhyxCalculator::evaluate(PhyxCalculator::ExpressionCacheItem cacheItem, const QList<int> whiteSpaceList)
{
    stackLevel++;
#ifdef QT_DEBUG
        qDebug() << "evaluating expression:" << cacheItem.expression;
        qDebug() << "stack level:" << stackLevel;
#endif
    for (int i = 0; i < cacheItem.size(); i++)
    {
        m_errorStartPosition = restoreErrorPosition(cacheItem.startPosList.at(i), whiteSpaceList);     //just in case
        m_errorEndPosition   = restoreErrorPosition(cacheItem.endPosList.at(i), whiteSpaceList)+1;

        if (this->hasError())
        {
            stackLevel--;
#ifdef QT_DEBUG
            qDebug() << "returned to stack level:" << stackLevel;
#endif
            return false;
        }

        QString function = cacheItem.function(i);

        if (function == "bufferParameter")
            parameterBuffer = cacheItem.parameter(i);
        else
        {
#ifdef QT_DEBUG
            qDebug() << function;

        if (functionMap.value(function, NULL) != NULL)
#endif
            (this->*functionMap.value(function))();
#ifdef QT_DEBUG
        else
             qFatal("Function %s not found!", function.toLatin1().constData());
#endif
        }
    }
    stackLevel--;
#ifdef QT_DEBUG
    qDebug() << "returned to stack level:" << stackLevel;
#endif
    return true;
}

void PhyxCalculator::loadFile(QString fileName)
{
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QStringList lines = QString::fromUtf8(file.readAll()).split('\n');
        foreach (QString line, lines)
        {
            if (line.contains("//"))
                line.truncate(line.indexOf("//"));

            if (line.trimmed().isEmpty() || (line.trimmed().at(0) == '#'))
                continue;

            this->setExpression(line.trimmed());
            if (!this->evaluate())
                qDebug() << line;
        }
    }
}

PhyxVariable *PhyxCalculator::variable(QString name) const
{
    return variableManager->getVariable(name);
}

PhyxVariable *PhyxCalculator::constant(QString name) const
{
    return variableManager->getConstant(name);
}

PhyxUnit *PhyxCalculator::unit(QString symbol) const
{
    return unitSystem->unit(symbol);
}

PhyxUnitSystem::PhyxPrefix PhyxCalculator::prefix(QString symbol, QString unitGroup) const
{
    return unitSystem->prefix(symbol, unitGroup);
}

PhyxVariableManager::PhyxFunction *PhyxCalculator::function(QString name) const
{
    return variableManager->getFunction(name);
}

PhyxVariableManager::PhyxDataset *PhyxCalculator::dataset(int index) const
{
    return variableManager->getDataset(index);
}

void PhyxCalculator::clearVariables()
{
    variableManager->clearVariables();
}

PhyxVariableManager::PhyxVariableMap *PhyxCalculator::variables() const
{
    return variableManager->variables();
}

PhyxVariableManager::PhyxVariableMap *PhyxCalculator::constants() const
{
    return variableManager->constants();
}

PhyxUnitSystem::PhyxUnitMap PhyxCalculator::units() const
{
    return unitSystem->units();
}

QList<PhyxUnitSystem::PhyxPrefix> PhyxCalculator::prefixes() const
{
    return unitSystem->prefixes();
}

QStringList PhyxCalculator::functions() const
{
    QStringList functionList;
    functionList.append(standardFunctionList);
    PhyxVariableManager::PhyxFunctionMap *functionMap = variableManager->functions();
    QMapIterator<QString, PhyxVariableManager::PhyxFunction*> mapIterator(*functionMap);
    while (mapIterator.hasNext())
    {
        mapIterator.next();
        functionList.append(mapIterator.key());
    }

    return functionList;
}

PhyxVariableManager::PhyxDatasetList *PhyxCalculator::datasets() const
{
    return variableManager->datasets();
}

QString PhyxCalculator::complexToString(const PhyxValueDataType number,
                                        int precision,
                                        char numberFormat,
                                        QString imaginaryUnit,
                                        bool useBraces,
                                        bool useFractions)
{
    QString string;
    boost::format format(QString("%.%1%2").arg(precision).arg(numberFormat).toStdString());
    int components = 0;
    //bool useInteger = false;

    //check wheter number is integer or not
    /*if (number.imag() == PHYX_FLOAT_NULL)
    {
        if (static_cast<PhyxFloatDataType>(static_cast<PhyxIntegerDataType>(number.real())) == number.real())
            useInteger = true;
    }*/

    if (number.real() != PHYX_FLOAT_NULL)
    {
        std::ostringstream ss;
        if (!useFractions)
        {
            //if (!useInteger)
            ss << format % number.real();
    //      else
    //            ss << format % static_cast<PhyxIntegerDataType>(number.real());
            string.append(QString::fromStdString(ss.str()));
        }
        else
        {
            PhyxFraction fraction = decimalToFraction(number.real(),precision);
            ss << format % fraction.numerator;
            string.append(QString::fromStdString(ss.str()));
            if (fraction.denominator != PHYX_FLOAT_ONE)
            {
                string.append("/");
                ss.str("");
                ss << format % fraction.denominator;
                string.append(QString::fromStdString(ss.str()));
            }
        }
        components++;
    }

    if (number.imag() != PHYX_FLOAT_NULL)
    {
        if (!string.isEmpty())
            string.append("+");
        if ((number.imag() != 1) && (number.imag() != -1))
        {
            std::ostringstream ss;
            if (!useFractions)
            {
                ss << format % number.imag();
                string.append(QString::fromStdString(ss.str()));
            }
            else
            {
                PhyxFraction fraction = decimalToFraction(number.imag(),precision);
                ss << format % fraction.numerator;
                string.append(QString::fromStdString(ss.str()));
                if (fraction.denominator != PHYX_FLOAT_ONE)
                {
                    string.append("/");
                    ss.str("");
                    ss << format % fraction.denominator;
                    string.append(QString::fromStdString(ss.str()));
                }
            }
        }
        else if (number.imag() == -1)
        {
            string.append("-");
        }
        string.append(imaginaryUnit);
        components++;
    }

    string.replace("+-","-");

    if (string.isEmpty())
        string.append("0");
    else if ((components == 2) && useBraces)
    {
        string.prepend("(");
        string.append(")");
    }

    return string;
}

PhyxValueDataType PhyxCalculator::stringToComplex(QString string)
{
    PhyxValueDataType value;

    if (string.indexOf(QRegularExpression("[ij]")) != -1)
    {
        string.remove(QRegularExpression("[ij]"));
        if (string.isEmpty())
            string="1";

        PhyxFloatDataType tmpValue;
        std::istringstream inStream(string.toStdString());
        inStream >> tmpValue;
        value = PhyxValueDataType(PHYX_FLOAT_NULL,tmpValue);
    }
    else
    {
        PhyxFloatDataType tmpValue;
        std::istringstream inStream(string.toStdString());
        inStream >> tmpValue;
        value = PhyxValueDataType(tmpValue,PHYX_FLOAT_NULL);
    }

    return value;
}

PhyxIntegerDataType PhyxCalculator::hexToLongInt(QString string)
{
    PhyxIntegerDataType value = 0;
    PhyxIntegerDataType n = 1;

    string.remove("0x");
    for (int i = string.size()-1; i >= 0; i--)
    {
        if (string.at(i).isNumber())
            value += (string.at(i).toLatin1()-48)*n;
        else
            value += (string.at(i).toLatin1()-55)*n;
        n *= 16;
    }

    return value;
}

PhyxIntegerDataType PhyxCalculator::octToLongInt(QString string)
{
    PhyxIntegerDataType value = 0;
    PhyxIntegerDataType n = 1;

    string.remove("0o");
    for (int i = string.size()-1; i >= 0; i--)
    {
        value += (string.at(i).toLatin1()-48)*n;
        n *= 8;
    }

    return value;
}

long PhyxCalculator::binToLongInt(QString string)
{
    PhyxIntegerDataType value = 0;
    PhyxIntegerDataType n = 1;

    string.remove("0b");
    for (int i = string.size()-1; i >= 0; i--)
    {
        value += (string.at(i).toLatin1()-48)*n;
        n *= 2;
    }

    return value;
}

PhyxIntegerDataType PhyxCalculator::bcdToLongInt(PhyxIntegerDataType number)
{
    PhyxIntegerDataType output = 0;
    int i = 0;

    while (number != 0)
    {
        PhyxIntegerDataType digit = number & 0x0F;
        output += pow(PHYX_FLOAT_TEN,i) * digit;

        number = number >> 4;
        i++;
    }
    return output;
}

QString PhyxCalculator::longIntToHex(PhyxIntegerDataType number)
{
    QString output;
    int i = 0;

    while (number != 0)
    {
        int rest = number % 16;
        if (rest < 10)
            output.prepend(QChar::fromLatin1(rest+48));
        else
            output.prepend(QChar::fromLatin1(rest+55));
        number /= 16;
        i++;
    }

    if ((i % 2) != 0)
        output.prepend("0");

    output.prepend("0x");
    return output;
}

QString PhyxCalculator::longIntToOct(PhyxIntegerDataType number)
{
    QString output;

    while (number != 0)
    {
        int rest = number % 8;
        output.prepend(QChar::fromLatin1(rest+48));
        number /= 8;
    }

    output.prepend("0o");
    return output;
}

QString PhyxCalculator::longIntToBin(PhyxIntegerDataType number)
{
    QString output;
    int i = 0;

    while (number != 0)
    {
        int rest = number % 2;
        output.prepend(QChar::fromLatin1(rest+48));
        number /= 2;
        i++;
    }

    while ((i % 8) != 0)
    {
        output.prepend("0");
        i++;
    }

    output.prepend("0b");
    return output;
}

PhyxIntegerDataType PhyxCalculator::longIntToBcd(PhyxIntegerDataType number)
{
    PhyxIntegerDataType output = 0;
    int i = 0;

    while (number != 0)
    {
        PhyxIntegerDataType digit = number % 10;
        output |= digit << (i*4);

        number /= 10;
        i++;
    }
    return output;
}

PhyxIntegerDataType PhyxCalculator::gcd(PhyxIntegerDataType x, PhyxIntegerDataType y)
{
    //euclidean algorithm
    while (x != y)
    {
        if (x < y)
            y -= x;
        else
            x -= y;
    }
    return x;
}

PhyxIntegerDataType PhyxCalculator::lcm(PhyxIntegerDataType x, PhyxIntegerDataType y)
{
    return (abs(x)/gcd(x,y))*abs(y);
}

PhyxFloatDataType PhyxCalculator::toInt(PhyxFloatDataType x)
{
    return static_cast<PhyxFloatDataType>(static_cast<PhyxIntegerDataType>(x));
}

PhyxCalculator::PhyxFraction PhyxCalculator::decimalToFraction(PhyxFloatDataType decimal, PhyxFloatDataType accuracyFactor)
{
    //source: http://www.google.at/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cts=1331720360130&ved=0CCwQFjAA&url=http%3A%2F%2Fhomepage.smc.edu%2Fkennedy_john%2FDEC2FRAC.PDF&ei=bmZgT6KWJZHCtAbZs6CVBg&usg=AFQjCNFtOSqNWlkYGJt0iY89ziqgssxF3w
    PhyxFloatDataType decimalSign;
    PhyxFloatDataType z;
    PhyxFloatDataType previousDenominator;
    PhyxFloatDataType scratchValue;
    //PhyxFloatDataType fractionNumerator;
    //PhyxFloatDataType fractionDenominator;
    PhyxFraction fraction;

    accuracyFactor = pow(PHYX_FLOAT_TEN, -accuracyFactor);

    if (decimal < PHYX_FLOAT_NULL)      //save the sign
        decimalSign = -PHYX_FLOAT_ONE;
    else
        decimalSign = PHYX_FLOAT_ONE;

    decimal = fabs(decimal);

    if (decimal == toInt(decimal))   //handle integers
    {
        fraction.numerator = decimal*decimalSign;
        fraction.denominator = PHYX_FLOAT_ONE;
        return fraction;
    }
    if (decimal < FRACTION_MIN)
    {
        fraction.numerator = decimalSign;
        fraction.denominator = FRACTION_BIGGEST;
        return fraction;
    }
    if (decimal > FRACTION_MAX)
    {
        fraction.numerator = FRACTION_BIGGEST*decimalSign;
        fraction.denominator = PHYX_FLOAT_ONE;
        return fraction;
    }
    z = decimal;
    previousDenominator = PHYX_FLOAT_NULL;
    fraction.denominator = PHYX_FLOAT_ONE;
    do
    {
        z = PHYX_FLOAT_ONE/(z-toInt(z));
        scratchValue = fraction.denominator;
        fraction.denominator = fraction.denominator * toInt(z)+previousDenominator;
        previousDenominator = scratchValue;
        fraction.numerator = toInt(decimal*fraction.denominator + (PHYX_FLOAT_ONE/PHYX_FLOAT_TWO)); //rounding
    }
    while (!((fabs(decimal-(fraction.numerator/fraction.denominator)) < accuracyFactor) || (z == toInt(z))));
    fraction.numerator = decimalSign*fraction.numerator;
    return fraction;
}

PhyxUnitSystem::PhyxPrefix PhyxCalculator::getBestPrefix(PhyxFloatDataType value, PhyxFloatDataType power, QString unitGroup, QString preferedPrefix) const
{
    PhyxFloatDataType preferedPrefixValue = PHYX_FLOAT_ONE;
    if (!preferedPrefix.isEmpty())
        preferedPrefixValue = pow(unitSystem->prefix(preferedPrefix, unitGroup).value, power);

    if (!unitGroup.isEmpty())
    {
        QList<PhyxUnitSystem::PhyxPrefix> prefixes = unitSystem->prefixes(unitGroup);
        for (int i = prefixes.size()-1; i >= 0; i--)
        {
            if (prefixes.at(i).inputOnly)
                continue;

            prefixes[i].value = pow(prefixes[i].value, power);
            prefixes[i].value /= preferedPrefixValue;
            PhyxFloatDataType tmpValue = value / prefixes.at(i).value;

            if (fabs(tmpValue) >= PHYX_FLOAT_ONE)
                return prefixes.at(i);
        }
    }

    PhyxUnitSystem::PhyxPrefix prefix;
    prefix.value = PHYX_FLOAT_ONE;
    prefix.symbol = "";
    return prefix;
}

PhyxCalculator::ResultVariable PhyxCalculator::formatVariable(PhyxVariable *variable,
                                                              OutputMode outputMode,
                                                              PrefixMode prefixMode,
                                                              int precision,
                                                              char numberFormat,
                                                              QString imaginaryUnit,
                                                              bool useFractions) const
{
    ResultVariable result;
    PhyxValueDataType value = variable->value();
    PhyxCompoundUnit *unit = variable->unit();

    switch (outputMode)
    {
    case OnlyBaseUnitsOutputMode:
        result.unit = unit->dimensionString();
        break;

    case MinimizeUnitOutputMode:
        result.unit = unit->symbol();
        if (prefixMode == UsePrefix)
        {
            if (unit->isSimpleUnit())
            {
                if (value.imag() == PHYX_FLOAT_NULL)
                {
                    PhyxUnitSystem::PhyxPrefix realPrefix = getBestPrefix(value.real(), unit->compounds().first().power, unit->unitGroup(), unit->preferedPrefix());     //get best prefix
                    value = PhyxValueDataType(value.real() / realPrefix.value, PHYX_FLOAT_NULL);

                    if (!unit->preferedPrefix().isEmpty())      // preferd prefix handling
                        result.unit.remove(0, unit->preferedPrefix().size());

                    result.unit.prepend(realPrefix.symbol);
                } else if (value.real() == PHYX_FLOAT_NULL)
                {
                    PhyxUnitSystem::PhyxPrefix imagPrefix = getBestPrefix(value.imag(), unit->compounds().first().power, unit->unitGroup(), unit->preferedPrefix());     //get best prefix
                    value = PhyxValueDataType(PHYX_FLOAT_NULL, value.imag() / imagPrefix.value);

                    if (!unit->preferedPrefix().isEmpty())      // preferd prefix handling
                        result.unit.remove(0, unit->preferedPrefix().size());

                    result.unit.prepend(imagPrefix.symbol);
                }
            }
        }
        else if (prefixMode == UseNoPrefix)
        {

        }
        break;

    case ForceInputUnitsOutputMode:
        if (prefixMode == UsePrefix)
        {

        }
        else if (prefixMode == UseNoPrefix)
        {

        }
        break;
    }

    result.value = complexToString(value, precision, numberFormat, imaginaryUnit, !unit->isOne(), useFractions);

    return result;
}

const PhyxCalculator::ExpressionCacheItem PhyxCalculator::earleyTreeToCacheItem(const QList<EarleyTreeItem> earleyTree, QString const expression)
{
    ExpressionCacheItem cacheItem;
    for (int i = (earleyTree.size()-1); i >= 0; i--)
    {
        for (int ii = 0; ii < earleyTree.at(i).rule->functions.size(); ii++)
        {
            cacheItem.appendItem(earleyTree.at(i).rule->functions.at(ii), earleyTree.at(i).startPos, earleyTree.at(i).endPos);
        }
    }
    cacheItem.expression = expression;

    return cacheItem;
}

bool PhyxCalculator::popVariables(int count)
{
    if (variableStack.size() < count)
    {
        raiseException(ProgramError);
        return false;
    }
    for (int i = count-1; i >= 0; i--)
    {
        variableList[i] = variableStack.pop();
    }
    return true;
}

void PhyxCalculator::pushVariables(int count, int deleteCount)
{
    for (int i = 0; i < count; i++)
    {
        variableStack.push(variableList[i]);
    }
    for (int i = count; i < (count + deleteCount); i++)
    {
        if (variableList[i] != NULL)
        {
            variableList[i]->deleteLater();
            variableList[i] = NULL;
        }
    }
}

void PhyxCalculator::valueCheckComplex()
{
    if (!popVariables(1))
        return;

    if (variableList[0]->isComplex())
        raiseException(ValueComplexError);

    pushVariables(1,0);
}

void PhyxCalculator::valueCheckComplex2()
{
    if (!popVariables(2))
        return;

    if (variableList[0]->isComplex() || variableList[1]->isComplex())
        raiseException(ValueComplexError);

    pushVariables(2,0);
}

void PhyxCalculator::valueCheckComplex3()
{
    if (!popVariables(3))
        return;

    if (variableList[0]->isComplex() || variableList[1]->isComplex() || variableList[2]->isComplex())
        raiseException(ValueComplexError);

    pushVariables(3,0);
}

void PhyxCalculator::valueCheckComplex2th()
{
    if (!popVariables(2))
        return;

    if (variableList[1]->isComplex())
        raiseException(ValueComplexError);

    pushVariables(2,0);
}

void PhyxCalculator::valueCheckComplex3th()
{
    if (!popVariables(3))
        return;

    if (variableList[0]->isComplex())
        raiseException(ValueComplexError);

    pushVariables(3,0);
}

void PhyxCalculator::valueCheckPositive()
{
    if (!popVariables(1))
        return;

    if (!variableList[0]->isPositive())
        raiseException(ValueNotPositiveError);

    pushVariables(1,0);
}

void PhyxCalculator::valueCheckInteger()
{
    if (!popVariables(1))
        return;

    if (!variableList[0]->isInteger())
        raiseException(ValueNotIntegerError);

    pushVariables(1,0);
}

void PhyxCalculator::valueCheckInteger2()
{
    if (!popVariables(2))
        return;

    if (!variableList[0]->isInteger())
        raiseException(ValueNotIntegerError);
    else if (!variableList[1]->isInteger())
        raiseException(ValueNotIntegerError);

    pushVariables(2,0);
}

void PhyxCalculator::valueCheckInteger2th()
{
    if (!popVariables(2))
        return;

    if (!variableList[0]->isInteger())
        raiseException(ValueNotIntegerError);

    pushVariables(2,0);
}

void PhyxCalculator::valueCheckInteger3th()
{
    if (!popVariables(3))
        return;

    if (!variableList[0]->isInteger())
        raiseException(ValueNotIntegerError);

    pushVariables(3,0);
}

void PhyxCalculator::valuePush1()
{
    valueBuffer = PhyxValueDataType(PHYX_FLOAT_ONE,PHYX_FLOAT_NULL);
    pushVariable();
}

void PhyxCalculator::valuePush2()
{
    valueBuffer = PhyxValueDataType(PHYX_FLOAT_TWO,PHYX_FLOAT_NULL);
    pushVariable();
}

void PhyxCalculator::valuePush3()
{
    valueBuffer = PhyxValueDataType(PHYX_FLOAT_THREE,PHYX_FLOAT_NULL);
    pushVariable();
}

void PhyxCalculator::valueCopy()
{
    if (!popVariables(1))
        return;

    variableList[1] = new PhyxVariable(this);
    PhyxVariable::copyVariable(variableList[0], variableList[1]);

    pushVariables(2,0);
}

void PhyxCalculator::valueAdd()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(variableList[0]->value() + variableList[1]->value());

    pushVariables(1,1);
}

void PhyxCalculator::valueSub()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(variableList[0]->value() - variableList[1]->value());

    pushVariables(1,1);
}

void PhyxCalculator::valueMul()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(variableList[0]->value() * variableList[1]->value());

    pushVariables(1,1);
}

void PhyxCalculator::valueDiv()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(variableList[0]->value() / variableList[1]->value());

    pushVariables(1,1);
}

void PhyxCalculator::valueMod()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(PhyxValueDataType(static_cast<PhyxFloatDataType>(variableList[0]->toInt() % variableList[1]->toInt()), PHYX_FLOAT_NULL));

    pushVariables(1,1);
}

void PhyxCalculator::valueNeg()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(-variableList[0]->value());

    pushVariables(1,0);
}

void PhyxCalculator::valueNoPow()
{
    if (!popVariables(2))
        return;

    if (variableList[0]->unit()->isOne())
    {
        pushVariables(2,0);
        valuePow();
    }
    else
    {
        pushVariables(1,1);
    }
}

void PhyxCalculator::valuePow()
{
    if (!popVariables(2))
        return;

    if (variableList[1]->value().imag() == PHYX_FLOAT_NULL)     // precision fix
        variableList[0]->setValue(pow(variableList[0]->value(),variableList[1]->value().real()));
    else
        variableList[0]->setValue(pow(variableList[0]->value(),variableList[1]->value()));

    pushVariables(1,1);
}

void PhyxCalculator::valueSin()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(sin(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueArcsin()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(boost::math::asin(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueCos()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(cos(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueArccos()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(boost::math::acos(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueTan()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(tan(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueArctan()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(boost::math::atan(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueCot()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_ONE, PHYX_FLOAT_NULL)/tan(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueArccot()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_ONE, PHYX_FLOAT_NULL)/boost::math::atan(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueSec()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_ONE, PHYX_FLOAT_NULL)/cos(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueArcsec()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_ONE, PHYX_FLOAT_NULL)/boost::math::acos(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueCsc()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_ONE, PHYX_FLOAT_NULL)/sin(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueArccsc()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_ONE, PHYX_FLOAT_NULL)/boost::math::asin(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueSinh()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(sinh(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueArcsinh()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(boost::math::asinh(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueCosh()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(cosh(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueArccosh()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(boost::math::acosh(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueTanh()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(tanh(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueArctanh()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(boost::math::atanh(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueCoth()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_ONE, PHYX_FLOAT_NULL)/tanh(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueArccoth()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_ONE, PHYX_FLOAT_NULL)/boost::math::atanh(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueSech()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_ONE, PHYX_FLOAT_NULL)/cosh(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueArcsech()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_ONE, PHYX_FLOAT_NULL)/boost::math::acosh(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueCsch()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_ONE, PHYX_FLOAT_NULL)/sinh(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueArccsch()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_ONE, PHYX_FLOAT_NULL)/boost::math::asinh(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueExp()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(exp(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueLn()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(log(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueLog10()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(log10(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueLog2()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(log(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueLogn()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(log(variableList[1]->value()) / log(variableList[0]->value()));

    pushVariables(1,1);
}

void PhyxCalculator::valueRoot()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(exp(log(variableList[1]->value()) / variableList[0]->value()));

    pushVariables(1,1);
}

void PhyxCalculator::valueSqrt()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(sqrt(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueAbs()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(abs(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::valueMax()
{
    if (!popVariables(2))
        return;

    if (variableList[0]->value().real() >= variableList[1]->value().real())
    {
        variableStack.push(variableList[0]);
        variableList[1]->deleteLater();
    }
    else
    {
        variableStack.push(variableList[1]);
        variableList[0]->deleteLater();
    }
}

void PhyxCalculator::valueMin()
{
    if (!popVariables(2))
        return;

    if (variableList[0]->value().real()<= variableList[1]->value().real())
    {
        variableStack.push(variableList[0]);
        variableList[1]->deleteLater();
    }
    else
    {
        variableStack.push(variableList[1]);
        variableList[0]->deleteLater();
    }
}

void PhyxCalculator::valueAvg()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue((variableList[0]->value() + variableList[1]->value())
                        / PhyxValueDataType(PHYX_FLOAT_TWO,PHYX_FLOAT_NULL));

    pushVariables(1,1);
}

void PhyxCalculator::valuePi()
{
#ifndef Q_WS_S60
    valueBuffer = PhyxValueDataType(boost::math::constants::pi<PhyxFloatDataType>(), PHYX_FLOAT_NULL);
#else
    valueBuffer = PhyxValueDataType(static_cast<PhyxFloatDataType>(M_PI), PHYX_FLOAT_NULL);
#endif
    unitBuffer = "";
    pushVariable();
}

void PhyxCalculator::valueE()
{
#ifndef Q_WS_S60
    valueBuffer = PhyxValueDataType(boost::math::constants::e<PhyxFloatDataType>(), PHYX_FLOAT_NULL);
#else
    valueBuffer = PhyxValueDataType(static_cast<PhyxFloatDataType>(M_E), PHYX_FLOAT_NULL);
#endif
    unitBuffer = "";
    pushVariable();
}

void PhyxCalculator::valueInt()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(static_cast<PhyxFloatDataType>(variableList[0]->toInt()),PHYX_FLOAT_NULL));

    pushVariables(1,0);
}

void PhyxCalculator::valueTrunc()
{
    if (!popVariables(1))
        return;

#ifndef Q_WS_S60
    PhyxValueDataType value(boost::math::trunc<PhyxFloatDataType>(variableList[0]->value().real()),PHYX_FLOAT_NULL);
#else
    PhyxValueDataType value(static_cast<PhyxFloatDataType>(variableList[0]->toInt()),PHYX_FLOAT_NULL);
#endif
    variableList[0]->setValue(value);

    pushVariables(1,0);
}

void PhyxCalculator::valueFloor()
{
    if (!popVariables(1))
        return;


    variableList[0]->setValue(PhyxValueDataType(floor(variableList[0]->value().real()),PHYX_FLOAT_NULL));

    pushVariables(1,0);
}

void PhyxCalculator::valueRound()
{
    if (!popVariables(1))
        return;

#ifndef Q_WS_S60
    PhyxValueDataType value(boost::math::round<PhyxFloatDataType>(variableList[0]->value().real()),PHYX_FLOAT_NULL);
#else
    PhyxValueDataType value(floor(variableList[0]->value().real()+(PHYX_FLOAT_ONE/PHYX_FLOAT_TWO)),PHYX_FLOAT_NULL);
#endif
    variableList[0]->setValue(value);

    pushVariables(1,0);
}

void PhyxCalculator::valueCeil()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(ceil(variableList[0]->value().real()),PHYX_FLOAT_NULL));

    pushVariables(1,0);
}

void PhyxCalculator::valueSign()
{
    if (!popVariables(1))
        return;

    if (variableList[0]->value().real() > PHYX_FLOAT_NULL)
    {
        variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_ONE,PHYX_FLOAT_NULL));
    }
    else if (variableList[0]->value().real() == PHYX_FLOAT_NULL)
    {
        variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_NULL,PHYX_FLOAT_NULL));
    }
    else
    {
        variableList[0]->setValue(PhyxValueDataType(-PHYX_FLOAT_ONE,PHYX_FLOAT_NULL));
    }

    pushVariables(1,0);
}

void PhyxCalculator::valueHeaviside()
{
    if (!popVariables(1))
        return;

    if (variableList[0]->value().real() >= PHYX_FLOAT_NULL)
        variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_ONE,PHYX_FLOAT_NULL));
    else
        variableList[0]->setValue(PhyxValueDataType(PHYX_FLOAT_NULL,PHYX_FLOAT_NULL));

    pushVariables(1,0);
}

void PhyxCalculator::valueRand()
{
    valueBuffer = PhyxValueDataType(static_cast<PhyxFloatDataType>(qrand() / static_cast<PhyxFloatDataType>(RAND_MAX)),PHYX_FLOAT_NULL);
    unitBuffer = "";
    pushVariable();
}

void PhyxCalculator::valueRandint()
{
    if (!popVariables(1))
        return;

    PhyxIntegerDataType max = variableList[0]->toInt();
    valueBuffer = static_cast<PhyxIntegerDataType>(static_cast<PhyxFloatDataType>(max) * static_cast<PhyxFloatDataType>(qrand())/static_cast<PhyxFloatDataType>(RAND_MAX));
    unitBuffer = "";
    pushVariable();

    pushVariables(0,1);
}

void PhyxCalculator::valueRandg()
{
    if (!popVariables(2))
        return;

    PhyxFloatDataType mean, standard;
    mean = variableList[0]->value().real();
    standard = variableList[1]->value().real();

    boost::variate_generator<boost::mt19937, boost::normal_distribution<> >
        generator(boost::mt19937(QDateTime::currentMSecsSinceEpoch()),
                  boost::normal_distribution<>(mean,standard));
    variableList[0]->setValue(PhyxValueDataType(static_cast<PhyxFloatDataType>(generator()),PHYX_FLOAT_NULL));

    pushVariables(1,1);
}

void PhyxCalculator::valueFaculty()
{
    if (!popVariables(1))
        return;

    PhyxFloatDataType value = PHYX_FLOAT_ONE;
    int n = static_cast<int>(variableList[0]->toInt());

    for (int i = 2L; i <=n; i++)
        value *= static_cast<PhyxFloatDataType>(i);

    variableList[0]->setValue(PhyxValueDataType(value,PHYX_FLOAT_NULL));

    pushVariables(1,0);
}

void PhyxCalculator::valueBcd()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(static_cast<PhyxFloatDataType>(bcdToLongInt(variableList[0]->toInt())),PHYX_FLOAT_NULL));

    pushVariables(1,0);
}

void PhyxCalculator::valueToBcd()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(static_cast<PhyxFloatDataType>(longIntToBcd(variableList[0]->toInt())),PHYX_FLOAT_NULL));

    pushVariables(1,0);
}

void PhyxCalculator::valueAns()
{
    parameterBuffer = "#";
    variableLoad();
}

void PhyxCalculator::valueGcd()
{
    if (!popVariables(2))
        return;

    PhyxIntegerDataType x = abs(variableList[0]->toInt());
    PhyxIntegerDataType y = abs(variableList[1]->toInt());

    variableList[0]->setValue(PhyxValueDataType(static_cast<PhyxFloatDataType>(gcd(x,y)), PHYX_FLOAT_NULL));

    pushVariables(1,1);
}

void PhyxCalculator::valueLcm()
{
    if (!popVariables(2))
        return;

    PhyxIntegerDataType x = abs(variableList[0]->toInt());
    PhyxIntegerDataType y = abs(variableList[1]->toInt());

    variableList[0]->setValue(PhyxValueDataType(static_cast<PhyxFloatDataType>(lcm(x,y)), PHYX_FLOAT_NULL));

    pushVariables(1,1);
}

void PhyxCalculator::complexReal()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(real(variableList[0]->value()),PHYX_FLOAT_NULL));

    pushVariables(1,0);
}

void PhyxCalculator::complexImag()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(imag(variableList[0]->value()),PHYX_FLOAT_NULL));

    pushVariables(1,0);
}

void PhyxCalculator::complexArg()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(arg(variableList[0]->value()),PHYX_FLOAT_NULL));

    pushVariables(1,0);
}

void PhyxCalculator::complexNorm()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(PhyxValueDataType(norm(variableList[0]->value()),PHYX_FLOAT_NULL));

    pushVariables(1,0);
}

void PhyxCalculator::complexConj()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(conj(variableList[0]->value()));

    pushVariables(1,0);
}

void PhyxCalculator::complexPolar()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(std::polar(variableList[0]->value().real(), variableList[1]->value().real()));

    pushVariables(1,1);
}

void PhyxCalculator::logicAnd()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(variableList[0]->value().real() && variableList[1]->value().real());

    pushVariables(1,1);
}

void PhyxCalculator::logicOr()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(variableList[0]->value().real() || variableList[1]->value().real());

    pushVariables(1,1);
}

void PhyxCalculator::logicNand()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(!(variableList[0]->value().real() && variableList[1]->value().real()));

    pushVariables(1,1);
}

void PhyxCalculator::logicNor()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(!(variableList[0]->value().real() || variableList[1]->value().real()));

    pushVariables(1,1);
}

void PhyxCalculator::logicXand()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue((variableList[0]->value().real() && variableList[1]->value().real())
                        || (!variableList[0]->value().real() && !variableList[1]->value().real()));

    pushVariables(1,1);
}

void PhyxCalculator::logicXor()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue((variableList[0]->value().real() && !variableList[1]->value().real())
                        || (!variableList[0]->value().real() && variableList[1]->value().real()));

    pushVariables(1,1);
}

void PhyxCalculator::logicNot()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(!variableList[0]->value().real());

    pushVariables(1,0);
}

void PhyxCalculator::logicEqual()
{
    if (!popVariables(2))
        return;

    variableList[2] = new PhyxVariable(this);
    variableList[2]->setValue((variableList[0]->value() == variableList[1]->value()) && variableList[0]->unit()->isSame(variableList[1]->unit()));
    variableStack.push(variableList[2]);

    pushVariables(0,2);
}

void PhyxCalculator::logicNotEqual()
{
    if (!popVariables(2))
        return;

    variableList[2] = new PhyxVariable(this);
    variableList[2]->setValue((variableList[0]->value() != variableList[1]->value()) || !variableList[0]->unit()->isSame(variableList[1]->unit()));
    variableStack.push(variableList[2]);

    pushVariables(0,2);
}

void PhyxCalculator::logicGreater()
{
    if (!popVariables(2))
        return;

    variableList[2] = new PhyxVariable(this);
    variableList[2]->setValue(variableList[0]->value().real() > variableList[1]->value().real());
    variableStack.push(variableList[2]);

    pushVariables(0,2);
}

void PhyxCalculator::logicGreaterOrEqual()
{
    if (!popVariables(2))
        return;

    variableList[2] = new PhyxVariable(this);
    variableList[2]->setValue(variableList[0]->value().real() >= variableList[1]->value().real());
    variableStack.push(variableList[2]);

    pushVariables(0,2);
}

void PhyxCalculator::logicSmaller()
{
    if (!popVariables(2))
        return;

    variableList[2] = new PhyxVariable(this);
    variableList[2]->setValue(variableList[0]->value().real() < variableList[1]->value().real());
    variableStack.push(variableList[2]);

    pushVariables(0,2);
}

void PhyxCalculator::logicSmallerOrEqual()
{
    if (!popVariables(2))
        return;

    variableList[2] = new PhyxVariable(this);
    variableList[2]->setValue(variableList[0]->value().real() <= variableList[1]->value().real());
    variableStack.push(variableList[2]);

    pushVariables(0,2);
}

void PhyxCalculator::bitAnd()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(variableList[0]->toInt() & variableList[1]->toInt());

    pushVariables(1,1);
}

void PhyxCalculator::bitOr()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(variableList[0]->toInt() | variableList[1]->toInt());

    pushVariables(1,1);
}

void PhyxCalculator::bitXor()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(variableList[0]->toInt() ^ variableList[1]->toInt());

    pushVariables(1,1);
}

void PhyxCalculator::bitInv()
{
    if (!popVariables(1))
        return;

    variableList[0]->setValue(~variableList[0]->toInt());

    pushVariables(1,0);
}

void PhyxCalculator::bitShiftLeft()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(variableList[0]->toInt() << variableList[1]->toInt());

    pushVariables(1,1);
}

void PhyxCalculator::bitShiftRight()
{
    if (!popVariables(2))
        return;

    variableList[0]->setValue(variableList[0]->toInt() >> variableList[1]->toInt());

    pushVariables(1,1);
}

void PhyxCalculator::conditionIfElse()
{
    if (!popVariables(3))
        return;

    if (variableList[0]->toInt())
    {
        variableStack.push(variableList[1]);
        variableList[2]->deleteLater();
    }
    else
    {
        variableStack.push(variableList[2]);
        variableList[1]->deleteLater();
    }

    variableList[0]->deleteLater();
}

void PhyxCalculator::unitCheckDimensionless()
{
    if (!popVariables(1))
        return;

    if (!variableList[0]->unit()->isDimensionlessUnit())
        raiseException(UnitNotDimensionlessError);
    else
        variableList[0]->unit()->simplify();

    pushVariables(1,0);
}

void PhyxCalculator::unitCheckDimensionless2()
{
    if (!popVariables(2))
        return;

    if (!variableList[0]->unit()->isDimensionlessUnit() || !variableList[1]->unit()->isDimensionlessUnit())
        raiseException(UnitNotDimensionlessError);
    else
    {
        variableList[0]->unit()->simplify();
        variableList[1]->unit()->simplify();
    }

    pushVariables(2,0);
}

void PhyxCalculator::unitCheckDimensionless2th()
{
    if (!popVariables(2))
        return;

    if (!variableList[0]->unit()->isDimensionlessUnit())
        raiseException(UnitNotDimensionlessError);
    else
        variableList[0]->unit()->simplify();

    pushVariables(2,0);
}

void PhyxCalculator::unitCheckDimensionless3th()
{
    if (!popVariables(3))
        return;

    if (!variableList[0]->unit()->isDimensionlessUnit())
        raiseException(UnitNotDimensionlessError);
    else
        variableList[0]->unit()->simplify();

    pushVariables(3,0);
}

void PhyxCalculator::unitCheckConvertible()
{
    if (!popVariables(2))
        return;

    if (!variableList[0]->unit()->isConvertible(variableList[1]->unit()))
        raiseException(UnitsNotConvertibleError);
    else
        variableList[0]->unit()->compoundsEqualize(variableList[1]->unit());

    pushVariables(2,0);
}

void PhyxCalculator::unitConvert()
{
    if (!popVariables(2))
        return;

    if (parameterBuffer.at(0).isNumber())   //add a multiplication sign if output seems to be something other than a unit
    {
        parameterBuffer.prepend("*");
    }

    variableList[0]->convertUnit(variableList[1]->unit());
    variableList[0]->setValue(variableList[0]->value() / variableList[1]->value());

    pushVariables(1,1);
}

void PhyxCalculator::unitMul()
{
    if (!popVariables(2))
        return;

    variableList[0]->unit()->multiply(variableList[1]->unit());

    pushVariables(2,0);
}

void PhyxCalculator::unitDiv()
{
    if (!popVariables(2))
        return;

    variableList[0]->unit()->divide(variableList[1]->unit());

    pushVariables(2,0);
}

void PhyxCalculator::unitPow()
{
    if (!popVariables(2))
        return;

    variableList[0]->unit()->raise(variableList[1]->value().real());

    pushVariables(2,0);
}

void PhyxCalculator::unitRoot()
{
    if (!popVariables(2))
        return;

    variableList[0]->unit()->root(variableList[1]->value().real());

    pushVariables(2,0);
}

void PhyxCalculator::unitSqrt()
{
    if (!popVariables(1))
        return;

    variableList[0]->unit()->root(PHYX_FLOAT_TWO);

    pushVariables(1,0);
}

void PhyxCalculator::unitClear()
{
    if (!popVariables(1))
        return;

    variableList[0]->setUnit(new PhyxCompoundUnit(this));

    pushVariables(1,0);
}

void PhyxCalculator::unitAdd()
{
    if (variableStack.isEmpty())        // <- base unit
    {
        unitSystem->addBaseUnit(stringBuffer, 0, unitGroupBuffer, prefixBuffer);
    }
    else
    {
        if (variableStack.size() == 2)
        {
            if (!popVariables(2))
                return;
        }
        else if (variableStack.size() == 1)
        {
            if (!popVariables(1))
                return;
        }
        else
        {
            raiseException(ProgramError);
            return;
        }

        variableList[0]->unit()->simplify();

        PhyxUnit *unit = new PhyxUnit();
        unit->setPowers(variableList[0]->unit()->powers());
        unit->setUnitGroup(unitGroupBuffer);
        unit->setPreferedPrefix(prefixBuffer);
        unit->setSymbol(stringBuffer);
        unit->setFlags(0);
        unit->setScaleFactor(variableList[0]->value().real());
        if (variableList[1] != NULL)
            unit->setOffset(variableList[1]->value().real());

        unitSystem->addDerivedUnit(unit);

        if (variableList[1] != NULL)
            pushVariables(0,2);
        else
            pushVariables(0,1);
    }

    stringBuffer.clear();
    unitGroupBuffer.clear();
    prefixBuffer.clear();
    clearFlags();
}

void PhyxCalculator::unitRemove()
{

}

void PhyxCalculator::variableAdd()
{
    if (!popVariables(1))
        return;

    variableManager->addVariable(stringBuffer, variableList[0]);
    stringBuffer.clear();
}

void PhyxCalculator::variableRemove()
{
    variableManager->removeVariable(parameterBuffer);
}

void PhyxCalculator::variableLoad()
{
    variableStack.push(variableManager->getVariable(parameterBuffer));
    nameBuffer = parameterBuffer;
}

void PhyxCalculator::variablePreInc()
{
    valuePush1();
    valueAdd();
    valueCopy();

    stringBuffer = nameBuffer;
    variableAdd();
}

void PhyxCalculator::variablePreDec()
{
    valuePush1();
    valueSub();
    valueCopy();

    stringBuffer = nameBuffer;
    variableAdd();
}

void PhyxCalculator::variablePostInc()
{
    valueCopy();
    valuePush1();
    valueAdd();

    stringBuffer = nameBuffer;
    variableAdd();
}

void PhyxCalculator::variablePostDec()
{
    valueCopy();
    valuePush1();
    valueSub();

    stringBuffer = nameBuffer;
    variableAdd();
}

void PhyxCalculator::constantAdd()
{
    if (!popVariables(1))
        return;

    variableManager->addConstant(stringBuffer, variableList[0]);
    stringBuffer.clear();
}

void PhyxCalculator::constantRemove()
{
    variableManager->removeConstant(parameterBuffer);
}

void PhyxCalculator::constantLoad()
{
    variableStack.push(variableManager->getConstant(parameterBuffer));
    nameBuffer = parameterBuffer;
}

void PhyxCalculator::listAddStart()
{
    listModeActive = true;
    listModeType = ListAddType;
}

void PhyxCalculator::listSubStart()
{
    listModeActive = true;
    listModeType = ListSubType;
}

void PhyxCalculator::listMulStart()
{
    listModeActive = true;
    listModeType = ListMulType;
}

void PhyxCalculator::listDivStart()
{
    listModeActive = true;
    listModeType = ListDivType;
}

void PhyxCalculator::listModStart()
{
    listModeActive = true;
    listModeType = ListModType;
}

void PhyxCalculator::listPowStart()
{
    listModeActive = true;
    listModeType = ListPowType;
}

void PhyxCalculator::listBOrStart()
{
    listModeActive = true;
    listModeType = ListBOrType;
}

void PhyxCalculator::listBAndStart()
{
    listModeActive = true;
    listModeType = ListBAndType;
}

void PhyxCalculator::listBXorStart()
{
    listModeActive = true;
    listModeType = ListBXorType;
}

void PhyxCalculator::listXandStart()
{
    listModeActive = true;
    listModeType = ListXandType;
}

void PhyxCalculator::listAndStart()
{
    listModeActive = true;
    listModeType = ListAndType;
}

void PhyxCalculator::listNandStart()
{
    listModeActive = true;
    listModeType = ListNandType;
}

void PhyxCalculator::listXorStart()
{
    listModeActive = true;
    listModeType = ListXorType;
}

void PhyxCalculator::listOrStart()
{
    listModeActive = true;
    listModeType = ListOrType;
}

void PhyxCalculator::listNorStart()
{
    listModeActive = true;
    listModeType = ListNorType;
}

void PhyxCalculator::listValueSave()
{
    stringBuffer = tr("current result");
    variableAdd();
}

void PhyxCalculator::listValueLoad()
{
    parameterBuffer = tr("current result");
    variableLoad();
}

void PhyxCalculator::listValueSwap()
{
    if (!popVariables(2))
        return;

    variableList[2]=variableList[0];
    variableList[0]=variableList[1];
    variableList[1]=variableList[2];

    pushVariables(2,0);
}

void PhyxCalculator::listEnd()
{
    listModeActive = false;
    variableManager->removeVariable(tr("current result"));
}

bool PhyxCalculator::executeFunction(QString expression, QStringList parameters, bool verifyOnly = false)
{
    QStringList newVariables;   //lists where all temporary variables are stores
    QList<PhyxVariable*> oldVariables;
    bool success;

    //prevent gui from updating while function is running
    noGuiUpdate = true;

    //push function parameters to stack
    for (int i = 0; i < parameters.size(); i++)
    {
        QString parameterName = parameters.at(i);

        //if variable with same name is defined, temporarily change value
        if (variableManager->containsVariable(parameterName))
        {
            newVariables.append(parameterName);
            oldVariables.append(variableManager->getVariable(parameterName));
        }
        variableManager->addVariable(parameterName, variableStack.pop());
    }

    noGuiUpdate = false;

    //execute function
    QList<int> whiteSpaceList;
    m_expression = removeWhitespace(expression, &whiteSpaceList);

    if (expressionCacheMap.contains(m_expression))
    {
        if (!verifyOnly)
            success = this->evaluate(expressionCacheMap.value(m_expression), whiteSpaceList);
        else
            success = true;
    }
    else
    {
        success = /*this->setExpression(expression);*/ earleyParser->parseWord(expression);
        if (success)
        {
            if (!verifyOnly)
            {
                QList<EarleyTreeItem> earleyTree = earleyParser->getTree();                     //get a earley tree for the function
                expressionCacheMap.insert(m_expression, earleyTreeToCacheItem(earleyTree, m_expression));
                success = this->evaluate(earleyTree, m_expression, whiteSpaceList);
            }
        }
    }

    if (!success)
    {
        if (m_error)
            noGuiUpdate = true;

        raiseException(SyntaxError);
    }

    noGuiUpdate = true;

    //clear temporary variables
    for (int i = 0; i < parameters.size(); i++)
    {
        if (!newVariables.contains(parameters.at(i)))
            variableManager->removeVariable(parameters.at(i));
    }

    //rename temporary renamed variables back
    for (int i = 0; i < newVariables.count(); i++)
        variableManager->addVariable(newVariables.at(i), oldVariables[i]);

    //gui should update again
    noGuiUpdate = false;

    return success;
}

void PhyxCalculator::functionAdd()
{
    int clipIndex = parameterBuffer.indexOf("(");
    //int equalIndex = parameterBuffer.indexOf("=");
    QString functionName = parameterBuffer.left(clipIndex);
    QString functionExpression = expressionBuffer;//parameterBuffer.mid(equalIndex+1);
    QStringList functionParameters;
    while (!functionParameterStack.isEmpty())
    {
        functionParameters.append(functionParameterStack.pop());
        valueBuffer = PhyxValueDataType(PHYX_FLOAT_ONE, PHYX_FLOAT_NULL);
        pushVariable();     //for each parameter push 1 to stack for verifying the syntax of the expression
    }

    if (executeFunction(functionExpression, functionParameters, true))
        variableManager->addFunction(functionName, functionExpression, functionParameters);
}

void PhyxCalculator::functionRemove()
{
    int clipIndex = parameterBuffer.indexOf("(");
    QString functionName = parameterBuffer.left(clipIndex);
    variableManager->removeFunction(functionName);
}

void PhyxCalculator::functionRun()
{
    //int clipIndex = parameterBuffer.indexOf("(");
    QString functionName;//= parameterBuffer.left(clipIndex);
    QStringList functionList = functions();
    functionList.sort();

    for (int i = functionList.size()-1; i >= 0; i--)
    {
        if (parameterBuffer.indexOf(functionList.at(i)) == 0)
        {
            functionName = functionList.at(i);
            break;
        }
    }

    PhyxVariableManager::PhyxFunction *function = variableManager->getFunction(functionName);
    executeFunction(function->expression, function->parameters);
}

void PhyxCalculator::bufferUnit()
{
    //get the unit
    unitBuffer = parameterBuffer;
}

void PhyxCalculator::bufferValue()
{
    valueBuffer = stringToComplex(parameterBuffer);
}

void PhyxCalculator::bufferHex()
{
    valueBuffer = PhyxValueDataType(static_cast<PhyxFloatDataType>(hexToLongInt(parameterBuffer)),PHYX_FLOAT_NULL);
}

void PhyxCalculator::bufferOct()
{
    valueBuffer = PhyxValueDataType(static_cast<PhyxFloatDataType>(octToLongInt(parameterBuffer)),PHYX_FLOAT_NULL);
}

void PhyxCalculator::bufferBin()
{
    valueBuffer = PhyxValueDataType(static_cast<PhyxFloatDataType>(binToLongInt(parameterBuffer)),PHYX_FLOAT_NULL);
}

void PhyxCalculator::bufferHexString()
{
    if (!popVariables(1))
        return;

    stringBuffer = longIntToHex(static_cast<PhyxIntegerDataType>(variableList[0]->value().real()));

    pushVariables(0,1);
}

void PhyxCalculator::bufferOctString()
{
    if (!popVariables(1))
        return;

    stringBuffer = longIntToOct(static_cast<PhyxIntegerDataType>(variableList[0]->value().real()));

    pushVariables(0,1);
}

void PhyxCalculator::bufferBinString()
{
    if (!popVariables(1))
        return;

    stringBuffer = longIntToBin(static_cast<PhyxIntegerDataType>(variableList[0]->value().real()));

    pushVariables(0,1);
}

void PhyxCalculator::bufferPrefix()
{
    prefixBuffer = parameterBuffer;
}

void PhyxCalculator::bufferString()
{
    stringBuffer = parameterBuffer;
    stringBuffer.remove('"');

    if (stringBuffer.at(0).isNumber())
        raiseException(StringNotValidError);
}

void PhyxCalculator::bufferExpression()
{
    expressionBuffer = parameterBuffer;
}

void PhyxCalculator::bufferUnitGroup()
{
    unitGroupBuffer = parameterBuffer;
}

void PhyxCalculator::pushVariable()
{
    //create new variable
    PhyxVariable *variable = new PhyxVariable(this);
    variable->unit()->setUnitSystem(unitSystem);
    if (!unitBuffer.isEmpty())
        variable->setUnit(unitSystem->unit(unitBuffer));

    //handle units with prefered prefix (like kg)
    QString preferedPrefix;
    QString unitGroup;
    PhyxFloatDataType preferedPrefixValue = PHYX_FLOAT_ONE;
    if (!variable->unit()->isDimensionlessUnit())
    {
        preferedPrefix  = variable->unit()->compounds().first().unit->preferedPrefix();
        unitGroup       = variable->unit()->compounds().first().unit->unitGroup();
        if (!preferedPrefix.isEmpty())
            preferedPrefixValue = unitSystem->prefix(preferedPrefix, unitGroup).value;
    }

    if (!prefixBuffer.isEmpty())
    {
        PhyxUnitSystem::PhyxPrefix prefix = unitSystem->prefix(prefixBuffer, variable->unit()->unitGroup());
        if (!prefix.unitGroup.isEmpty())
        {
            PhyxValueDataType value = PhyxValueDataType(prefix.value, PHYX_FLOAT_NULL);

            value /= preferedPrefixValue;
            valueBuffer *= value;

            variable->setValue(valueBuffer);
        }
        else
        {
            raiseException(PrefixError);
        }
    }
    else
        variable->setValue(valueBuffer / PhyxValueDataType(preferedPrefixValue,PHYX_FLOAT_NULL));

    //push it to the stack
    variableStack.push(variable);

    //cleanup
    valueBuffer = PHYX_FLOAT_ONE;
    unitBuffer.clear();
    prefixBuffer.clear();
}

void PhyxCalculator::pushFunctionParameter()
{
    functionParameterStack.push(stringBuffer);
}

void PhyxCalculator::setInputOnlyFlag()
{
    flagBuffer |= InputOnlyFlag;
}

void PhyxCalculator::outputVariable()
{
    if (stackLevel == 1)    //this function has only effect on lowest stack level
    {
        if (!variableStack.isEmpty())
        {
            if (!listModeActive)
            {
                if (!popVariables(1))
                    return;

                //set the special variable #
                variableManager->addVariable("#", variableList[0]);
                //if (m_result != NULL)   // delete old result
                //    delete m_result;
                m_resultValue = variableList[0]->value();
                m_resultUnit = variableList[0]->unit()->symbol();
                m_result = variableList[0];

                if (!noGuiUpdate)
                    emit outputResult();
            }
            else
            {
                listValueLoad();
                listValueSwap();
                if (listModeType == ListAddType)
                {
                    unitCheckConvertible();
                    if (m_error)
                        return;
                    valueAdd();
                }
                else if (listModeType == ListSubType)
                {
                    unitCheckConvertible();
                    if (m_error)
                        return;
                    valueSub();
                }
                else if (listModeType == ListMulType)
                {
                    unitMul();
                    valueMul();
                }
                else if (listModeType == ListDivType)
                {
                    unitDiv();
                    valueDiv();
                }
                else if (listModeType == ListModType)
                {
                    unitCheckDimensionless2();
                    if (m_error)
                        return;
                    valueCheckInteger2();
                    if (m_error)
                        return;
                    valueMod();
                }
                else if (listModeType == ListPowType)
                {
                    unitCheckDimensionless();
                    if (m_error)
                        return;
                    unitPow();
                    valuePow();
                }
                else
                {
                    unitCheckDimensionless2();
                    if (m_error)
                        return;
                    valueCheckInteger2();
                    if (m_error)
                        return;
                    if (listModeType == ListBAndType)
                        bitAnd();
                    else if (listModeType == ListBOrType)
                        bitOr();
                    else if (listModeType == ListBXorType)
                        bitXor();
                    else if (listModeType == ListXandType)
                        logicXand();
                    else if (listModeType == ListAndType)
                        logicAnd();
                    else if (listModeType == ListNandType)
                        logicNand();
                    else if (listModeType == ListXorType)
                        logicXor();
                    else if (listModeType == ListOrType)
                        logicOr();
                    else if (listModeType == ListNorType)
                        logicNor();
                }

                listValueSave();
            }
        }
    }
}

void PhyxCalculator::outputString()
{
    if (!noGuiUpdate)
        emit outputText(stringBuffer);
}

void PhyxCalculator::outputConvertedUnit()
{
    if (!popVariables(1))
        return;

    m_resultValue = variableList[0]->value();
    m_resultUnit = "";
    m_result = variableList[0];

    if (!noGuiUpdate)
        emit outputConverted(parameterBuffer);
    parameterBuffer.clear();
}

void PhyxCalculator::unitGroupAdd()
{
    unitSystem->addUnitGroup(stringBuffer);
    stringBuffer.clear();
}

void PhyxCalculator::unitGroupRemove()
{
    unitSystem->removeUnitGroup(unitGroupBuffer);
    unitGroupBuffer.clear();
}

void PhyxCalculator::prefixAdd()
{
    if (!popVariables(1))
        return;

    unitSystem->addPrefix(stringBuffer, variableList[0]->value().real(), unitGroupBuffer, (flagBuffer & InputOnlyFlag));

    unitGroupBuffer.clear();
    stringBuffer.clear();
    clearFlags();

    pushVariables(0,1);
}

void PhyxCalculator::prefixRemove()
{
    unitSystem->removePrefix(prefixBuffer);
    prefixBuffer.clear();
}

void PhyxCalculator::appendLowLevelOperation(QString variableName, PhyxCalculator::LowLevelOperationType type, PhyxVariable *variable)
{
    LowLevelOperation   *operation = new LowLevelOperation;
    LowLevelOperationList *operationList = new LowLevelOperationList;

    operation->type = type;
    operation->variableName = variableName;
    operation->variable = variable;
    operationList->append(operation);
    lowLevelStack.push(operationList);
}

void PhyxCalculator::runLowLevelOperation(PhyxCalculator::LowLevelOperation *operation)
{
    if (operation->type == AssignmentOperation)
    {
        variableManager->addVariable(operation->variableName, operation->variable);
    }
    else if (operation->type == AssignmentRemoveOperation)
    {
        variableManager->removeVariable(operation->variableName);
    }
    else if (operation->type == OutputOperation)
    {
        //set stacklevel temporary to 0 to make outputVariable work
        int tmpStackLevel = stackLevel;
        stackLevel = 1;

        variableStack.push(operation->variable);
        outputVariable();

        stackLevel = tmpStackLevel;
    }
    else if (operation->type == CombinedAssignmentOperationAdd)
    {
        variableStack.push(variableManager->getVariable(operation->variableName));
        variableStack.push(operation->variable);
        unitCheckConvertible();
        valueAdd();
        variableManager->addVariable(operation->variableName, variableStack.pop());
    }
    else if (operation->type == CombinedAssignmentOperationSub)
    {
        variableStack.push(variableManager->getVariable(operation->variableName));
        variableStack.push(operation->variable);
        unitCheckConvertible();
        valueSub();
        variableManager->addVariable(operation->variableName, variableStack.pop());
    }
    else if (operation->type == CombinedAssignmentOperationMul)
    {
        variableStack.push(variableManager->getVariable(operation->variableName));
        variableStack.push(operation->variable);
        unitMul();
        valueMul();
        variableManager->addVariable(operation->variableName, variableStack.pop());
    }
    else if (operation->type == CombinedAssignmentOperationDiv)
    {
        variableStack.push(variableManager->getVariable(operation->variableName));
        variableStack.push(operation->variable);
        unitDiv();
        valueDiv();
        variableManager->addVariable(operation->variableName, variableStack.pop());
    }
    else if (operation->type == CombinedAssignmentOperationMod)
    {
        variableStack.push(variableManager->getVariable(operation->variableName));
        variableStack.push(operation->variable);
        unitCheckDimensionless2();
        valueCheckInteger2();
        valueMod();
        variableManager->addVariable(operation->variableName, variableStack.pop());
    }
    else if (operation->type == CombinedAssignmentOperationAnd)
    {
        variableStack.push(variableManager->getVariable(operation->variableName));
        variableStack.push(operation->variable);
        unitCheckDimensionless2();
        valueCheckInteger2();
        bitAnd();
        variableManager->addVariable(operation->variableName, variableStack.pop());
    }
    else if (operation->type == CombinedAssignmentOperationOr)
    {
        variableStack.push(variableManager->getVariable(operation->variableName));
        variableStack.push(operation->variable);
        unitCheckDimensionless2();
        valueCheckInteger2();
        bitOr();
        variableManager->addVariable(operation->variableName, variableStack.pop());
    }
    else if (operation->type == CombinedAssignmentOperationXor)
    {
        variableStack.push(variableManager->getVariable(operation->variableName));
        variableStack.push(operation->variable);
        unitCheckDimensionless2();
        valueCheckInteger2();
        bitXor();
        variableManager->addVariable(operation->variableName, variableStack.pop());
    }
    else if (operation->type == CombinedAssignmentOperationShiftLeft)
    {
        variableStack.push(variableManager->getVariable(operation->variableName));
        variableStack.push(operation->variable);
        unitCheckDimensionless2();
        valueCheckInteger2();
        bitShiftLeft();
        variableManager->addVariable(operation->variableName, variableStack.pop());
    }
    else if (operation->type == CombinedAssignmentOperationShiftRight)
    {
        variableStack.push(variableManager->getVariable(operation->variableName));
        variableStack.push(operation->variable);
        unitCheckDimensionless2();
        valueCheckInteger2();
        bitShiftRight();
        variableManager->addVariable(operation->variableName, variableStack.pop());
    }
}

void PhyxCalculator::lowLevelAdd()
{
    LowLevelOperationList *operationList1 = lowLevelStack.pop();
    LowLevelOperationList *operationList2 = lowLevelStack.pop();

    operationList1->append(*operationList2);
    lowLevelStack.push(operationList1);

    delete operationList2;
}

void PhyxCalculator::lowLevelRun()
{
    if (!lowLevelStack.isEmpty())
    {
        LowLevelOperationList *operationList1 = lowLevelStack.pop();

        for (int i = operationList1->size()-1; i  >= 0; i--)
        {
            runLowLevelOperation(operationList1->at(i));
            delete operationList1->at(i);
        }

        delete operationList1;
    }
}

void PhyxCalculator::lowLevelAssignment()
{
    if (!popVariables(1))
        return;

    appendLowLevelOperation(stringBuffer, AssignmentOperation, variableList[0]);
    stringBuffer.clear();
}

void PhyxCalculator::lowLevelAssignmentRemove()
{
    appendLowLevelOperation(nameBuffer, AssignmentRemoveOperation, NULL);
    nameBuffer.clear();
}

void PhyxCalculator::lowLevelCombinedAssignmentAdd()
{
    if (!popVariables(1))
        return;

    appendLowLevelOperation(nameBuffer, CombinedAssignmentOperationAdd, variableList[0]);
    nameBuffer.clear();
}

void PhyxCalculator::lowLevelCombinedAssignmentSub()
{
    if (!popVariables(1))
        return;

    appendLowLevelOperation(nameBuffer, CombinedAssignmentOperationSub, variableList[0]);
    nameBuffer.clear();
}

void PhyxCalculator::lowLevelCombinedAssignmentMul()
{
    if (!popVariables(1))
        return;

    appendLowLevelOperation(nameBuffer, CombinedAssignmentOperationMul, variableList[0]);
    nameBuffer.clear();
}

void PhyxCalculator::lowLevelCombinedAssignmentDiv()
{
    if (!popVariables(1))
        return;

    appendLowLevelOperation(nameBuffer, CombinedAssignmentOperationDiv, variableList[0]);
    nameBuffer.clear();
}

void PhyxCalculator::lowLevelCombinedAssignmentMod()
{
    if (!popVariables(1))
        return;

    appendLowLevelOperation(nameBuffer, CombinedAssignmentOperationMod, variableList[0]);
    nameBuffer.clear();
}

void PhyxCalculator::lowLevelCombinedAssignmentAnd()
{
    if (!popVariables(1))
        return;

    appendLowLevelOperation(nameBuffer, CombinedAssignmentOperationAnd, variableList[0]);
    nameBuffer.clear();
}

void PhyxCalculator::lowLevelCombinedAssignmentOr()
{
    if (!popVariables(1))
        return;

    appendLowLevelOperation(nameBuffer, CombinedAssignmentOperationOr, variableList[0]);
    nameBuffer.clear();
}

void PhyxCalculator::lowLevelCombinedAssignmentXor()
{
    if (!popVariables(1))
        return;

    appendLowLevelOperation(nameBuffer, CombinedAssignmentOperationXor, variableList[0]);
    nameBuffer.clear();
}

void PhyxCalculator::lowLevelCombinedAssignmentShiftLeft()
{
    if (!popVariables(1))
        return;

    appendLowLevelOperation(nameBuffer, CombinedAssignmentOperationShiftLeft, variableList[0]);
    nameBuffer.clear();
}

void PhyxCalculator::lowLevelCombinedAssignmentShiftRight()
{
    if (!popVariables(1))
        return;

    appendLowLevelOperation(nameBuffer, CombinedAssignmentOperationShiftRight, variableList[0]);
    nameBuffer.clear();
}

void PhyxCalculator::lowLevelOutput()
{
    if (stackLevel <= 1)    //deactivate this function for stack levels > 1
    {
        if (!popVariables(1))
            return;

        appendLowLevelOperation(QString(), OutputOperation, variableList[0]);
    }
}

void PhyxCalculator::calculateDataset(QString expression,
                                      QStringList parameters,
                                      PhyxVariable *startVariable,
                                      PhyxFloatDataType stop,
                                      PhyxFloatDataType step,
                                      PhyxVariableManager::DatasetType datasetType)
{
    PhyxVariable *tmpVariable;
    PhyxCompoundUnit *unit;

    PhyxVariableManager::PhyxDataset *dataset;
    QList<PhyxValueDataType> xData;
    QList<PhyxValueDataType> yData;
    PhyxCompoundUnit *xUnit;
    PhyxCompoundUnit *yUnit;

    PhyxFloatDataType start = startVariable->value().real();
    PhyxFloatDataType value = start;

    PhyxFloatDataType logParamStep = PHYX_FLOAT_ONE; //variable needed for logarithmic datasets
    PhyxFloatDataType logParamStartDecade = PHYX_FLOAT_NULL; //variable needed for logarithmic datasets
    int i = 2;

    dataset = new PhyxVariableManager::PhyxDataset; //create dataset
    dataset->name = expression;
    dataset->type = datasetType;
    dataset->plotted = true;
    dataset->plotXAxis = 0;
    dataset->plotYAxis = 0;
    xUnit = new PhyxCompoundUnit();
    yUnit = new PhyxCompoundUnit();
    PhyxCompoundUnit::copyCompoundUnit(startVariable->unit(), xUnit); //set x unit

    //create parameters for logarithmic dataset
    if (datasetType == PhyxVariableManager::LogarithmicDataset)
    {
        PhyxFloatDataType stopDecade;
        PhyxFloatDataType decade;

        logParamStartDecade = log10(start);
        stopDecade = log10(stop);
        decade = stopDecade - logParamStartDecade;

        logParamStep = decade/step;
    }

    //initialize first run
    tmpVariable = new PhyxVariable();
    unit = new PhyxCompoundUnit();
    PhyxCompoundUnit::copyCompoundUnit(startVariable->unit(), unit);
    tmpVariable->setUnit(unit);
    tmpVariable->setValue(PhyxValueDataType(value,PHYX_FLOAT_NULL));
    variableStack.push(tmpVariable);

    //execute first run
    if (executeFunction(expression, parameters, false))
    {
        tmpVariable = variableStack.pop();
        PhyxCompoundUnit::copyCompoundUnit(tmpVariable->unit(), yUnit); //set y unit
        tmpVariable->deleteLater();

        while (value <= stop)
        {
            //initialize
            tmpVariable = new PhyxVariable();
            unit = new PhyxCompoundUnit();
            PhyxCompoundUnit::copyCompoundUnit(startVariable->unit(), unit);
            tmpVariable->setUnit(unit);
            tmpVariable->setValue(PhyxValueDataType(value,PHYX_FLOAT_NULL));
            variableStack.push(tmpVariable);

            //execute
            if (executeFunction(expression, parameters,false))
                tmpVariable = variableStack.pop();
            else
                return;

            //save data
            xData.append(PhyxValueDataType(value, PHYX_FLOAT_NULL));
            yData.append(tmpVariable->value());
            tmpVariable->deleteLater();

            //increase value
            if (datasetType == PhyxVariableManager::LogarithmicDataset)
            {
                value = pow(PHYX_FLOAT_TEN,logParamStartDecade+logParamStep*i);
                i++;
            }
            else
                value += step;
        }

        dataset->unit.append(xUnit);
        dataset->unit.append(yUnit);
        dataset->data.append(xData);
        dataset->data.append(yData);

        variableManager->addDataset(dataset);
        emit datasetsChanged();
    }
    else
    {
        delete dataset;
        delete xUnit;
        delete yUnit;
    }
}

void PhyxCalculator::datasetCreateStep()
{
    if (!popVariables(3))
        return;

    PhyxVariable *startVariable = variableList[0];
    PhyxFloatDataType stop = variableList[1]->value().real();
    PhyxFloatDataType step = variableList[2]->value().real();
    QString expression = expressionBuffer;

    QStringList parameters;
    parameters.append(stringBuffer);

    calculateDataset(expression, parameters, startVariable, stop, step, PhyxVariableManager::LinearDataset);

    pushVariables(0,3);
}

void PhyxCalculator::datasetCreate()
{
    if (!popVariables(2))
        return;

    PhyxVariable *startVariable = variableList[0];
    PhyxFloatDataType start = variableList[0]->value().real();
    PhyxFloatDataType stop = variableList[1]->value().real();
    PhyxFloatDataType step = (stop - start) / PHYX_FLOAT_THOUSAND;
    QString expression = expressionBuffer;

    QStringList parameters;
    parameters.append(stringBuffer);

    calculateDataset(expression, parameters, startVariable, stop, step, PhyxVariableManager::LinearDataset);

    pushVariables(0,2);
}

void PhyxCalculator::datasetLogCreateStep()
{
    if (!popVariables(3))
        return;

    PhyxVariable *startVariable = variableList[0];
    PhyxFloatDataType stop = variableList[1]->value().real();
    PhyxFloatDataType step = variableList[2]->value().real();
    QString expression = expressionBuffer;

    QStringList parameters;
    parameters.append(stringBuffer);

    calculateDataset(expression, parameters, startVariable, stop, step, PhyxVariableManager::LinearDataset);

    pushVariables(0,3);
}

void PhyxCalculator::datasetLogCreate()
{
    if (!popVariables(2))
        return;

    PhyxVariable *startVariable = variableList[0];
    PhyxFloatDataType stop = variableList[1]->value().real();
    PhyxFloatDataType step = PHYX_FLOAT_THOUSAND;
    QString expression = expressionBuffer;

    QStringList parameters;
    parameters.append(stringBuffer);

    calculateDataset(expression, parameters, startVariable, stop, step, PhyxVariableManager::LogarithmicDataset);

    pushVariables(0,2);
}
