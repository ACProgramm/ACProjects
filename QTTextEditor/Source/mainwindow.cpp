#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPushButton>
#include <QMenu>
#include <QKeySequence>
#include <QAction>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QKeyEvent>
#include <QShortcut>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QScrollArea>
#include <QApplication>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QIcon>
#include <QStyle>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowIcon(QIcon(":/images/notes.png"));

    // --- Инициализация статусбара ---
    statusLabel = new QLabel("Line: 1/1, Column: 1", this);
    statusLabel->setContentsMargins(9, 0, 0, 0);
    ui->statusbar->addWidget(statusLabel);


    // --- Установка иконки заголовка ---
    QPixmap pixmap(":/images/notes.png");
    QLabel *iconLabel = findChild<QLabel*>("iconLabel");
    if (iconLabel) {
        iconLabel->setPixmap(pixmap.scaled(17, 17, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // --- Подключение курсора первой вкладки ---
    QWidget *firstTab = ui->tabWidget->widget(0);
    if (firstTab) {
        QTextEdit *firstTextEdit = firstTab->findChild<QTextEdit *>();
        if (firstTextEdit) {
            connect(firstTextEdit, &QTextEdit::cursorPositionChanged, this, &MainWindow::updateCursorPosition);
        }
    }

    // --- Вызов функций создания menubar ---
    setupFileMenu();
    setupEditMenu();
    setupAboutMenu();

    // --- Подключение горячих клавиш и вкладок ---
    this->installEventFilter(this); // Добавил обработчик событий клавиатуры
    QShortcut *deleteShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(deleteShortcut, &QShortcut::activated, this, &MainWindow::deleteCurrentTab);

    connect(ui->tabWidget, &QTabWidget::tabBarClicked, this, &MainWindow::onTabClicked);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabClicked);
}

// --- Код - растягивание обьектов по окну ---
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    int statusBarHeight = ui->statusbar->height(); // Получаем высоту статусбара

    ui->CustomTitleBar->setGeometry(0, 0, this->width(), 45);
    ui->widgetMenuBar->setGeometry(0, 46, this->width(), 40);

    // Исправляем высоту tabWidget, вычитая statusBarHeight
    int tabWidgetHeight = this->height() - 87 - statusBarHeight;
    ui->tabWidget->setGeometry(0, 87, this->width(), tabWidgetHeight);

}


// --- Создание меню "Файл" ---
void MainWindow::setupFileMenu() {

    QPushButton *fileButton = findChild<QPushButton*>("fileButton");
    if (fileButton) {
        QMenu *fileMenu = new QMenu(this);
        QPushButton *fileButton = findChild<QPushButton*>("fileButton");
        if (fileButton) {
            fileButton->setIcon(QIcon(":/images/folder.png"));  // <-- Добавил сюда
            QMenu *fileMenu = new QMenu(this);
            fileButton->setMenu(fileMenu);  // <-- Устанавливаем меню
        }


        QAction *newAction = new QAction(style()->standardIcon(QStyle::SP_FileIcon), "New", this);
        newAction->setShortcut(QKeySequence::New);
        fileMenu->addAction(newAction);
        connect(newAction, &QAction::triggered, this, &MainWindow::addNewTab);

        QAction *openAction = new QAction(style()->standardIcon(QStyle::SP_DirOpenIcon), "Open", this);
        openAction->setShortcut(QKeySequence::Open);
        fileMenu->addAction(openAction);
        connect(openAction, &QAction::triggered, this, &MainWindow::openFile);


        QAction *saveAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), "Save", this);
        saveAction->setShortcut(QKeySequence::Save);
        fileMenu->addAction(saveAction);
        connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

        QAction *saveAsAction = new QAction("Save As", this);
        saveAsAction->setIcon(QIcon(":/images/save.png"));
        fileMenu->addAction(saveAsAction);
        connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);

        fileMenu->addSeparator();

        QAction *printAction = new QAction("Print", this);
        printAction->setIcon(QIcon(":/images/printer.png"));
        printAction->setShortcut(QKeySequence::Print);
        fileMenu->addAction(printAction);
        connect(printAction, &QAction::triggered, this, &MainWindow::printDocument);

        QAction *printPreviewAction = new QAction(style()->standardIcon(QStyle::SP_FileDialogNewFolder), "Print Preview", this);
        fileMenu->addAction(printPreviewAction);
        connect(printPreviewAction, &QAction::triggered, this, &MainWindow::printPreview); // Связываем

        QAction *exportPdfAction = new QAction("Export PDF", this);
        exportPdfAction->setIcon(QIcon(":/images/pdff.png"));
        fileMenu->addAction(exportPdfAction);
        connect(exportPdfAction, &QAction::triggered, this, &MainWindow::exportToPDF);

        fileMenu->addSeparator();

        QAction *exitAction = new QAction("Exit", this);
        exitAction->setIcon(QIcon(":/images/exit-full-screen.png"));
        exitAction->setShortcut(QKeySequence::Quit);
        fileMenu->addAction(exitAction);
        connect(exitAction, &QAction::triggered, this, &MainWindow::close);

        fileButton->setMenu(fileMenu);

        connect(fileButton, &QPushButton::clicked, fileButton, [=]() {
            fileMenu->exec(fileButton->mapToGlobal(QPoint(0, fileButton->height())));
        });

        fileMenu->setStyleSheet("QMenu::item:selected { color: #FFFBE5; }");
    }

}

// --- Создание меню "Редактировать" ---
void MainWindow::setupEditMenu() {

    QPushButton *editButton = findChild<QPushButton*>("editButton");
    if (editButton) {
        QMenu *editMenu = new QMenu(this);
        QPushButton *editButton = findChild<QPushButton*>("editButton");
        if (editButton) {
            QMenu *editMenu = new QMenu(this);
            editButton->setIcon(QIcon(":/images/pencil.png")); // Устанавливаем иконку для кнопки
        }



        QAction *undoAction = new QAction(style()->standardIcon(QStyle::SP_ArrowBack), "Undo", this);
        undoAction->setShortcut(QKeySequence::Undo);
        editMenu->addAction(undoAction);
        connect(undoAction, &QAction::triggered, this, &MainWindow::undo);

        QAction *redoAction = new QAction(style()->standardIcon(QStyle::SP_ArrowForward), "Redo", this);
        redoAction->setShortcut(QKeySequence::Redo);
        editMenu->addAction(redoAction);
        connect(redoAction, &QAction::triggered, this, &MainWindow::redo);

        editMenu->addSeparator();

        QAction *cutAction = new QAction(style()->standardIcon(QStyle::SP_DialogCloseButton), "Cut", this);
        cutAction->setShortcut(QKeySequence::Cut);
        editMenu->addAction(cutAction);
        connect(cutAction, &QAction::triggered, this, &MainWindow::cut);

        QAction *copyAction = new QAction(style()->standardIcon(QStyle::SP_FileIcon), "Copy", this);
        copyAction->setShortcut(QKeySequence::Copy);
        editMenu->addAction(copyAction);
        connect(copyAction, &QAction::triggered, this, &MainWindow::copy);

        QAction *pasteAction = new QAction(style()->standardIcon(QStyle::SP_DialogOkButton), "Paste", this);
        pasteAction->setShortcut(QKeySequence::Paste);
        editMenu->addAction(pasteAction);
        connect(pasteAction, &QAction::triggered, this, &MainWindow::paste);

        QAction *selectAllAction = new QAction(style()->standardIcon(QStyle::SP_FileDialogListView), "Select All", this);
        selectAllAction->setShortcut(QKeySequence::SelectAll);
        editMenu->addAction(selectAllAction);
        connect(selectAllAction, &QAction::triggered, this, &MainWindow::selectAll);

        editMenu->addSeparator();

        QAction *fontAction = new QAction(style()->standardIcon(QStyle::SP_FileDialogDetailedView), "Font", this);
        editMenu->addAction(fontAction);
        connect(fontAction, &QAction::triggered, this, &MainWindow::changeFont);

        QAction *colorAction = new QAction("Color", this);
        colorAction->setIcon(QIcon(":/images/color-wheel.png"));
        editMenu->addAction(colorAction);
        connect(colorAction, &QAction::triggered, this, &MainWindow::changeColor);

        editButton->setMenu(editMenu);

        connect(editButton, &QPushButton::clicked, editButton, [=]() {
            editMenu->exec(editButton->mapToGlobal(QPoint(0, editButton->height())));
        });

        editMenu->setStyleSheet("QMenu::item:selected { color: #FFFBE5; }");
    }

}

// --- Создание меню "О приложении" ---
void MainWindow::setupAboutMenu() {

    QPushButton *aboutButton = findChild<QPushButton*>("aboutButton");
    if (aboutButton) {
        QMenu *aboutMenu = new QMenu(this);
        QPushButton *aboutButton = findChild<QPushButton*>("aboutButton");
        if (aboutButton) {
            QMenu *aboutMenu = new QMenu(this);
            aboutButton->setIcon(QIcon(":/images/support.png")); // Устанавливаем иконку для кнопки
        }



        QAction *infoAction = new QAction("Info", this);
        infoAction->setIcon(QIcon(":/images/information.png"));
        aboutMenu->addAction(infoAction);
        connect(infoAction, &QAction::triggered, this, &MainWindow::showInformation);


        aboutMenu->setStyleSheet("QMenu::item:selected { color: #FFFBE5; }");

        aboutButton->setMenu(aboutMenu);

        connect(aboutButton, &QPushButton::clicked, aboutButton, [=]() {
            aboutMenu->exec(aboutButton->mapToGlobal(QPoint(0, aboutButton->height())));
        });
    }
}

// --- Движение окна мышью ---
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}


// --- Создание новой вкладки ---
void MainWindow::addNewTab()
{
    QWidget *newTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(newTab);
    layout->setContentsMargins(0, 0, 0, 0);  // Убираем отступы

    QTextEdit *newTextEdit = new QTextEdit(newTab);
    newTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // Растягиваем
    layout->addWidget(newTextEdit);

    newTab->setLayout(layout);
    newTab->setStyleSheet("background-color: #FFFEF7;");

    int index = ui->tabWidget->addTab(newTab, "New Tab");
    ui->tabWidget->setCurrentIndex(index);
    connect(newTextEdit, &QTextEdit::cursorPositionChanged, this, &MainWindow::updateCursorPosition);

}

// --- Удаление вкладки ---
void MainWindow::deleteCurrentTab()
{
    int index = ui->tabWidget->currentIndex();

    if (index != -1) {
        if (ui->tabWidget->count() > 1) { // Если вкладок больше одной, удаляем
            QWidget *tabToRemove = ui->tabWidget->widget(index);
            ui->tabWidget->removeTab(index);
            delete tabToRemove;
        } else { // Если вкладка последняя, просто очищаем текст
            QWidget *tab = ui->tabWidget->widget(index);
            if (tab) {
                QTextEdit *textEdit = tab->findChild<QTextEdit *>();
                if (textEdit) {
                    textEdit->clear(); // Очищаем содержимое
                }
            }
        }
    }
}

// --- Обработка клика по вкладке ---
void MainWindow::onTabClicked(int index)
{
    if (index != -1) {
        QWidget *tab = ui->tabWidget->widget(index);
        if (tab) {
            QTextEdit *textEdit = tab->findChild<QTextEdit *>();
            if (textEdit) {
                textEdit->clearFocus(); // Убираем фокус при ЛЮБОМ взаимодействии
            }
        }
    }
}

// --- Код открытие файла Open ---
void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) return; // Если пользователь отменил выбор

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }

    QTextStream in(&file);
    QString text = in.readAll();
    file.close();

    // Получаем только имя файла без пути
    QFileInfo fileInfo(fileName);
    QString shortName = fileInfo.fileName(); // Например: "example.txt"

    // Получаем текущую вкладку
    int index = ui->tabWidget->currentIndex();
    QWidget *tab = ui->tabWidget->widget(index);
    if (tab) {
        QTextEdit *textEdit = tab->findChild<QTextEdit *>();
        if (textEdit) {
            textEdit->setText(text); // Загружаем содержимое в `QTextEdit` текущей вкладки
        }
    }

    // Меняем заголовок вкладки на имя файла
    ui->tabWidget->setTabText(index, shortName);

    // Обновляем заголовок окна
    setWindowTitle(shortName);
}

// --- Код сохранения файла Save ---
void MainWindow::saveFile()
{
    int index = ui->tabWidget->currentIndex();
    if (index == -1) return;

    QWidget *tab = ui->tabWidget->widget(index);
    QTextEdit *textEdit = tab->findChild<QTextEdit *>();

    if (!textEdit) return;

    // Проверяем, есть ли у вкладки сохранённый путь
    if (!filePaths.contains(tab) || filePaths[tab].isEmpty()) {
        saveFileAs();
        return;
    }

    QFile file(filePaths[tab]);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file!");
        return;
    }

    QTextStream out(&file);
    out << textEdit->toPlainText();
    file.close();
}

// --- Код сохранения как  Save As ---
void MainWindow::saveFileAs()
{
    int index = ui->tabWidget->currentIndex();
    if (index == -1) return;

    QWidget *tab = ui->tabWidget->widget(index);
    QTextEdit *textEdit = tab->findChild<QTextEdit *>();

    if (!textEdit) return;

    QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) return;

    filePaths[tab] = fileName; // Сохраняем путь для вкладки

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file!");
        return;
    }

    QTextStream out(&file);
    out << textEdit->toPlainText();
    file.close();

    ui->tabWidget->setTabText(index, QFileInfo(fileName).fileName()); // Меняем имя вкладки
}

// --- Код печати файла Print ---
void MainWindow::printDocument() {
    int index = ui->tabWidget->currentIndex();
    if (index == -1) return;

    QWidget *tab = ui->tabWidget->widget(index);
    QTextEdit *textEdit = tab->findChild<QTextEdit *>();
    if (!textEdit) return;

    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        textEdit->print(&printer);
    }
}

// --- Код просмотра файла Print Preview ---
void MainWindow::printPreview()
{
    QPrinter printer;
    QPrintPreviewDialog previewDialog(&printer, this);

    connect(&previewDialog, &QPrintPreviewDialog::paintRequested, this, [&](QPrinter *printer) {
        int index = ui->tabWidget->currentIndex();  // Получаем индекс активной вкладки
        if (index == -1) return;  // Если вкладок нет, ничего не делаем

        QWidget *currentTab = ui->tabWidget->widget(index);
        QTextEdit *textEdit = currentTab->findChild<QTextEdit *>();  // Ищем QTextEdit внутри вкладки
        if (textEdit)
            textEdit->print(printer);
    });

    previewDialog.exec();
}

// --- Код экспорта файла Export PDF ---
void MainWindow::exportToPDF() {
    int index = ui->tabWidget->currentIndex();
    if (index == -1) return;

    QWidget *tab = ui->tabWidget->widget(index);
    QTextEdit *textEdit = tab->findChild<QTextEdit *>();
    if (!textEdit) return;

    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", "", "PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    textEdit->document()->print(&printer);
}


// --- Код Undo ---
void MainWindow::undo()
{
    if (ui->tabWidget->currentWidget()) {
        QTextEdit *textEdit = ui->tabWidget->currentWidget()->findChild<QTextEdit *>();
        if (textEdit) {
            textEdit->undo();
        }
    }
}

// --- Код Redo ---
void MainWindow::redo()
{
    if (ui->tabWidget->currentWidget()) {
        QTextEdit *textEdit = ui->tabWidget->currentWidget()->findChild<QTextEdit *>();
        if (textEdit) {
            textEdit->redo();
        }
    }
}

// --- Код Cut ---
void MainWindow::cut()
{
    if (ui->tabWidget->currentWidget()) {
        QTextEdit *textEdit = ui->tabWidget->currentWidget()->findChild<QTextEdit *>();
        if (textEdit) {
            textEdit->cut();
        }
    }
}

// --- Код Copy ---
void MainWindow::copy()
{
    if (ui->tabWidget->currentWidget()) {
        QTextEdit *textEdit = ui->tabWidget->currentWidget()->findChild<QTextEdit *>();
        if (textEdit) {
            textEdit->copy();
        }
    }
}

// --- Код Paste ---
void MainWindow::paste()
{
    if (ui->tabWidget->currentWidget()) {
        QTextEdit *textEdit = ui->tabWidget->currentWidget()->findChild<QTextEdit *>();
        if (textEdit) {
            textEdit->paste();
        }
    }
}

// --- Код Select All ---
void MainWindow::selectAll()
{
    if (ui->tabWidget->currentWidget()) {
        QTextEdit *textEdit = ui->tabWidget->currentWidget()->findChild<QTextEdit *>();
        if (textEdit) {
            textEdit->selectAll();
        }
    }
}

// --- Код смены фона ---
void MainWindow::changeFont()
{
    if (ui->tabWidget->currentWidget()) {
        QTextEdit *textEdit = ui->tabWidget->currentWidget()->findChild<QTextEdit *>();
        if (textEdit) {
            bool ok;
            QFont font = QFontDialog::getFont(&ok, textEdit->font(), this);
            if (ok) {
                textEdit->setFont(font);
            }
        }
    }
}

// --- Код смены цвета ---
void MainWindow::changeColor()
{
    if (ui->tabWidget->currentWidget()) {
        QTextEdit *textEdit = ui->tabWidget->currentWidget()->findChild<QTextEdit *>();
        if (textEdit) {
            QColor color = QColorDialog::getColor(textEdit->textColor(), this);
            if (color.isValid()) {
                textEdit->setTextColor(color);
            }
        }
    }
}

// --- Код реализации подсчета строк и колонок для statusBar ---
void MainWindow::updateCursorPosition() {
    QWidget *currentTab = ui->tabWidget->currentWidget();
    if (!currentTab) return;

    QTextEdit *textEdit = currentTab->findChild<QTextEdit *>();
    if (!textEdit) return;

    QTextCursor cursor = textEdit->textCursor();

    int line = cursor.blockNumber() + 1; // Строка
    int totalLines = textEdit->document()->blockCount(); // Всего строк
    int column = cursor.positionInBlock() + 1; // Колонка

    qDebug() << "Updating cursor position: Line" << line << "/" << totalLines << "Column" << column;

    statusLabel->setText(QString("Line: %1/%2, Column: %3")
                             .arg(line)
                             .arg(totalLines)
                             .arg(column));
}

// --- Реализация о приложении QMessageBox ---
void MainWindow::showInformation() {

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("About Scriptorio");

    QPixmap iconPixmap(":/images/notes.png");
    if (!iconPixmap.isNull()) { // Проверяем, загружена ли иконка
        msgBox.setIconPixmap(iconPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // Устанавливаем текст
    msgBox.setText("Scriptorio is a modern and minimalist notepad designed for effortless text editing and document management.\n\n"
                   "Featuring tabbed navigation, customizable fonts, and a sleek user interface, it allows for a seamless writing experience. "
                   "You can create, edit, save, print, and export your documents with ease.\n\n"
                   "Enjoy smooth and efficient text editing with essential features like undo, redo, cut, copy, paste, and more.\n\n"
                   "© 2025 Scriptorio. All rights reserved.");

    msgBox.exec();
}

// --- Реализация кнопки увеличить на полный экран ---
void MainWindow::on_btnMaximize_clicked()
{
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

// --- Реализация кнопки свернуть приложение ---
void MainWindow::on_btnMinimize_clicked()
{
    showMinimized();
}

// --- Реализация кнопки закрыть приложение ---
void MainWindow::on_btnClose_clicked()
{
    close();
}

MainWindow::~MainWindow()
{
    delete ui;
}
