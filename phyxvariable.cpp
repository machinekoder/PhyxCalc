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

#include "phyxvariable.h"

PhyxVariable::PhyxVariable(QObject *parent) :
    QObject(parent)
{
    m_value = 1;
    m_unit = new PhyxCompoundUnit();
}

PhyxVariable::~PhyxVariable()
{
    m_unit->deleteLater();
}

bool PhyxVariable::convertUnit(PhyxCompoundUnit *unit)
{
    return m_unit->convertTo(unit);
}

void PhyxVariable::copyVariable(PhyxVariable *source, PhyxVariable *destination)
{
    //PhyxCompoundUnit *unit = new PhyxCompoundUnit();
    PhyxCompoundUnit::copyCompoundUnit(source->unit(), destination->unit());
    //destination->setUnit(unit);
    destination->setValue(source->value());
}

void PhyxVariable::setUnit(PhyxUnit *unit)
{
    m_unit->fromSimpleUnit(unit);
    connect(m_unit, &PhyxUnit::offsetValue,
            this, &PhyxVariable::offsetValue);
    connect(m_unit, &PhyxUnit::scaleValue,
            this, &PhyxVariable::scaleValue);
}

bool PhyxVariable::isComplex()
{
    return (m_value.imag() != PHYX_FLOAT_NULL);
}

bool PhyxVariable::isPositive()
{
    return (m_value.real() >= PHYX_FLOAT_NULL);
}

bool PhyxVariable::isInteger()
{
    return (!this->isComplex() && (static_cast<PhyxFloatDataType>(this->toInt()) == m_value.real()));
}

PhyxIntegerDataType PhyxVariable::toInt()
{
    return static_cast<PhyxIntegerDataType>(m_value.real());
}
