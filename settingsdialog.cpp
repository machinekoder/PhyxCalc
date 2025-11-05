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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
#ifdef MOBILE_VERSION
    this->setWindowState(Qt::WindowMaximized);
    ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
#endif

    //set icons
    ui->listWidget->item(0)->setIcon(QIcon::fromTheme("code-context",QIcon(":/icons/code-context")));
    ui->listWidget->item(1)->setIcon(QIcon::fromTheme("edit-text-frame-update",QIcon(":/icons/edit-text-frame-update")));
    ui->listWidget->item(2)->setIcon(QIcon::fromTheme("accessories-text-editor",QIcon(":/icons/accessories-text-editor")));
    ui->listWidget->item(3)->setIcon(QIcon(":/icons/plot-icon"));

    QPushButton *button = ui->buttonBox->addButton(tr("Open Settings Directory"), QDialogButtonBox::ActionRole);
    button->setIcon(QIcon::fromTheme("document-open",QIcon(":/icons/document-open")));
    connect(button, &QPushButton::clicked,
            this, &SettingsDialog::openSettingsDir);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::setAppSettings(AppSettings *settings)
{
    //read output options
    switch (settings->output.numbers.format)
    {
    case 'e':ui->formatRadioe->setChecked(true);
             break;
    case 'E':ui->formatRadioE->setChecked(true);
             break;
    case 'f':ui->formatRadiof->setChecked(true);
             break;
    case 'g':ui->formatRadiog->setChecked(true);
             break;
    case 'G':ui->formatRadioG->setChecked(true);
             break;
    case 'x':ui->formatRadiox->setChecked(true);
             break;
    case 'X':ui->formatRadioX->setChecked(true);
             break;
    }
    ui->decimalPrecisionSpin->setValue(settings->output.numbers.decimalPrecision);
    ui->fractionCheck->setChecked(settings->output.numbers.useFractions);

    switch (settings->output.unitMode)
    {
        case 1: ui->unitRadio1->setChecked(true);
                break;
        case 2: ui->unitRadio2->setChecked(true);
                break;
        case 3: ui->unitRadio3->setChecked(true);
                break;
    }

    switch (settings->output.prefixMode)
    {
        case 1: ui->prefixRadio1->setChecked(true);
                break;
        case 2: ui->prefixRadio2->setChecked(true);
                break;
    }

    if (settings->output.imaginaryUnit == "i")
        ui->imaginaryIRadio->setChecked(true);
    else if (settings->output.imaginaryUnit == "j")
        ui->imaginaryJRadio->setChecked(true);

    //read line parser options
    ui->expressionOutputNumbersCheck->setChecked(settings->lineParser.expression.outputWithNumbers);
    ui->expressionOutputResultCheck->setChecked(settings->lineParser.expression.outputResult);

    //read text editor options
    ui->fontCombo->setCurrentFont(settings->textEditor.font);
    ui->fontSizeSpin->setValue(settings->textEditor.font.pointSize());
    ui->fontAntialiasCheck->setChecked(!(settings->textEditor.font.styleStrategy() & QFont::NoAntialias));

    ui->colorSchemeUseCheck->setChecked(settings->textEditor.useSyntaxHighlighter);
    for (int i = 0; i < settings->textEditor.colorScheme.size(); i++)
    {
        ui->colorSchemeList->setCurrentRow(i);
        setColorSchemeForeground(settings->textEditor.colorScheme.at(i).foregroundColor);
        setColorSchemeBackground(settings->textEditor.colorScheme.at(i).backgroundColor);
        setColorSchemeBold(settings->textEditor.colorScheme.at(i).bold);
        setColorSchemeItalic(settings->textEditor.colorScheme.at(i).italic);
    }

    ui->plotWindowShowCheck->setChecked(settings->plot.autoShowPlotWindow);
}

void SettingsDialog::getAppSettings(AppSettings *settings)
{
    //write output options
    if (ui->formatRadioe->isChecked())
        settings->output.numbers.format = 'e';
    else if (ui->formatRadioE->isChecked())
        settings->output.numbers.format = 'E';
    else if (ui->formatRadiof->isChecked())
        settings->output.numbers.format = 'f';
    else if (ui->formatRadiog->isChecked())
        settings->output.numbers.format = 'g';
    else if (ui->formatRadioG->isChecked())
        settings->output.numbers.format = 'G';
    else if (ui->formatRadiox->isChecked())
        settings->output.numbers.format = 'x';
    else if (ui->formatRadioX->isChecked())
        settings->output.numbers.format = 'X';

    settings->output.numbers.decimalPrecision = ui->decimalPrecisionSpin->value();
    settings->output.numbers.useFractions = ui->fractionCheck->isChecked();

    if (ui->unitRadio1->isChecked())
        settings->output.unitMode = 1;
    else if (ui->unitRadio2->isChecked())
        settings->output.unitMode = 2;
    else if (ui->unitRadio3->isChecked())
        settings->output.unitMode = 3;

    if (ui->prefixRadio1->isChecked())
        settings->output.prefixMode = 1;
    else if (ui->prefixRadio2->isChecked())
        settings->output.prefixMode = 2;

    if (ui->imaginaryIRadio->isChecked())
        settings->output.imaginaryUnit = "i";
    else if (ui->imaginaryJRadio->isChecked())
        settings->output.imaginaryUnit = "j";

    //write line parser options
    settings->lineParser.expression.outputWithNumbers = ui->expressionOutputNumbersCheck->isChecked();
    settings->lineParser.expression.outputResult = ui->expressionOutputResultCheck->isChecked();

    //write text editor options
    QFont font = ui->fontCombo->currentFont();
    font.setPointSize(ui->fontSizeSpin->value());
    if (ui->fontAntialiasCheck->isChecked())
        font.setStyleStrategy(QFont::PreferDefault);
    else
        font.setStyleStrategy(QFont::NoAntialias);
    settings->textEditor.font = font;

    settings->textEditor.useSyntaxHighlighter = ui->colorSchemeUseCheck->isChecked();
    for (int i = 0; i < settings->textEditor.colorScheme.size(); i++)
    {
        ui->colorSchemeList->setCurrentRow(i);
        settings->textEditor.colorScheme[i].foregroundColor = ui->colorSchemeList->currentItem()->foreground().color();
        settings->textEditor.colorScheme[i].backgroundColor = ui->colorSchemeList->currentItem()->background().color();
        settings->textEditor.colorScheme[i].bold            = ui->colorSchemeList->currentItem()->font().bold();
        settings->textEditor.colorScheme[i].italic          = ui->colorSchemeList->currentItem()->font().italic();
    }

    settings->plot.autoShowPlotWindow = ui->plotWindowShowCheck->isChecked();
}

void SettingsDialog::openSettingsDir()
{
    QString path = QDir::toNativeSeparators(m_settingsDir);
    QDesktopServices::openUrl(QUrl("file:///" + path));
}

void SettingsDialog::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

void SettingsDialog::on_formatRadioCustom_toggled(bool checked)
{
    ui->decimalPrecisionSpin->setEnabled(!checked);
    ui->formatCustomEdit->setEnabled(checked);
}

void SettingsDialog::setColorSchemeForeground(QColor color)
{
    ui->colorSchemeList->currentItem()->setForeground(QBrush(color));
    ui->colorSchemeForegroundButton->setStyleSheet(QString("border-color: rgb(0, 0, 0);\nborder: 2px solid;\nbackground-color: rgba(%1,%2,%3,%4)")
                                                   .arg(color.red())
                                                   .arg(color.green())
                                                   .arg(color.blue())
                                                   .arg(color.alpha()));
}

void SettingsDialog::setColorSchemeBackground(QColor color)
{
    ui->colorSchemeList->currentItem()->setBackground(QBrush(color));
    ui->colorSchemeBackgroundButton->setStyleSheet(QString("border-color: rgb(0, 0, 0);\nborder: 2px solid;\nbackground-color: rgba(%1,%2,%3,%4)")
                                                   .arg(color.red())
                                                   .arg(color.green())
                                                   .arg(color.blue())
                                                   .arg(color.alpha()));
}

void SettingsDialog::setColorSchemeBold(bool bold)
{
    QFont font = ui->colorSchemeList->currentItem()->font();
    font.setBold(bold);
    ui->colorSchemeList->currentItem()->setFont(font);
    ui->colorSchemeBoldCheck->setChecked(bold);
}

void SettingsDialog::setColorSchemeItalic(bool italic)
{
    QFont font = ui->colorSchemeList->currentItem()->font();
    font.setItalic(italic);
    ui->colorSchemeList->currentItem()->setFont(font);
    ui->colorSchemeItalicCheck->setChecked(italic);
}

void SettingsDialog::on_colorSchemeForegroundButton_clicked()
{
    QColor color = QColorDialog::getColor(ui->colorSchemeList->currentItem()->foreground().color(),
                                          this,
                                          tr("Select Foreground Color"),
                                          QColorDialog::ShowAlphaChannel);
    if (color.isValid())
        setColorSchemeForeground(color);
}

void SettingsDialog::on_colorSchemeBackgroundButton_clicked()
{
    QColor color = QColorDialog::getColor(ui->colorSchemeList->currentItem()->background().color(),
                                          this,
                                          tr("Select Background Color"),
                                          QColorDialog::ShowAlphaChannel);
    if (color.isValid())
        setColorSchemeBackground(color);
}

void SettingsDialog::on_colorSchemeDeleteForegroundButton_clicked()
{
    setColorSchemeForeground(QColor());
}

void SettingsDialog::on_colorSchemeDeleteBackgroundButton_clicked()
{
    setColorSchemeBackground(QColor(0,0,0,0));
}

void SettingsDialog::on_colorSchemeBoldCheck_clicked(bool checked)
{
    setColorSchemeBold(checked);
}

void SettingsDialog::on_colorSchemeItalicCheck_clicked(bool checked)
{
    setColorSchemeItalic(checked);
}

void SettingsDialog::on_colorSchemeList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    setColorSchemeForeground(current->foreground().color());
    if (current->background().style() != Qt::NoBrush)
        setColorSchemeBackground(current->background().color());
    else
        setColorSchemeBackground(QColor(0,0,0,0));
    setColorSchemeBold(current->font().bold());
    setColorSchemeItalic(current->font().italic());
}

void SettingsDialog::on_fontCombo_currentFontChanged(const QFont &f)
{
    ui->colorSchemeList->setFont(f);
}
