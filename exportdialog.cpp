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

#include "exportdialog.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

    QPushButton *button = ui->buttonBox->addButton(tr("Copy to Clipboard"), QDialogButtonBox::ActionRole);
    connect(button, &QPushButton::clicked,
            this, &ExportDialog::copyToClipboard);

#ifdef MOBILE_VERSION
    this->setWindowState(Qt::WindowMaximized);
#else
    button = ui->buttonBox->addButton(tr("Open Formel Editor"), QDialogButtonBox::ActionRole);
    connect(button, &QPushButton::clicked,
            this, &ExportDialog::openFormelEditor);
#endif
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::setText(QString text)
{
    ui->plainTextEdit->setPlainText(text);
}

void ExportDialog::copyToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->plainTextEdit->toPlainText());
}

void ExportDialog::openFormelEditor()
{
    int returnvar = system("libreoffice --math");
    qDebug() << "formel editor returned" << returnvar;
}
