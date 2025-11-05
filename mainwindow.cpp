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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //nasty workaround for finding the settings directory
    QSettings tmpConfig(QSettings::IniFormat, QSettings::UserScope, "phyxcalc", "settings");
    settingsDir = QFileInfo(tmpConfig.fileName()).absolutePath() + "/";
    autosaveFilename = settingsDir + "autosave.txt";
    //settingsDir = QDir::currentPath() + "/settings/";
    firstStartConfig();

    unitLoader = new UnitLoader();
    unitLoader->loadSymbols(settingsDir);

    ui->tabWidget->removeTab(0);
    ui->tabWidget->removeTab(0);

    connect(ui->tabWidget, &QTabWidget::currentChanged,
            this, &MainWindow::tabChanged);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested,
            this, &MainWindow::closeTab);

    connect(ui->actionExit, &QAction::triggered,
            this, &MainWindow::close);
    connect(ui->actionClose_All, &QAction::triggered,
            this, &MainWindow::closeAllTabs);

    initializeGUI();
    loadAllDocks();

    loadSettings();
    addNewTab();

    restoreDocument();  //in case of a crash restore the document
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!closeAllTabs())
        event->ignore();
    else
    {
        saveSettings();
        deleteAutosaves();
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == documentList.at(activeTab)->expressionEdit) {
        if (event->type() == QEvent::KeyPress)
        {
             QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

             if ((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Enter))
             {
                 //check for comment
                 if (documentList.at(activeTab)->lineParser->commentLineSelected())
                    return QMainWindow::eventFilter(obj, event);

                 if (keyEvent->modifiers() == Qt::ShiftModifier)
                     documentList.at(activeTab)->lineParser->insertNewLine(true);
                 else if (keyEvent->modifiers() == Qt::ControlModifier)
                    documentList.at(activeTab)->lineParser->parseLine(false);
                 else
                    documentList.at(activeTab)->lineParser->parseLine(true);

                 autosaveDocument();

                 return true;
             }
             else if ((keyEvent->key() == Qt::Key_Delete) || (keyEvent->key() == Qt::Key_Backspace))
             {
                 if (documentList.at(activeTab)->lineParser->resultLineSelected())
                 {
                     documentList.at(activeTab)->lineParser->deleteLine();
                     return true;
                 }
             }
        }
        else if (event->type() == QEvent::ToolTip)  //mouseover tooltips
        {
                QHelpEvent* helpEvent = static_cast <QHelpEvent*>(event);
                QTextCursor cursor = documentList.at(activeTab)->expressionEdit->cursorForPosition(helpEvent->pos());
                int pos = cursor.positionInBlock();
                foreach (QTextLayout::FormatRange range, cursor.block().layout()->additionalFormats()) //syntaxhighlighter formats are additional formats
                {
                    if (pos >= range.start && pos <= range.start + range.length)    //get current format
                    {
                        cursor.setPosition(cursor.block().position() + range.start);
                        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, range.length);
                        if (range.format.toolTip() == "variable")   //tooltip is used in syntaxhighlighter
                        {
                            QToolTip::showText(helpEvent->globalPos(),
                                               documentList.at(activeTab)->lineParser->variableToolTip(cursor.selectedText()));
                        }
                        else if (range.format.toolTip() == "constant")
                        {
                            QToolTip::showText(helpEvent->globalPos(),
                                               documentList.at(activeTab)->lineParser->constantToolTip(cursor.selectedText()));
                        }
                        else if (range.format.toolTip() == "function")
                        {
                            QToolTip::showText(helpEvent->globalPos(),
                                               documentList.at(activeTab)->lineParser->functionToolTip(cursor.selectedText()));
                        }
                        else
                            QToolTip::hideText();
                        break;
                    }
                }
                return true;
        }
        return QMainWindow::eventFilter(obj, event);
     } else {
         // standard event processing
         return QMainWindow::eventFilter(obj, event);
     }
}

void MainWindow::copyResource(QString name)
{
    if (!QFile::exists(settingsDir + name))
    {
        QFile::copy(":/settings/" + name, settingsDir + name);
        QFile::setPermissions(settingsDir + name, QFile::ReadOwner | QFile::WriteOwner);
    }
}

void MainWindow::firstStartConfig()
{
    QDir().mkdir(settingsDir);

    copyResource("definitions.txt");
    copyResource("settings.ini");
    copyResource("symbols.txt");
    copyResource("docks.txt");
    copyResource("numbersDock.txt");
    copyResource("operatorsDock.txt");
    copyResource("electronicFunctions.txt");
}

void MainWindow::loadSettings()
{
    QSettings settings(settingsDir + "settings.ini", QSettings::IniFormat);

    settings.beginGroup("window");
        this->restoreState(settings.value("state", QByteArray()).toByteArray());
        this->restoreGeometry(settings.value("geometry", QByteArray()).toByteArray());
        ui->action_Slim_Mode->setChecked(settings.value("slimMode", true).toBool());
        on_action_Slim_Mode_triggered();
    settings.endGroup();

    settings.beginGroup("variableDock");
        ui->variableTable->setColumnWidth(0, settings.value("column1Width",80).toInt());
        ui->variableTable->setColumnWidth(1, settings.value("column2Width",80).toInt());
        ui->variableTable->setColumnWidth(2, settings.value("column3Width",80).toInt());
    settings.endGroup();

    settings.beginGroup("constantDock");
        ui->constantsTable->setColumnWidth(0, settings.value("column1Width",80).toInt());
        ui->constantsTable->setColumnWidth(1, settings.value("column2Width",80).toInt());
        ui->constantsTable->setColumnWidth(2, settings.value("column3Width",80).toInt());
    settings.endGroup();

    settings.beginGroup("unitsDock");
        ui->unitsTable->setColumnWidth(0, settings.value("column1Width",60).toInt());
        ui->unitsTable->setColumnWidth(1, settings.value("column2Width",80).toInt());
        ui->unitsTable->setColumnWidth(2, settings.value("column3Width",80).toInt());
        ui->unitsTable->setColumnWidth(3, settings.value("column4Width",80).toInt());
        ui->unitsTable->setColumnWidth(4, settings.value("column5Width",80).toInt());
    settings.endGroup();

    settings.beginGroup("prefixesDock");
        ui->prefixesTable->setColumnWidth(0, settings.value("column1Width",60).toInt());
        ui->prefixesTable->setColumnWidth(1, settings.value("column2Width",80).toInt());
        ui->prefixesTable->setColumnWidth(2, settings.value("column3Width",80).toInt());
    settings.endGroup();

    settings.beginGroup("output");
        settings.beginGroup("numbers");
            appSettings.output.numbers.decimalPrecision = settings.value("decimalPrecision", 6).toInt();
            appSettings.output.numbers.format = settings.value("format", 'g').toInt();
            appSettings.output.numbers.useFractions = settings.value("useFractions", false).toBool();
        settings.endGroup();
        appSettings.output.unitMode = settings.value("unitMode",1).toInt();
        appSettings.output.imaginaryUnit = settings.value("imaginaryUnit", "i").toString();
        appSettings.output.prefixMode = settings.value("prefixMode",1).toInt();
    settings.endGroup();
    settings.beginGroup("lineParser");
        settings.beginGroup("expression");
            appSettings.lineParser.expression.outputWithNumbers = settings.value("outputWithNumbers", false).toBool();
            appSettings.lineParser.expression.outputResult = settings.value("outputResult", false).toBool();
        settings.endGroup();
    settings.endGroup();
    settings.beginGroup("textEditor");
        appSettings.textEditor.font.fromString(settings.value("font", "Monospace").toString());
        if (settings.value("fontAntialiasing", true).toBool())
            appSettings.textEditor.font.setStyleStrategy(QFont::PreferDefault);
        else
            appSettings.textEditor.font.setStyleStrategy(QFont::NoAntialias);
        appSettings.textEditor.useSyntaxHighlighter = settings.value("useSyntaxHighlighter", true).toBool();
        settings.beginGroup("colorScheme");
            colorSchemeItem item;
            settings.beginGroup("text");
                item.foregroundColor = settings.value("foregroundColor", QColor(Qt::black)).value<QColor>();
                item.backgroundColor = settings.value("backgroundColor", QColor(Qt::transparent)).value<QColor>();
                item.bold            = settings.value("bold", false).toBool();
                item.italic          = settings.value("italic", false).toBool();
                appSettings.textEditor.colorScheme.append(item);
            settings.endGroup();
            settings.beginGroup("selection");
                item.foregroundColor = settings.value("foregroundColor", QColor(255,255,255)).value<QColor>();
                item.backgroundColor = settings.value("backgroundColor", QColor(144,168,192)).value<QColor>();
                item.bold            = settings.value("bold", false).toBool();
                item.italic          = settings.value("italic", false).toBool();
                appSettings.textEditor.colorScheme.append(item);
            settings.endGroup();
            settings.beginGroup("number");
                item.foregroundColor = settings.value("foregroundColor", QColor(Qt::darkBlue)).value<QColor>();
                item.backgroundColor = settings.value("backgroundColor", QColor(Qt::transparent)).value<QColor>();
                item.bold            = settings.value("bold", false).toBool();
                item.italic          = settings.value("italic", false).toBool();
                appSettings.textEditor.colorScheme.append(item);
            settings.endGroup();
            settings.beginGroup("string");
                item.foregroundColor = settings.value("foregroundColor", QColor(Qt::darkGreen)).value<QColor>();
                item.backgroundColor = settings.value("backgroundColor", QColor(Qt::transparent)).value<QColor>();
                item.bold            = settings.value("bold", false).toBool();
                item.italic          = settings.value("italic", false).toBool();
                appSettings.textEditor.colorScheme.append(item);
            settings.endGroup();
            settings.beginGroup("comment");
                item.foregroundColor = settings.value("foregroundColor", QColor(Qt::darkGreen)).value<QColor>();
                item.backgroundColor = settings.value("backgroundColor", QColor(Qt::transparent)).value<QColor>();
                item.bold            = settings.value("bold", false).toBool();
                item.italic          = settings.value("italic", false).toBool();
                appSettings.textEditor.colorScheme.append(item);
            settings.endGroup();
            settings.beginGroup("keyword");
                item.foregroundColor = settings.value("foregroundColor", QColor(Qt::darkYellow)).value<QColor>();
                item.backgroundColor = settings.value("backgroundColor", QColor(Qt::transparent)).value<QColor>();
                item.bold            = settings.value("bold", false).toBool();
                item.italic          = settings.value("italic", false).toBool();
                appSettings.textEditor.colorScheme.append(item);
            settings.endGroup();
            settings.beginGroup("function");
                item.foregroundColor = settings.value("foregroundColor", QColor(Qt::black)).value<QColor>();
                item.backgroundColor = settings.value("backgroundColor", QColor(Qt::transparent)).value<QColor>();
                item.bold            = settings.value("bold", false).toBool();
                item.italic          = settings.value("italic", true).toBool();
                appSettings.textEditor.colorScheme.append(item);
            settings.endGroup();
            settings.beginGroup("variable");
                item.foregroundColor = settings.value("foregroundColor", QColor(Qt::darkMagenta)).value<QColor>();
                item.backgroundColor = settings.value("backgroundColor", QColor(Qt::transparent)).value<QColor>();
                item.bold            = settings.value("bold", false).toBool();
                item.italic          = settings.value("italic", false).toBool();
                appSettings.textEditor.colorScheme.append(item);
            settings.endGroup();
            settings.beginGroup("constant");
                item.foregroundColor = settings.value("foregroundColor", QColor(Qt::darkRed)).value<QColor>();
                item.backgroundColor = settings.value("backgroundColor", QColor(Qt::transparent)).value<QColor>();
                item.bold            = settings.value("bold", false).toBool();
                item.italic          = settings.value("italic", false).toBool();
                appSettings.textEditor.colorScheme.append(item);
            settings.endGroup();
            settings.beginGroup("unit");
                item.foregroundColor = settings.value("foregroundColor", QColor(Qt::black)).value<QColor>();
                item.backgroundColor = settings.value("backgroundColor", QColor(Qt::transparent)).value<QColor>();
                item.bold            = settings.value("bold", false).toBool();
                item.italic          = settings.value("italic", false).toBool();
                appSettings.textEditor.colorScheme.append(item);
            settings.endGroup();
            settings.beginGroup("error");
                item.foregroundColor = settings.value("foregroundColor", QColor(Qt::red)).value<QColor>();
                item.backgroundColor = settings.value("backgroundColor", QColor(Qt::transparent)).value<QColor>();
                item.bold            = settings.value("bold", false).toBool();
                item.italic          = settings.value("italic", false).toBool();
                appSettings.textEditor.colorScheme.append(item);
            settings.endGroup();
        settings.endGroup();
    settings.endGroup();

    settings.beginGroup("plot");
        appSettings.plot.autoShowPlotWindow = settings.value("autoShowPlotWindow",true).toBool();
    settings.endGroup();

    recentDocuments = settings.value("recentDocuments", QStringList()).toStringList();
    updateRecentDocuments();
}

void MainWindow::saveSettings()
{
    QSettings settings(settingsDir + "settings.ini", QSettings::IniFormat);

    settings.beginGroup("window");
        settings.setValue("state",this->saveState());
        settings.setValue("geometry", this->saveGeometry());
        settings.setValue("slimMode", ui->action_Slim_Mode->isChecked());
    settings.endGroup();

    settings.beginGroup("variableDock");
        settings.setValue("column1Width",ui->variableTable->columnWidth(0));
        settings.setValue("column2Width",ui->variableTable->columnWidth(1));
        settings.setValue("column3Width",ui->variableTable->columnWidth(2));
    settings.endGroup();

    settings.beginGroup("constantDock");
        settings.setValue("column1Width",ui->constantsTable->columnWidth(0));
        settings.setValue("column2Width",ui->constantsTable->columnWidth(1));
        settings.setValue("column3Width",ui->constantsTable->columnWidth(2));
    settings.endGroup();

    settings.beginGroup("unitsDock");
        settings.setValue("column1Width",ui->unitsTable->columnWidth(0));
        settings.setValue("column2Width",ui->unitsTable->columnWidth(1));
        settings.setValue("column3Width",ui->unitsTable->columnWidth(2));
        settings.setValue("column4Width",ui->unitsTable->columnWidth(3));
        settings.setValue("column5Width",ui->unitsTable->columnWidth(4));
    settings.endGroup();

    settings.beginGroup("prefixesDock");
        settings.setValue("column1Width",ui->prefixesTable->columnWidth(0));
        settings.setValue("column2Width",ui->prefixesTable->columnWidth(1));
        settings.setValue("column3Width",ui->prefixesTable->columnWidth(2));
    settings.endGroup();

    settings.beginGroup("output");
        settings.beginGroup("numbers");
            settings.setValue("decimalPrecision", appSettings.output.numbers.decimalPrecision);
            settings.setValue("format", appSettings.output.numbers.format);
            settings.setValue("useFractions", appSettings.output.numbers.useFractions);
        settings.endGroup();
        settings.setValue("unitMode", appSettings.output.unitMode);
        settings.setValue("imaginaryUnit", appSettings.output.imaginaryUnit);
        settings.setValue("prefixMode", appSettings.output.prefixMode);
    settings.endGroup();
    settings.beginGroup("lineParser");
        settings.beginGroup("expression");
            settings.setValue("outputWithNumbers", appSettings.lineParser.expression.outputWithNumbers);
            settings.setValue("outputResult", appSettings.lineParser.expression.outputResult);
        settings.endGroup();
    settings.endGroup();
    settings.beginGroup("textEditor");
        settings.setValue("font", appSettings.textEditor.font.toString());
        settings.setValue("fontAntialiasing", !(appSettings.textEditor.font.styleStrategy() & QFont::NoAntialias));
        settings.setValue("useSyntaxHighlighter", appSettings.textEditor.useSyntaxHighlighter);
        settings.beginGroup("colorScheme");
            colorSchemeItem item;
            int n = 0;
            for (int i = 0; i < appSettings.textEditor.colorScheme.size(); i++)
            {
                QString group;
                switch (i) {
                    case 0: group = "text"; break;
                    case 1: group = "selection"; break;
                    case 2: group = "number"; break;
                    case 3: group = "string"; break;
                    case 4: group = "comment"; break;
                    case 5: group = "keyword"; break;
                    case 6: group = "function"; break;
                    case 7: group = "variable"; break;
                    case 8: group = "constant"; break;
                    case 9: group = "unit"; break;
                    case 10: group = "error"; break;
                }
                settings.beginGroup(group);
                item = appSettings.textEditor.colorScheme.at(i);
                settings.setValue("foregroundColor", item.foregroundColor);
                settings.setValue("backgroundColor", item.backgroundColor);
                settings.setValue("bold",            item.bold);
                settings.setValue("italic",          item.italic);
                n++;
                settings.endGroup();
            }
        settings.endGroup();
    settings.endGroup();

    settings.beginGroup("plot");
        settings.setValue("autoShowPlotWindow", appSettings.plot.autoShowPlotWindow);
    settings.endGroup();

    settings.setValue("recentDocuments", recentDocuments);

    settings.sync();
}

void MainWindow::initializeGUI()
{
    //initialize variable Dock
    QTableWidgetItem *newItem;

    ui->variableTable->setColumnCount(3);
    ui->variableTable->setRowCount(0);
    ui->variableTable->setSortingEnabled(true);

    QFont font;
    font.setWeight(QFont::Bold);

    newItem = new QTableWidgetItem(tr("Variable"));
    newItem->setFont(font);
    ui->variableTable->setHorizontalHeaderItem(0, newItem);
    newItem = new QTableWidgetItem(tr("Value"));
    newItem->setFont(font);
    ui->variableTable->setHorizontalHeaderItem(1, newItem);
    newItem = new QTableWidgetItem(tr("Unit"));
    newItem->setFont(font);
    ui->variableTable->setHorizontalHeaderItem(2, newItem);

    connect(ui->actionVariables, &QAction::toggled,
            ui->variablesDock, &QDockWidget::setVisible);
    connect(ui->variablesDock, &QDockWidget::visibilityChanged,
            ui->actionVariables, &QAction::setChecked);

    //initialize constant Dock

    ui->constantsTable->setColumnCount(3);
    ui->constantsTable->setRowCount(0);
    ui->constantsTable->setSortingEnabled(true);

    newItem = new QTableWidgetItem(tr("Constant"));
    newItem->setFont(font);
    ui->constantsTable->setHorizontalHeaderItem(0, newItem);
    newItem = new QTableWidgetItem(tr("Value"));
    newItem->setFont(font);
    ui->constantsTable->setHorizontalHeaderItem(1, newItem);
    newItem = new QTableWidgetItem(tr("Unit"));
    newItem->setFont(font);
    ui->constantsTable->setHorizontalHeaderItem(2, newItem);

    connect(ui->actionConstants, &QAction::toggled,
            ui->constantsDock, &QDockWidget::setVisible);
    connect(ui->constantsDock, &QDockWidget::visibilityChanged,
            ui->actionConstants, &QAction::setChecked);

    //initialize units Dock
    ui->unitsTable->setColumnCount(5);
    ui->unitsTable->setRowCount(0);
    ui->unitsTable->setSortingEnabled(true);

    newItem = new QTableWidgetItem(tr("Unit"));
    newItem->setFont(font);
    ui->unitsTable->setHorizontalHeaderItem(0, newItem);
    newItem = new QTableWidgetItem(tr("Dimension"));
    newItem->setFont(font);
    ui->unitsTable->setHorizontalHeaderItem(1, newItem);
    newItem = new QTableWidgetItem(tr("Scale Factor"));
    newItem->setFont(font);
    ui->unitsTable->setHorizontalHeaderItem(2, newItem);
    newItem = new QTableWidgetItem(tr("Offset"));
    newItem->setFont(font);
    ui->unitsTable->setHorizontalHeaderItem(3, newItem);
    newItem = new QTableWidgetItem(tr("Unit System"));
    newItem->setFont(font);
    ui->unitsTable->setHorizontalHeaderItem(4, newItem);

    connect(ui->actionUnits, &QAction::toggled,
            ui->unitsDock, &QDockWidget::setVisible);
    connect(ui->unitsDock, &QDockWidget::visibilityChanged,
            ui->actionUnits, &QAction::setChecked);

    //initialize prefixes Dock
    ui->prefixesTable->setColumnCount(3);
    ui->prefixesTable->setRowCount(0);
    ui->prefixesTable->setSortingEnabled(true);

    newItem = new QTableWidgetItem(tr("Prefix"));
    newItem->setFont(font);
    ui->prefixesTable->setHorizontalHeaderItem(0, newItem);
    newItem = new QTableWidgetItem(tr("Value"));
    newItem->setFont(font);
    ui->prefixesTable->setHorizontalHeaderItem(1, newItem);
    newItem = new QTableWidgetItem(tr("Unit System"));
    newItem->setFont(font);
    ui->prefixesTable->setHorizontalHeaderItem(2, newItem);

    connect(ui->actionPrefixes, &QAction::toggled,
            ui->prefixesDock, &QDockWidget::setVisible);
    connect(ui->prefixesDock, &QDockWidget::visibilityChanged,
            ui->actionPrefixes, &QAction::setChecked);

    //initialize functions Dock

    ui->functionsList->setViewMode(QListView::IconMode);
    ui->functionsList->setResizeMode(QListView::Adjust);

    connect(ui->functionsList, &QListWidget::itemClicked,
            this, &MainWindow::dockWidgetPressed);
    connect(ui->actionFunctions, &QAction::toggled,
            ui->functionsDock, &QDockWidget::setVisible);
    connect(ui->functionsDock, &QDockWidget::visibilityChanged,
            ui->actionFunctions, &QAction::setChecked);

    //initialize special buttons
    QMenu *configureMenu = new QMenu(this);
    configureMenu->addMenu(ui->menuFile);
    configureMenu->addMenu(ui->menuEdit);
    configureMenu->addMenu(ui->menuCalculation);
    configureMenu->addMenu(ui->menuWindow);
    //configureMenu->addMenu(ui->menuTools);
    configureMenu->addMenu(ui->menuDocks);
    configureMenu->addAction(ui->actionSettings);
    configureMenu->addAction(ui->actionHelp);
    configureMenu->addAction(ui->actionAbout);
    //configureMenu->addMenu(ui->menuHelp);
    ui->actionConfigure_and_control->setMenu(configureMenu);
    QToolButton *configureButton = new QToolButton();
    configureButton->setText(tr("Configure"));
    configureButton->setPopupMode(QToolButton::InstantPopup);
    configureButton->setMenu(configureMenu);

    //initialize theme icons
    ui->actionAbout->setIcon(QIcon::fromTheme("help-about",QIcon(":/icons/help-about")));
    ui->actionClear_Variables->setIcon(QIcon::fromTheme("edit-clear",QIcon(":/icons/edit-clear")));
    ui->actionClose->setIcon(QIcon::fromTheme("dialog-close",QIcon(":/icons/dialog-close")));
    //ui->actionClose_All
    //ui->actionClose_Other
    ui->actionConfigure_and_control->setIcon(QIcon::fromTheme("configure",QIcon(":/icons/configure")));
    //ui->actionConstants
    ui->actionCopy->setIcon(QIcon::fromTheme("edit-copy",QIcon(":/icons/edit-copy")));
    ui->actionCut->setIcon(QIcon::fromTheme("edit-cut",QIcon(":/icons/edit-cut")));
    ui->actionExit->setIcon(QIcon::fromTheme("application-exit",QIcon(":/icons/application-exit")));
    ui->actionExport->setIcon(QIcon::fromTheme("document-export",QIcon(":/icons/document-export")));
    ui->actionHelp->setIcon(QIcon::fromTheme("help-contents", QIcon(":/icons/help-contents")));
    ui->actionNew->setIcon(QIcon::fromTheme("document-new",QIcon(":/icons/document-new")));
    ui->actionNew_Tab->setIcon(QIcon::fromTheme("tab-new",QIcon(":/icons/tab-new")));
    ui->actionOpen->setIcon(QIcon::fromTheme("document-open",QIcon(":/icons/document-open")));
    ui->actionPaste->setIcon(QIcon::fromTheme("edit-paste",QIcon(":/icons/edit-paste")));
    //ui->actionPhyxCalc
    ui->actionRecalculate_All->setIcon(QIcon::fromTheme("run-build",QIcon(":/icons/run-build")));
    ui->actionRecalculate_from_Line->setIcon(QIcon::fromTheme("run-build-cursor",QIcon(":/icons/run-build-cursor")));
    ui->actionRedo->setIcon(QIcon::fromTheme("edit-redo",QIcon(":/icons/edit-redo")));
    ui->actionSave->setIcon(QIcon::fromTheme("document-save",QIcon(":/icons/document-save")));
    ui->actionSave_All->setIcon(QIcon::fromTheme("document-save-all",QIcon(":/icons/document-save-all")));
    ui->actionSave_As->setIcon(QIcon::fromTheme("document-save-as",QIcon(":/icons/document-save-as")));
    ui->actionSettings->setIcon(QIcon::fromTheme("configure",QIcon(":/icons/configure")));
    ui->actionUndo->setIcon(QIcon::fromTheme("edit-undo",QIcon(":/icons/edit-undo")));
    //ui->actionVariables
    //ui->action_Slim_Mode
    ui->action_Plot_Window->setIcon(QIcon(":/icons/plot-icon"));
    configureButton->setIcon(QIcon(":/appicon/icon"));//QIcon::fromTheme("configure",QIcon(":/icons/configure")));

    //initialize Main Toolbar
    configureAction = ui->mainToolBar->addWidget(configureButton);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionNew);
    ui->mainToolBar->addAction(ui->actionOpen);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->actionSave);
    ui->mainToolBar->addAction(ui->actionSave_As);
    ui->mainToolBar->addSeparator();
    //ui->mainToolBar->addAction(ui->actionUndo);
    //ui->mainToolBar->addAction(ui->actionRedo);
    //ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(ui->action_Plot_Window);
    ui->mainToolBar->addAction(ui->actionRecalculate_All);
    ui->mainToolBar->addAction(ui->actionRecalculate_from_Line);
    ui->mainToolBar->addAction(ui->actionClear_Variables);
    ui->mainToolBar->addSeparator();
#ifndef MOBILE_VERSION
    ui->mainToolBar->addAction(ui->actionExport);
    ui->mainToolBar->addSeparator();
#endif
    ui->mainToolBar->addAction(ui->actionClose);

    //intialize plot window
    plotWindow = new PlotWindow(this);
    connect(plotWindow, &QWidget::visibilityChanged,
            ui->action_Plot_Window, &QAction::setChecked);

    //initialize plot action
    connect(ui->actionPlot, &QAction::triggered,
            this, &MainWindow::showPlotDialog);
}

void MainWindow::switchLayout(int number)
{
    if (number == 0)    //normal layout
    {
        ui->mainToolBar->setIconSize(QSize(24,24));
        ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui->menuBar->setVisible(true);
        configureAction->setVisible(false);
    }
    else                //slim layout
    {
         ui->mainToolBar->setIconSize(QSize(16,16));
         ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
         ui->menuBar->setVisible(false);
         configureAction->setVisible(true);
    }
}

void MainWindow::addNewTab()
{
    QWidget *newTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();

    activeTab = documentList.size();

    Document *newDocument = new Document;
    newDocument->expressionEdit = new QTextEdit(newTab);
    newDocument->lineParser = new LineParser(this);
    newDocument->lineParser->setUnitLoader(unitLoader);
    newDocument->lineParser->setVariableTable(ui->variableTable);
    newDocument->lineParser->setConstantsTable(ui->constantsTable);
    newDocument->lineParser->setUnitsTable(ui->unitsTable);
    newDocument->lineParser->setPrefixesTable(ui->prefixesTable);
    newDocument->lineParser->setFunctionsList(ui->functionsList);
    newDocument->lineParser->setCalculationEdit(newDocument->expressionEdit);
    newDocument->lineParser->setPlotWindow(plotWindow);
    newDocument->lineParser->setAppSettings(&appSettings);
    newDocument->lineParser->phyxCalculator()->loadFile(settingsDir + "/definitions.txt");
    newDocument->name = "";
    newDocument->path = "";
    documentList.append(newDocument);

    newDocument->expressionEdit->installEventFilter(this);
    connect(newDocument->expressionEdit->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::documentModified);
    connect(newDocument->lineParser, &LineParser::listWidgetUpdate,
            this, &MainWindow::loadListWidget);

    layout->addWidget(newDocument->expressionEdit);
    layout->setMargin(0);
    newTab->setLayout(layout);

    ui->tabWidget->addTab(newTab,QIcon(),"");
    ui->tabWidget->setCurrentIndex(activeTab);
    newDocument->lineParser->setLoading(false);
    syncDocumentTitle();

    //initialize custom context menu
    newDocument->expressionEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(newDocument->expressionEdit, &QWidget::customContextMenuRequested,
            this, &MainWindow::showContexMenu);
}

bool MainWindow::closeTab(int index)
{
    Document *document = documentList.at(index);

    //if saving fails abort
    if (!saveDocument(document, false, true))
        return false;

    //never close the last tab
    if (ui->tabWidget->count() != 1)
    {
        ui->tabWidget->removeTab(index);
        document->lineParser->deleteLater();
        document->expressionEdit->deleteLater();
        documentList.removeAt(index);;
        delete document;
    }
    else
    {
        document->name = tr("Untitled");
        document->path = "";
        document->expressionEdit->clear();
        document->lineParser->clearAllVariables();
        document->expressionEdit->document()->setModified(false);
    }

    emit tabChanged(activeTab);

    return true;
}

bool MainWindow::closeAllTabs()
{
    for (int i = documentList.size()-1; i > -1; i--)
    {
       if (!closeTab(i))
           return false;
    }

    return true;
}

void MainWindow::tabChanged(int index)
{
    activeTab = index;
    documentList.at(activeTab)->lineParser->updateSettings();
    syncDocumentTitle();
}

bool MainWindow::saveDocument(Document *document, bool force, bool exit)
{
    //check wheter the document is unchanged or not
    if (!(document->expressionEdit->document()->isModified() ||
            ((document->name == "") && !exit)
            || force))
        return true;

    //exit = if tab is closed
    if (exit)
    {
        //ask the user to save the document
        QMessageBox messageBox(this);
        messageBox.setText(tr("The document has been modified."));
        messageBox.setInformativeText(tr("Do you want to save your changes?"));
        messageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        messageBox.button(QMessageBox::Discard)->setText(tr("Do Not Save"));
        messageBox.button(QMessageBox::Discard)->setIcon(QIcon());
        messageBox.button(QMessageBox::Cancel)->setText(tr("Do Not Close"));
        messageBox.setDefaultButton(QMessageBox::Save);
        int returnCode = messageBox.exec();

        switch (returnCode)
        {
            case QMessageBox::Save:     break;
            case QMessageBox::Discard:  return true;
            case QMessageBox::Cancel:   return false;
        }
    }

    //ask for a filename
    if (document->name.isEmpty() || force)
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Document"), QDir::homePath() + "/" + document->name, tr("Text files (*.txt)(*.txt)"));

        if (fileName.isEmpty())
            return false;
        else
        {
            int pos = fileName.lastIndexOf("/");
            document->path = fileName.left(pos+1);
            document->name = fileName.mid(pos+1);
            document->expressionEdit->document()->setModified(true);
        }
    }

    //Save the file
    QString text = document->expressionEdit->toPlainText().trimmed();
    QFile file(document->path + document->name);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(text.toUtf8());
        file.close();

        document->expressionEdit->document()->setModified(false);
        syncDocumentTitle();

        addRecentDocument(file.fileName());
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("An Error occured, can't save file %1").arg(file.fileName()));
    }

    return true;
}

void MainWindow::openDocument(QString fileName, bool newTab)
{
    Document *document;

    if (newTab)
        addNewTab();

    document = documentList.at(activeTab);

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        QString text = QString::fromUtf8(file.readAll());
        document->expressionEdit->setText(text);
        file.close();

        int pos = fileName.lastIndexOf("/");
        document->path = fileName.left(pos+1);
        document->name = fileName.mid(pos+1);
        document->expressionEdit->document()->setModified(false);

        syncDocumentTitle();

        addRecentDocument(fileName);
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("Can't open file %1").arg(fileName));
    }
}

void MainWindow::autosaveDocument()
{
    QFile file(autosaveFilename);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(documentList.at(activeTab)->expressionEdit->toPlainText().toUtf8());
        file.close();
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("An Error occured, can't save file %1").arg(file.fileName()));
    }
}

void MainWindow::restoreDocument()
{
    if (QFile::exists(autosaveFilename))
        openDocument(autosaveFilename,false);
}

void MainWindow::deleteAutosaves()
{
    if (QFile::exists(autosaveFilename))
        QFile::remove(autosaveFilename);
}

void MainWindow::syncDocumentTitle()
{
    QString name;

    if (documentList.at(activeTab)->name == "")
        name = tr("Untitled");
    else
        name = documentList.at(activeTab)->name;

    QString tabText = name;
    QString windowText = name;

    if (documentList.at(activeTab)->expressionEdit->document()->isModified())
    {
        tabText.append("*");
        windowText.append(tr(" [modified]"));
    }

    windowText.append(tr(" - PhyxCalc"));

    ui->tabWidget->setTabText(activeTab, tabText);
    this->setWindowTitle(windowText);
}

void MainWindow::documentModified()
{
    syncDocumentTitle();
}

void MainWindow::dockButtonPressed(QAbstractButton *button)
{
    documentList.at(activeTab)->expressionEdit->insertPlainText(button->toolTip().split(":").at(1).trimmed());
    documentList.at(activeTab)->expressionEdit->setFocus();
}

void MainWindow::dockWidgetPressed(QListWidgetItem *item)
{
    documentList.at(activeTab)->expressionEdit->insertPlainText(item->toolTip().split(":").at(1).trimmed());
    documentList.at(activeTab)->expressionEdit->setFocus();
}

void MainWindow::showContexMenu(const QPoint &point)
{
    QMenu *menu = documentList.at(activeTab)->expressionEdit->createStandardContextMenu();
    menu->addSeparator();
    menu->addAction(ui->actionPlot);
    menu->exec(documentList.at(activeTab)->expressionEdit->mapToGlobal(point));
}

void MainWindow::showPlotDialog()
{
    PlotDialog plotDialog;
    plotDialog.setExpression(documentList.at(activeTab)->expressionEdit->textCursor().selectedText());
    if (plotDialog.exec() == QDialog::Accepted)
        documentList.at(activeTab)->lineParser->appendLine(plotDialog.output());
}

void MainWindow::loadDock(const QString &name, const QStringList &items)
{
    QDockWidget *dockWidget = new QDockWidget(name, this);
    QWidget *widget = new QWidget(dockWidget);
    QBoxLayout *boxLayout = new QBoxLayout(QBoxLayout::TopToBottom, widget);
    boxLayout->setMargin(0);
    QAction *menuAction = new QAction(ui->menuDocks);

    QListWidget *listWidget = new QListWidget(widget);
    listWidget->setViewMode(QListView::IconMode);
    //listWidget->setIconSize(QSize(16,16));
    //listWidget->setSpacing(5);
    listWidget->setResizeMode(QListView::Adjust);

    //QFont font = listWidget->font();
    //font.setPointSize(12);
    //listWidget->setFont(font);

    dockWidget->setObjectName(name);
    this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

    loadListWidget(listWidget, items);  //load the itemlist

    //put the dock togheter
    dockWidget->setWidget(widget);
    widget->setLayout(boxLayout);
    boxLayout->addWidget(listWidget);

    //add an menu entry for the dock
    menuAction->setCheckable(true);
    menuAction->setText(name);
    ui->menuDocks->addAction(menuAction);

    //connect all sorts of signals and slots
    connect(listWidget, &QListWidget::itemClicked,
            this, &MainWindow::dockWidgetPressed);
    connect(menuAction, &QAction::toggled,
            dockWidget, &QDockWidget::setVisible);

    connect(dockWidget, &QDockWidget::visibilityChanged,
            menuAction, &QAction::setChecked);
}

void MainWindow::loadListWidget(QListWidget *listWidget, const QStringList &items)
{
    if (listWidget->count() != 0)
        listWidget->clear();

    int maximumWidth = 0;
    int currentWidth;

    for (int i = 0; i < items.size(); i++)
    {
        QString widgetName, toolTip, function;
        QStringList list;

        if (items.at(i).isEmpty())
            continue;

        //split the item and read name and tooltip
        list = items.at(i).split(";");
        if (list.size() == 2)       //for lists with size 2 just copy the second item
            list.append(list.at(1));
        if (list.size() < 3)
            return;
        widgetName = list.at(1);
        toolTip = list.at(0);
        function = list.at(2);

        //create a new list widget item
        QListWidgetItem *item = new QListWidgetItem(widgetName);
        item->setToolTip(toolTip + ": " + function);
        listWidget->addItem(item);

        //save the maximum width
        QFontMetrics fontMetrics = listWidget->fontMetrics();
        currentWidth = fontMetrics.width(widgetName + "M");
        if (currentWidth > maximumWidth)
            maximumWidth = currentWidth;
    }

    //set the minimum width
    for (int i = 0; i < listWidget->count(); i++)
    {
        listWidget->item(i)->setSizeHint(QSize(maximumWidth, 20));
    }
}

void MainWindow::loadAllDocks()
{
    //Load docks.txt
    QFile file(settingsDir + "docks.txt");
    if (file.open(QIODevice::ReadOnly))
    {
        QStringList dockNameList = QString::fromUtf8(file.readAll()).split("\n");

        for (int i = 0; i < dockNameList.size(); i++)
        {
            if (dockNameList.at(i).isEmpty())
                continue;

            QStringList list = dockNameList.at(i).split(";");

            QFile dockFile(settingsDir + list.at(1));
            if (dockFile.open(QIODevice::ReadOnly))
            {
                //Load the file and split it into string list
                loadDock(list.at(0), QString::fromUtf8(dockFile.readAll()).split("\n"));

                dockFile.close();
            }
        }

        file.close();
        return;
    }
    else
        return;
}

void MainWindow::updateRecentDocuments()
{
    //remove old actions
    for (int i = ui->menuOpen_Recent->actions().size()-1; i >= 0 ; i--)
    {
        QAction *action = ui->menuOpen_Recent->actions().at(i);
        ui->menuOpen_Recent->removeAction(action);
        action->deleteLater();
    }

    //and add new
    for (int i = recentDocuments.size()-1; i >= 0 ; i--)
    {
        QAction *recentDocumentAction = new QAction(this);
        QString fileName = recentDocuments.at(i).mid(recentDocuments.at(i).lastIndexOf('/')+1);
        recentDocumentAction->setText(tr("%1 [%2]").arg(fileName).arg(recentDocuments.at(i)));
        ui->menuOpen_Recent->addAction(recentDocumentAction);

        connect(recentDocumentAction, &QAction::triggered,
                this, &MainWindow::openRecentDocument);
    }
}

void MainWindow::addRecentDocument(QString name)
{
    if (!recentDocuments.contains(name))
        recentDocuments.append(name);
    else
    {
        recentDocuments.move(recentDocuments.indexOf(name), recentDocuments.size()-1);
    }

    if (recentDocuments.size() > 10)
        recentDocuments.removeFirst();

    updateRecentDocuments();
}

void MainWindow::openRecentDocument()
{
    QAction *action = (QAction*)QObject::sender();
    QString fileName = action->text().mid(action->text().indexOf('[')+1);
    fileName.chop(1);

    openDocument(fileName, true);
}

void MainWindow::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5) || defined(Q_OS_ANDROID)
    showMaximized();
#else
    show();
#endif
}

void MainWindow::setOrientation(MainWindow::ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#elif QT_VERSION < 0x050001
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#else
    default:
    return;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}


void MainWindow::on_actionNew_Tab_triggered()
{
    addNewTab();
}

void MainWindow::on_actionExport_triggered()
{
    ExportDialog *exportDialog = new ExportDialog(this);
    exportDialog->setText(documentList.at(activeTab)->lineParser->exportFormelEditor());
    exportDialog->show();
}

void MainWindow::on_actionSave_triggered()
{
    saveDocument(documentList.at(activeTab));
}

void MainWindow::on_actionSave_As_triggered()
{
    saveDocument(documentList.at(activeTab), true);
}

void MainWindow::on_actionSave_All_triggered()
{
    for (int i = 0; i < documentList.size(); i++)
    {
        saveDocument(documentList.at(i));
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath(), tr("Text files (*.txt)(*.txt)"));
    if (!fileName.isEmpty())
    {
        if (documentList.at(activeTab)->expressionEdit->document()->isModified())
            openDocument(fileName,true);
        else
            openDocument(fileName, false);
    }
}

void MainWindow::on_actionClose_triggered()
{
    closeTab(activeTab);
}

void MainWindow::on_actionClose_Other_triggered()
{
    for (int i = documentList.size()-1; i > -1; i--)
    {
        if (i != activeTab)
            closeTab(i);
    }
}

void MainWindow::on_actionUndo_triggered()
{
    documentList.at(activeTab)->expressionEdit->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    documentList.at(activeTab)->expressionEdit->redo();
}

void MainWindow::on_actionCut_triggered()
{
    documentList.at(activeTab)->expressionEdit->cut();
}

void MainWindow::on_actionCopy_triggered()
{
    documentList.at(activeTab)->expressionEdit->copy();
}

void MainWindow::on_actionPaste_triggered()
{
    documentList.at(activeTab)->expressionEdit->paste();
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog settingsDialog;
    settingsDialog.setSettingsDir(settingsDir);
    settingsDialog.setAppSettings(&appSettings);
    settingsDialog.exec();
    settingsDialog.getAppSettings(&appSettings);

    for (int i = 0; i < documentList.size(); i++)
        documentList.at(i)->lineParser->updateSettings();
}

void MainWindow::on_actionAbout_triggered()
{
    QString version(VERSION_REV);
    version.remove("$Rev: ");
    version.remove(" $");
    version.prepend(VERSION_MAJOR);
    QString qtVersion(QT_VERSION_STR);
    QString qwtVersion(QWT_VERSION_STR);
    QMessageBox::about(this, tr("About PhyxCalc"),
                       tr("<h2>PhyxCalc %1</h2>"
                          "Based on Qt %3 and Qwt %4 <br><br>"
                          "Built on %2 <br><br>"
                          "Copyright 2011 by Christian Schirm and Alexander R&ouml;ssler"
                          "<br><br>"
                          "PhyxCalc is free software: you can redistribute it and/or modify<br>"
                          "it under the terms of the GNU General Public License as published by<br>"
                          "the Free Software Foundation, either version 3 of the License, or<br>"
                          "(at your option) any later version.<br>"
                          "<br>"
                          "PhyxCalc is distributed in the hope that it will be useful,<br>"
                          "but WITHOUT ANY WARRANTY; without even the implied warranty of<br>"
                          "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the<br>"
                          "GNU General Public License for more details.<br>"
                          "<br>"
                          "You should have received a copy of the GNU General Public License<br>"
                          "along with PhyxCalc.  If not, see <a href='http://www.gnu.org/licenses'>http://www.gnu.org/licenses</a>"
                          "<br><br>"
                          "For questions, improvements or bugs visit: <a href='https://sourceforge.net/p/phyxcalc/'>https://sourceforge.net/p/phyxcalc/</a>")
                       .arg(version, LAST_CHANGE)
                       .arg(qtVersion)
                       .arg(qwtVersion));
}

void MainWindow::on_actionRecalculate_All_triggered()
{
    documentList.at(activeTab)->lineParser->parseAll();
}

void MainWindow::on_actionRecalculate_from_Line_triggered()
{
    documentList.at(activeTab)->lineParser->parseFromCurrentPosition();
}

void MainWindow::on_action_Slim_Mode_triggered()
{
    switchLayout(ui->action_Slim_Mode->isChecked());
}

void MainWindow::on_actionClear_Variables_triggered()
{
    documentList.at(activeTab)->lineParser->clearAllVariables();
}

void MainWindow::on_actionHelp_triggered()
{
    HelpDialog helpDialog;
    helpDialog.setHelpFile(":/doc/doc_en.html");
    helpDialog.exec();
}

void MainWindow::on_action_Plot_Window_triggered(bool checked)
{
    if (checked)
        plotWindow->show();
    else
        plotWindow->hide();
}
