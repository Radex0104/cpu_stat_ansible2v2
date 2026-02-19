#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "configmanager.h"
#include "ansiblerunner.h"
#include "windowgraphics.h"
#include <QDragEnterEvent>
#include <QDropEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void onAddHostClicked();
    void removeHost();
    void onPlayButtonClicked();
    void onAnsibleOutput(const QString& text);
    void onAnsibleFinished(bool success, int exitCode);
    void onAnsibleError(const QString& message);

private:
    void setupConnections();
    void loadSavedConfiguration();
    void updatePlayButtonState();
    void showMessage(const QString &message, bool isError = false);

    Ui::MainWindow *ui;
    WindowGraphics *graphics;  // Вместо отдельных виджетов
    ConfigManager *configManager;
    AnsibleRunner *ansibleRunner;

    QString currentFilePath;
    QList<HostConfig> hostsConfig;
    QString playbookPath;
};

#endif // MAINWINDOW_H
