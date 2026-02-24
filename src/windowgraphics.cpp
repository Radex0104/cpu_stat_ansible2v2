#include "windowgraphics.h"
#include <QDragEnterEvent>
#include <QMimeData>

WindowGraphics::WindowGraphics(QWidget *parent)
    : QWidget(parent)
    , progressManager(new ProgressManager(this))
{
    setupUI();
    setAcceptDrops(true);

    progressManager->setProgressBar(progressBar);
}

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
    sshPasswordEdit->setEchoMode(QLineEdit::Password);
    
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

    // ----- СЕКЦИЯ ПРОГРЕСС-БАРА -----
    QGroupBox *progressGroup = new QGroupBox("Прогресс выполнения");
    QVBoxLayout *progressLayout = new QVBoxLayout(progressGroup);
    
    progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setVisible(false); // Скрыт по умолчанию
    progressBar->setStyleSheet(
        "QProgressBar {"
        "    border: 1px solid #bbb;"
        "    border-radius: 5px;"
        "    text-align: center;"
        "    height: 25px;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #4CAF50;"
        "    border-radius: 5px;"
        "}"
    );
    
    progressLayout->addWidget(progressBar);
    mainLayout->addWidget(progressGroup);

    // ----- СЕКЦИЯ КНОПКИ ЗАПУСКА -----
    playButton = new QPushButton("Play");
    playButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    padding: 10px;"
        "    font-size: 14pt;"
        "    border: none;"
        "    border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #cccccc;"
        "}"
    );
    mainLayout->addWidget(playButton);

    // ----- СЕКЦИЯ ВЫВОДА ANSIBLE -----
    QGroupBox *outputGroup = new QGroupBox("Вывод Ansible");
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);

    outputTextEdit = new QTextEdit();
    outputTextEdit->setReadOnly(true);
    outputTextEdit->setMinimumHeight(200);
    outputTextEdit->setFontFamily("Courier New");

    outputLayout->addWidget(outputTextEdit);
    mainLayout->addWidget(outputGroup);
    mainLayout->addWidget(statusBar);
}

// Остальные методы без изменений
void WindowGraphics::updateFilePathLabel(const QString& text, bool success)
{
    filePathLabel->setText(text);
    filePathLabel->setStyleSheet(success ?
        "QLabel { color: green; font-size: 10pt; }" :
        "QLabel { color: red; font-size: 10pt; }");
}

void WindowGraphics::appendOutput(const QString& text)
{
    outputTextEdit->append(text);
}

void WindowGraphics::appendStatusBar(const QString& text)
{
    statusBar->showMessage(text);
}

void WindowGraphics::clearOutput()
{
    outputTextEdit->clear();
}

void WindowGraphics::addHostToList(const QString& hostInfo)
{
    hostsListWidget->addItem(hostInfo);
}

void WindowGraphics::removeHostFromList(int row)
{
    delete hostsListWidget->takeItem(row);
}

// void WindowGraphics::dragEnterEvent(QDragEnterEvent *event)
// {
//     if (event->mimeData()->hasUrls()) {
//         event->acceptProposedAction();
//     }
// }

// void WindowGraphics::dropEvent(QDropEvent *event)
// {
//     // Реализация dropEvent (можно оставить пустой или перенести из MainWindow)
//     event->acceptProposedAction();
// }