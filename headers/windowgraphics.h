#ifndef WINDOWGRAPHICS_H
#define WINDOWGRAPHICS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QTextEdit>
#include <QGroupBox>
#include <QStatusBar>
#include <QProgressBar>
#include "progressmanager.h"
class WindowGraphics : public QWidget
{
    Q_OBJECT

public:
    explicit WindowGraphics(QWidget *parent = nullptr);

    // Геттеры для доступа к элементам интерфейса
    QLabel* getFilePathLabel() const { return filePathLabel; }
    QLineEdit* getNewHostEdit() const { return newHostEdit; }
    QLineEdit* getSshUserEdit() const { return sshUserEdit; }
    QLineEdit* getSshPasswordEdit() const { return sshPasswordEdit; }
    QPushButton* getAddHostButton() const { return addHostButton; }
    QPushButton* getRemoveHostButton() const { return removeHostButton; }
    QPushButton* getPlayButton() const { return playButton; }
    QListWidget* getHostsListWidget() const { return hostsListWidget; }
    QTextEdit* getOutputTextEdit() const { return outputTextEdit; }
    QProgressBar* getProgressBar() const { return progressBar; } // Новый геттер

    // Методы обновления интерфейса
    void updateFilePathLabel(const QString& text, bool success);
    void appendOutput(const QString& text);
    void appendStatusBar(const QString& text);
    void clearOutput();
    void addHostToList(const QString& hostInfo);
    void removeHostFromList(int row);
    ProgressManager* getProgressManager() const { return progressManager; }
// protected:
//     void dragEnterEvent(QDragEnterEvent *event) override;
//     void dropEvent(QDropEvent *event) override;

private:
    void setupUI();

    // Элементы интерфейса
    QLabel *filePathLabel;
    QLineEdit *newHostEdit;
    QLineEdit *sshUserEdit;
    QLineEdit *sshPasswordEdit;
    QPushButton *addHostButton;
    QPushButton *removeHostButton;
    QPushButton *playButton;
    QListWidget *hostsListWidget;
    QTextEdit *outputTextEdit;
    QStatusBar *statusBar;
    QProgressBar *progressBar; // Новый элемент
    ProgressManager *progressManager;
};

#endif // WINDOWGRAPHICS_H