#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMap>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    // --- Инициализация меню ---
    void setupFileMenu();
    void setupEditMenu();
    void setupAboutMenu();

    // --- Управление вкладками ---
    void addNewTab();
    void deleteCurrentTab();
    void onTabClicked(int index);

    // --- Файловые операции ---
    void openFile();
    void saveFile();
    void saveFileAs();
    void printDocument();
    void exportToPDF();
    void printPreview();

    // --- Редактирование текста ---
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void changeFont();
    void changeColor();

    // --- Интерфейс и статусбар ---
    void updateCursorPosition();
    void showInformation();

    // --- Управление окном ---
    void on_btnClose_clicked();
    void on_btnMaximize_clicked();
    void on_btnMinimize_clicked();

private:
    Ui::MainWindow *ui;
    QMap<QWidget*, QString> filePaths; // Сопоставление вкладок с их путями
    QLabel *statusLabel;
    QPoint dragPosition; // Для перемещения окна
};

#endif // MAINWINDOW_H
