#include "windowgraphics.h"
#include <QDragEnterEvent>
#include <QMimeData>

// ==================== КОНСТРУКТОР ====================
WindowGraphics::WindowGraphics(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setAcceptDrops(true);
}

// ==================== ИНИЦИАЛИЗАЦИЯ ИНТЕРФЕЙСА ====================
void WindowGraphics::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ----- СЕКЦИЯ DRAG & DROP -----
    QLabel *dragDropLabel = new QLabel("Перетащите файл скрипта сюда");
    dragDropLabel->setAlignment(Qt::AlignCenter);
    dragDropLabel->setStyleSheet("QLabel { border: 2px dashed #aaa; padding: 20px; background-color: #f8f8f8; }");
    dragDropLabel->setMinimumHeight(100);
    mainLayout->addWidget(dragDropLabel);

    // ----- СЕКЦИЯ ОТОБРАЖЕНИЯ ПУТИ К ФАЙЛУ -----
    filePathLabel = new QLabel("Файл не выбран");
    filePathLabel->setStyleSheet("QLabel { color: #666; font-size: 10pt; }");
    mainLayout->addWidget(filePathLabel);

    // ----- СЕКЦИЯ ВВОДА SSH ПОЛЬЗОВАТЕЛЯ -----
//    sshUserEdit = new QLineEdit();
//    sshUserEdit->setPlaceholderText("ubuntu1 (пользователь на сервере)");
//    mainLayout->addWidget(sshUserEdit);

    // ----- СЕКЦИЯ НАСТРОЙКИ ХОСТОВ -----
    QGroupBox *hostsGroup = new QGroupBox("Настройка хостов");
    QVBoxLayout *hostsLayout = new QVBoxLayout(hostsGroup);

    // ---- ПАНЕЛЬ УПРАВЛЕНИЯ ХОСТАМИ ----
    QHBoxLayout *hostsControlLayout = new QHBoxLayout();
    newHostEdit = new QLineEdit();
    sshUserEdit = new QLineEdit();
    sshPasswordEdit = new QLineEdit();
    statusBar = new QStatusBar();
    sshUserEdit->setPlaceholderText("ubuntu1 (пользователь на сервере)");
    newHostEdit->setPlaceholderText("Введите адрес хоста (IP или домен)");
    sshPasswordEdit->setPlaceholderText("Введите пароль для хоста");
    addHostButton = new QPushButton("Добавить");
    removeHostButton = new QPushButton("Удалить");

    hostsControlLayout->addWidget(sshUserEdit);
    hostsControlLayout->addWidget(newHostEdit);
    hostsControlLayout->addWidget(sshPasswordEdit);
    hostsControlLayout->addWidget(addHostButton);
    hostsControlLayout->addWidget(removeHostButton);

    // ---- СПИСОК ХОСТОВ ----
    hostsListWidget = new QListWidget();
    hostsLayout->addLayout(hostsControlLayout);
    hostsLayout->addWidget(hostsListWidget);
    mainLayout->addWidget(hostsGroup);

    // ----- СЕКЦИЯ КНОПКИ ЗАПУСКА -----
    playButton = new QPushButton("Play");
    playButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 10px; font-size: 14pt; }");
    mainLayout->addWidget(playButton);

    // ----- СЕКЦИЯ ВЫВОДА ANSIBLE -----
    QGroupBox *outputGroup = new QGroupBox("Вывод Ansible");
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);

    outputTextEdit = new QTextEdit();
    outputTextEdit->setReadOnly(true);
    outputTextEdit->setMinimumHeight(200);

    outputLayout->addWidget(outputTextEdit);
    mainLayout->addWidget(outputGroup);
    mainLayout->addWidget(statusBar);    

}

// ==================== МЕТОДЫ ОБНОВЛЕНИЯ ИНТЕРФЕЙСА ====================

// ----- ОБНОВЛЕНИЕ ЛЕЙБЛА ПУТИ К ФАЙЛУ -----
void WindowGraphics::updateFilePathLabel(const QString& text, bool success)
{
    filePathLabel->setText(text);
    filePathLabel->setStyleSheet(success ?
        "QLabel { color: green; font-size: 10pt; }" :
        "QLabel { color: red; font-size: 10pt; }");
}

// ----- ДОБАВЛЕНИЕ ТЕКСТА В ОБЛАСТЬ ВЫВОДА -----
void WindowGraphics::appendOutput(const QString& text)
{
    outputTextEdit->append(text);
}

void WindowGraphics::appendStatusBar(const QString& text)
{
    statusBar->showMessage(text);
}

// ----- ОЧИСТКА ОБЛАСТИ ВЫВОДА -----
void WindowGraphics::clearOutput()
{
    outputTextEdit->clear();
}

// ----- ДОБАВЛЕНИЕ ХОСТА В СПИСОК -----
void WindowGraphics::addHostToList(const QString& hostInfo)
{
    hostsListWidget->addItem(hostInfo);
}

// ----- УДАЛЕНИЕ ХОСТА ИЗ СПИСКА -----
void WindowGraphics::removeHostFromList(int row)
{
    delete hostsListWidget->takeItem(row);
}

