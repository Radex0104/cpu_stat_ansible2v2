#ifndef WINDOWGRAPHICS_H
#define WINDOWGRAPHICS_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>

class WindowGraphics : public QWidget
{
    Q_OBJECT
public:
    explicit WindowGraphics(QWidget *parent = nullptr);

    // Геттеры для доступа к элементам UI
    QLabel* getFilePathLabel() const { return filePathLabel; }
    QListWidget* getHostsListWidget() const { return hostsListWidget; }
    QTextEdit* getOutputTextEdit() const { return outputTextEdit; }
    QPushButton* getPlayButton() const { return playButton; }
    QLineEdit* getNewHostEdit() const { return newHostEdit; }
    QLineEdit* getSshUserEdit() const { return sshUserEdit; }
    QLineEdit* getSshPasswordEdit() const { return sshPasswordEdit; }
    QPushButton* getAddHostButton() const { return addHostButton; }
    QPushButton* getRemoveHostButton() const { return removeHostButton; }
    // Методы для обновления UI
    void updateFilePathLabel(const QString& text, bool success = true);
    void appendOutput(const QString& text);
    void appendStatusBar(const QString& text);
    void clearOutput();
    void addHostToList(const QString& hostInfo);
    void removeHostFromList(int row);
    void setPlayButtonEnabled(bool enabled);

private:
    void setupUI();
    QPushButton *addHostButton;    // Добавить
    QPushButton *removeHostButton;
    QLabel *filePathLabel;
    QListWidget *hostsListWidget;
    QTextEdit *outputTextEdit;
    QPushButton *playButton;
    QLineEdit *newHostEdit;
    QLineEdit *sshUserEdit;
    QLineEdit *sshPasswordEdit;
    QStatusBar *statusBar;
};

#endif // WINDOWGRAPHICS_H
