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

#ifndef PHYXVARIABLE_H
#define PHYXVARIABLE_H

#include <QObject>
#include <QSet>
#include <complex>
#include "phyxunit.h"
#include "phyxcompoundunit.h"

typedef QSet<QString>               userUnitBuffer;         /// set of input units
typedef std::complex<PhyxFloatDataType>   PhyxValueDataType;      /// the base data type for values

class PhyxVariable : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PhyxValueDataType value READ value WRITE setValue)
    Q_PROPERTY(PhyxCompoundUnit *unit READ unit WRITE setUnit)

public:
    explicit PhyxVariable(QObject *parent = 0);
    ~PhyxVariable();

    bool convertUnit(PhyxCompoundUnit *unit);
    static void copyVariable(PhyxVariable *source, PhyxVariable *destination);

    bool isComplex();
    bool isPositive();
    bool isInteger();
    PhyxIntegerDataType toInt();

    PhyxValueDataType value() const
    {
        return m_value;
    }
    PhyxCompoundUnit * unit() const
    {
        return m_unit;
    }

private:
    PhyxValueDataType   m_value;
    PhyxCompoundUnit    *m_unit;

signals:
    
public slots:

void setValue(PhyxValueDataType arg)
{
    m_value = arg;
}
void setUnit(PhyxCompoundUnit * arg)
{
    m_unit->deleteLater();

    m_unit = arg;
    connect(m_unit, &PhyxUnit::offsetValue,
            this, &PhyxVariable::offsetValue);
    connect(m_unit, &PhyxUnit::scaleValue,
            this, &PhyxVariable::scaleValue);
}
void setUnit(PhyxUnit *unit);
void offsetValue(PhyxFloatDataType offset)
{
    m_value += offset;
}
void scaleValue(PhyxFloatDataType scaleFactor)
{
    m_value *= scaleFactor;
}
};

#endif // PHYXVARIABLE_H
