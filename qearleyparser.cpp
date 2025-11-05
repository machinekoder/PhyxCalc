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

#include "qearleyparser.h"

QEarleyParser::QEarleyParser(QObject *parent) :
    QObject(parent)
{
    isRecursionDone = false;
}

bool QEarleyParser::loadRule(QString rule, QStringList functions)
{
    //fixing the nonTerminal 0 problem, 0 is already a terminal
    if (nonTerminals.isEmpty())
    {
        nonTerminals.append(QString());
        rules.append(QList<EarleyRule>());
        isNullableVector.append(false);
    }

    int equalPos = rule.indexOf('=');
    if (equalPos == -1)
    {
        qDebug() << "Not a valid rule";
        return false;
    }

    //split premise and conclusio
    QString premise = rule.left(equalPos);
    QString conclusio = rule.mid(equalPos+1);

    EarleyRule newRule;

    newRule.premise = addNonTerminal(premise);         //convert premise

    //replace the any+ char \* -> unicode 127 DEL
    conclusio.replace("\\*",QChar(127));
    //replace the any char \~ -> unicode 27 ESC
    conclusio.replace("\\~",QChar(27));
    //replace the any char \+ -> unicode 26 SUB
    conclusio.replace("\\+",QChar(26));

    //convert conclusio
    if (!conclusio.isEmpty())   //check for epsilon rule
    {
        bool isNonTerminal = false;
        int nonTerminalPos = 0;
        for (int i = 0; i < conclusio.size(); i++)
        {
            if (conclusio.at(i) == '|')
            {
                if ((i > 0) && (conclusio.at(i-1) == '\\')) //terminated |
                {
                    newRule.conclusion.remove(newRule.conclusion.size()-1);
                    newRule.conclusion.append(conclusio.at(i).unicode());
                }
                else
                {
                    if (!isNonTerminal)
                        nonTerminalPos = i+1;
                    else
                    {
                        QString tmpNonTerminal = conclusio.mid(nonTerminalPos, i-nonTerminalPos);
                        newRule.conclusion.append(addNonTerminal(tmpNonTerminal));
                    }
                    isNonTerminal = !isNonTerminal;
                }
            }
            else if (!isNonTerminal)
            {
                newRule.conclusion.append(conclusio.at(i).unicode());
            }
        }
    }
    else
    {
        isNullableVector[-newRule.premise] = true;     //if epsilon rule, nonTerminal is nullable
    }

    newRule.functions = functions;
    rules[-newRule.premise].append(newRule);

    itemListCount = 0;   //when a rule is changed, whole parsing needs to be done again

    return true;
}

bool QEarleyParser::removeRule(QString rule)
{
    int equalPos = rule.indexOf('=');
    if (equalPos == -1)
    {
        qDebug() << "Not a valid rule";
        return false;
    }

    //split premise and conclusio
    QString premise = rule.left(equalPos);
    QString conclusio = rule.mid(equalPos+1);

    EarleyRule newRule;

    newRule.premise = -nonTerminals.indexOf(premise);      //find premise
    if (newRule.premise == 1)
    {
        qDebug() << "unknown rule";
        return false;
    }

    //replace the any+ char \* -> unicode 127 DEL
    conclusio.replace("\\*",QChar(127));
    //replace the any char \~ -> unicode 27 ESC
    conclusio.replace("\\~",QChar(27));
    //replace the any char \+ -> unicode 26 SUB
    conclusio.replace("\\+",QChar(26));

    //convert conclusio
    if (!conclusio.isEmpty())   //check for epsilon rule
    {
        bool isNonTerminal = false;
        int nonTerminalPos = 0;
        for (int i = 0; i < conclusio.size(); i++)
        {
            if (conclusio.at(i) == '|')
            {
                if ((i > 0) && (conclusio.at(i-1) == '\\')) //terminated |
                {
                    newRule.conclusion.remove(newRule.conclusion.size()-1);
                    newRule.conclusion.append(conclusio.at(i).unicode());
                }
                else
                {
                    if (!isNonTerminal)
                        nonTerminalPos = i+1;
                    else
                    {
                        QString         tmpNonTerminal = conclusio.mid(nonTerminalPos, i-nonTerminalPos);
                        EarleySymbol    tmpNTConverted;
                        tmpNTConverted = -nonTerminals.indexOf(tmpNonTerminal);      //find non terminal
                        if (tmpNTConverted == 1)
                        {
                            qDebug() << "unknown rule";
                            return false;
                        }
                        else
                            newRule.conclusion.append(tmpNTConverted);
                    }
                    isNonTerminal = !isNonTerminal;
                }
            }
            else if (!isNonTerminal)
            {
                newRule.conclusion.append(conclusio.at(i).unicode());
            }
        }
    }

    //remove the rule
    QList<EarleyRule> ruleList = rules.at(-newRule.premise);
    for (int i = (ruleList.size()-1); i >= 0; i--)
    {
        bool match = true;
        if (ruleList.at(i).conclusion.size() != newRule.conclusion.size())
            match = false;
        else
        {
            for (int i2 = 0; i2 < ruleList.at(i).conclusion.size(); i2++)
            {
                if (ruleList.at(i).conclusion.at(i2) != newRule.conclusion.at(i2))
                {
                    match = false;
                    break;
                }
            }
        }

        if (match)
        {
            rules[-newRule.premise].removeAt(i);

            itemListCount = 0;   //when a rule is changed, whole parsing needs to be done again
            return true;
        }
    }

    qDebug() << "unknown rule";
    return false;
}

/*bool QEarleyParser::loadRules(QStringList ruleList)
{
    nonTerminals.clear();
    rules.clear();
    isNullableVector.clear();

    foreach (QString rule, ruleList)
    {
        if (!loadRule(rule))
            return false;
    }

    qDebug() << "non terminals:" << nonTerminals;

    return true;
}
*/

EarleySymbol QEarleyParser::addNonTerminal(QString nonTerminal)
{
    if (nonTerminals.contains(nonTerminal))
        return -nonTerminals.indexOf(nonTerminal);
    else
    {
        nonTerminals.append(nonTerminal);
        rules.append(QList<EarleyRule>());
        isNullableVector.append(false);
        return -(nonTerminals.size()-1);
    }
}

void QEarleyParser::initialize()
{
    itemListCount = word.conclusion.size() + 1;

    earleyItemLists.clear();
    for (int i = 0; i < itemListCount; i++)
    {
        earleyItemLists.append(EarleyItemList());
    }
}

bool QEarleyParser::parse(int startPosition)
{
    int currentIndex = startPosition;

    if (startPosition == 0)
    {
        //predictor special case
        for (int i = 0; i < rules.at(-startSymbol).size(); i++)
        {
            appendEarleyItem(0, &rules[-startSymbol][i] ,0 , 0, NULL);
        }
    }

    for (int listIndex = startPosition; listIndex < itemListCount; listIndex++)
    {
        int oldcount = -1;
        while (earleyItemLists.at(currentIndex).size() != oldcount)
        {
            oldcount = earleyItemLists.at(currentIndex).size();

            for (int itemIndex = 0; itemIndex < earleyItemLists.at(currentIndex).size(); itemIndex++)
            {
                EarleyItem *item = &earleyItemLists[currentIndex][itemIndex];
                if (!(item->dotPos == item->rule->conclusion.size()))    //check for final state (beta is empty)
                {
                    EarleySymbol firstSymbol = item->rule->conclusion.at(item->dotPos);
                    if (firstSymbol < 0)    //if symbol < 0, symbol = nonTerminal
                    {
                        //Predictor
                        for (int i = 0; i < rules.at(-firstSymbol).size(); i++)
                        {
                            appendEarleyItem(currentIndex, &(rules[-firstSymbol][i]) ,0 , currentIndex, item);
                        }
                        //Aycock and Horspool Epsilon solution
                        if (isNullableVector.at(-firstSymbol))  //if B is nullable
                        {
                            appendEarleyItem(currentIndex, item->rule, item->dotPos+1, item->startPos, item);   //move point right
                        }
                    }
                    else if (currentIndex < (itemListCount-1))
                    {
                        //Scanner
                        if ((word.conclusion.at(currentIndex) == firstSymbol)
                                || (firstSymbol == 127)
                                || ((firstSymbol == 26) && (word.conclusion.at(currentIndex) != '='))
                                || ((firstSymbol == 27) && !QString(word.conclusion.at(currentIndex)).contains(QRegularExpression(EXCLUDED_CHARS))))  //127 is the any+ char, 27 the any char
                        {
                            appendEarleyItem(currentIndex+1, item->rule, item->dotPos+1, item->startPos, item);   //move point right
                        }
                    }
                }
                else
                {
                    //Completer
                    for (int i = 0; i < earleyItemLists.at(item->startPos).size(); i++)
                    {
                        EarleyItem *item2 = &earleyItemLists[item->startPos][i];
                        if (!(item2->dotPos == item2->rule->conclusion.size()) && (item2->rule->conclusion.at(item2->dotPos) == item->premise()))  //item2 is not empty and item2 B = item A
                        {
                            appendEarleyItem(currentIndex, item2->rule, item2->dotPos+1, item2->startPos, item);   //move point right
                        }
                    }
                }
            }
        }
        currentIndex++;
    }

    isRecursionDone = false;        //the itemLists are fine and partial parsing is possible

    //check wheter parsing was successful or not
    return checkSuccessful();
}

bool QEarleyParser::checkSuccessful()
{
    for (int i = 0; i < earleyItemLists.last().size(); i++)
    {
        EarleyItem *item = &earleyItemLists.last()[i];
        if ((item->premise() == startSymbol) && (item->dotPos == item->rule->conclusion.size()))
            return true;
    }
    return false;
}

void QEarleyParser::setWord(QString earleyWord)
{
    word.conclusion.clear();
    foreach (QChar character, earleyWord)
    {
        word.conclusion.append(character.unicode());
    }

    initialize();
}

void QEarleyParser::setStartSymbol(QString earleyStartSymbol)
{
    startSymbol = -nonTerminals.indexOf(earleyStartSymbol);
}

// this function is only for testing purposes
bool QEarleyParser::parseWord(QString earleyWord)
{
    setWord(earleyWord);
    return parse();
}

void QEarleyParser::clearWord()
{
    itemListCount = 0;
    word.conclusion.clear();
    earleyItemLists.clear();
}

bool QEarleyParser::addSymbol(QChar earleySymbol)
{
    word.conclusion.append(earleySymbol.unicode());

    if (itemListCount != 0) //rules changed
    {
        itemListCount++;
        earleyItemLists.append(EarleyItemList());

        if (isRecursionDone)    // if tree was already created whole parsing needs to be done
        {
            for (int i = 0; i < itemListCount; i++)
                earleyItemLists[i].clear();
            return parse();
        }
        else                    //else partial parsing is possible
            return parse(itemListCount-2);
    }
    else
    {
        initialize();
        return parse();
    }
}

bool QEarleyParser::removeSymbol()
{
    if (word.conclusion.size() > 0)
        word.conclusion.remove(word.conclusion.size()-1);
    if (itemListCount != 0) // rules changed
    {
        itemListCount--;
        earleyItemLists.removeLast();
        return checkSuccessful();
    }
    else
    {
        initialize();
        return parse();
    }
}

void QEarleyParser::appendEarleyItem(int index, EarleyRule *rule, int dotPos, int K, EarleyItem *origin)
{
    /*bool match = false;
    foreach (EarleyItem item, earleyItemLists.at(index))
    {
        match |= (item.A == A) && (item.alpha == alpha) && (item.beta == beta) && (item.K == K);
        if (match)
            return;
    }*/

    EarleyItem earleyItem;
    earleyItem.rule = rule;
    earleyItem.dotPos = dotPos;
    earleyItem.startPos = K;
    earleyItem.origin = origin;

    if (!earleyItemLists.at(index).contains(earleyItem))
        earleyItemLists[index].append(earleyItem);
    else
        return;

    //qDebug() << index << A << *rule << dotPos << K;
}

void QEarleyParser::treeRecursion(int listIndex, int itemIndex, EarleyItemList& tree)
{
    if (tree.at(0).dotPos == 0)  //first element (the start rule) must be backward completed
    {
        earleyItemResultLists = tree;  //save tree to global variable
    }
    else
    {
        if (tree.at(itemIndex).dotPos != 0)   //alpha is not empty
        {
            EarleySymbol lastSymbol = tree.at(itemIndex).rule->conclusion.at(tree.at(itemIndex).dotPos-1);   //symbol before dot
            if (lastSymbol < 0)     //if symbol < 0, symbol = nonTerminal
            {
                //backward predictor
                int kj = tree.at(itemIndex).startPos;
                int pj = tree.at(itemIndex).dotPos;
                for (int i = 0; i < earleyItemLists.at(listIndex).size(); i++)//foreach (EarleyItem item, earleyItemLists.at(listIndex))
                {
                    EarleyItem *item = &earleyItemLists[listIndex][i];
                    if ((item->premise() == lastSymbol) && (item->dotPos == item->rule->conclusion.size()) && (item->startPos >= (kj+pj-1)))
                    {
                        EarleyItemList treeCopy = tree;
                        treeCopy.insert(itemIndex+1,*item);
                        treeRecursion(listIndex, itemIndex+1, treeCopy);
                    }
                }
            }
            else
            {
                //backward scanner
                if ((word.conclusion.at(listIndex-1) == tree[itemIndex].prevSymbol()) || (word.conclusion.at(listIndex-1) == 127))
                {
                    tree[itemIndex].dotPos--;       //move dot left
                    treeRecursion(listIndex-1, itemIndex, tree);
                }
                else
                {
                    tree.removeAt(itemIndex);
                }
            }
        }
        else
        {
            //backward completer
            if (itemIndex > 0)
            {
                int kj = tree.at(itemIndex).startPos;
                int kj1 = tree.at(itemIndex-1).startPos;
                int pj1 = tree.at(itemIndex-1).dotPos;
                if ((pj1 > 1) || ((pj1 == 1) && (kj == kj1)))
                {
                    tree[itemIndex-1].dotPos--;                  //move point left
                    treeRecursion(listIndex, itemIndex-1, tree);
                }
            }
        }
    }
}

QString QEarleyParser::EarleyItemToString(EarleyItem item)
{
    QString itemString;
    itemString.append(nonTerminals.at(-item.premise()));
    itemString.append("=");
    for (int i=0; i<item.rule->conclusion.size(); i++)
    {
        if (item.dotPos == i) itemString.append("@");
        if (item.rule->conclusion[i] < 0)
            itemString.append("|" + nonTerminals.at(-item.rule->conclusion[i]) + "|");
        else
            itemString.append(QChar(item.rule->conclusion[i]));
    }
    if (item.dotPos == item.rule->conclusion.size()) itemString.append("@");
    //itemString.append(" , " + QString::number(item.K));
    return itemString;
}

QList<EarleyTreeItem> QEarleyParser::getTree()
{
    EarleyItemList tree;

    //remove unneeded items
    for (int listIndex = 0; listIndex < itemListCount; listIndex++)
    {
        for (int i = earleyItemLists.at(listIndex).size()-1; i >= 0 ; i--)
        {
            earleyItemLists[listIndex][i].endPos = listIndex-1;  //store end position of item for later use in evaluation
            /*if (!(earleyItemLists.at(listIndex).at(i).dotPos == earleyItemLists.at(listIndex).at(i).rule->conclusion.size()))  //not beta is empty
                earleyItemLists[listIndex].removeAt(i);*/
        }
    }
    isRecursionDone = true;     //must be set, lists are destroyed and partial parsing is not possible any more

    //for testing purposes only
    /*qDebug() << "Earley items after Parsing:";
    for (int x = 0; x < itemListCount; x++)
    {
        for (int y = 0; y < earleyItemLists.at(x).size(); y++)
        //foreach (EarleyItem item, earleyItemLists.at(x))
        {
            EarleyItem *item = &earleyItemLists[x][y];
            qDebug() << x << item << EarleyItemToString(*item) << item->startPos << item->origin;
        }
    }*/

    backtraceTree(&tree);

    //add the final item
    /*for (int i = 0; i < earleyItemLists.last().size(); i++)
    {
        EarleyItem *item = &earleyItemLists.last()[i];
        if ((item->premise() == startSymbol) && (item->dotPos == item->rule->conclusion.size()))
        {
            tree.append(*item);
        }
    }

    treeRecursion(itemListCount-1,0,tree);

    tree = earleyItemResultLists; //result has been stored in the deepest stack level of recursion
    */
    //for testing purposes only
    /*qDebug() << "Earley items after tree recursion:";
    foreach (EarleyItem item, tree)
    {
        qDebug() << EarleyItemToString(item) << item.startPos;
    }
    qDebug();*/

    return tree;
}

void QEarleyParser::backtraceTree(EarleyItemList *tree)
{
    EarleyItem *currentItem = NULL;

    //get the startItem
    for (int i = 0; i < earleyItemLists.last().size(); i++)
    {
        EarleyItem *item = &earleyItemLists.last()[i];
        if ((item->premise() == startSymbol) && (item->isFinal()))
        {
            currentItem = item;
        }
    }

    while (currentItem->origin != NULL)
    {
        if (currentItem->isFinal() && !currentItem->rule->functions.isEmpty())
            tree->append(*currentItem);
        currentItem = currentItem->origin;
    }
}
