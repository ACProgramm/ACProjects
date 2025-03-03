#include "widget.h"
#include "./ui_widget.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    baseConnection();

    setEditRules();
}

void Widget::setEditRules() {
    ui->loginEdit->setMaxLength(20);
    ui->passwordEdit->setMaxLength(20);
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
}

void Widget::baseConnection() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("qtBase");
    db.setUserName("postgres");
    db.setPassword("Cheporesta22");

    if (!db.open()) {
        qDebug() << "Ошибка подключения: " << db.lastError().text();

    } else {
        qDebug() << "Подключение успешно!";
    }

    QSqlQuery query;
    QString createTableQuery =
        "CREATE TABLE IF NOT EXISTS users ("
        "id SERIAL PRIMARY KEY, "
        "username VARCHAR(50) UNIQUE NOT NULL, "
        "password VARCHAR(50) NOT NULL"
        ");";

    if (!query.exec(createTableQuery)) {
        qDebug() << "Ошибка создания таблицы:" << query.lastError().text();
    } else {
        qDebug() << "Таблица 'users' создана или уже существует.";
    }

    QString insertUsersQuery =
        "INSERT INTO users (username, password) VALUES "
        "('user1', 'password1'), "
        "('user3', 'password2'), "
        "('user3', 'password3') "
        "ON CONFLICT (username) DO NOTHING;"; // Чтобы не дублировались

    if (!query.exec(insertUsersQuery)) {
        qDebug() << "Ошибка добавления пользователей:" << query.lastError().text();
    } else {
        qDebug() << "Пользователи добавлены (если их не было).";
    }

}

void Widget::on_loginButton_clicked()
{
    QString username = ui->loginEdit->text().trimmed();
    QString password = ui->passwordEdit->text().trimmed();

    // Проверяем, что поля не пустые
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите логин и пароль!");
        return;
    }

    // Проверяем логин и пароль в базе
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (query.exec() && query.next()) {
        QMessageBox::information(this, "Успех", "Вы успешно вошли!");
        // Здесь можно открыть другое окно или скрыть текущее
    } else {
        QMessageBox::critical(this, "Ошибка", "Неверный логин или пароль!");
    }
}

Widget::~Widget()
{
    delete ui;
}

