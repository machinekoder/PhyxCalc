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
#include "plotdialog.h"
#include "ui_plotdialog.h"

PlotDialog::PlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotDialog)
{
    ui->setupUi(this);

#ifdef MOBILE_VERSION
    this->setWindowState(Qt::WindowMaximized);
#endif

    connect(ui->variableEdit, &QLineEdit::textChanged,
            this, &PlotDialog::updateOutput);
    connect(ui->expressionEdit, &QLineEdit::textChanged,
            this, &PlotDialog::updateOutput);
    connect(ui->startEdit, &QLineEdit::textChanged,
            this, &PlotDialog::updateOutput);
    connect(ui->stopEdit, &QLineEdit::textChanged,
            this, &PlotDialog::updateOutput);
    connect(ui->stepEdit, &QLineEdit::textChanged,
            this, &PlotDialog::updateOutput);
    connect(ui->linearRadio, &QRadioButton::clicked,
            this, &PlotDialog::updateOutput);
    connect(ui->logRadio, &QRadioButton::clicked,
            this, &PlotDialog::updateOutput);

    updateOutput();
}

PlotDialog::~PlotDialog()
{
    delete ui;
}

void PlotDialog::setExpression(const QString expression)
{
    ui->expressionEdit->setText(expression);
}

void PlotDialog::updateOutput()
{
    QString output;

    if (ui->logRadio->isChecked())
        output.append("datalog");
    else
        output.append("data");
    output.append(QString("([%1],%2,%3,%4").arg(ui->expressionEdit->text())
                  .arg(ui->variableEdit->text())
                  .arg(ui->startEdit->text())
                  .arg(ui->stopEdit->text()));
    if (!ui->stepEdit->text().isEmpty())
        output.append(QString(",%1").arg(ui->stepEdit->text()));
    output.append(")");

    ui->outputLabel->setText(output);
    m_output = output;
}

void PlotDialog::on_linearRadio_clicked()
{
    ui->stepLabel->setText(tr("Step size:"));
}

void PlotDialog::on_logRadio_clicked()
{
    ui->stepLabel->setText(tr("Steps:"));
}
