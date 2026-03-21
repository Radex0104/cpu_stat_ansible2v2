#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "configmanager.h"
#include "ansiblerunner.h"
#include "windowgraphics.h"
#include "wslchecker.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    void showEvent(QShowEvent *event) override;
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
    void onWslCheckCompleted(const WSLChecker::WSLInfo &info);
    void onWslCheckError(const QString &error);
    void onWslSetupFinished(bool success);

    void onPythonFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onPythonError(QProcess::ProcessError error);
    void onPythonOutput();
    void onPythonErrorOutput();
    
private:
    void setupConnections();
    void loadSavedConfiguration();
    void setArchivePath(const QString& path);
    void updatePlayButtonState();
    void checkWSLAndShowStatus();
    void showMessage(const QString &message, bool isError = false);
    bool wslCheckPerformed = false;
    Ui::MainWindow *ui;
    WindowGraphics *graphics;
    ConfigManager *configManager;
    AnsibleRunner *ansibleRunner;
    WSLChecker *checker;
    QString currentFilePath;
    QList<HostConfig> hostsConfig;
    QString playbookPath;
    QString currentArchivePath;
    // Для работы с Python скриптом
    QProcess *pythonProcess;
    QString pythonScriptPath;

    // Методы для работы с данными из Prometheus
    void fetchPrometheusData();

signals:
    void fileDropped(const QFileInfo &fileInfo);
};

#endif // MAINWINDOW_H
