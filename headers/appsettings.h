#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QString>
#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTabWidget>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>

class AppSettings : public QObject
{
    Q_OBJECT
public:
    static AppSettings& instance();
    ~AppSettings();

    // Prometheus
    QString getPrometheusHost() const;
    void setPrometheusHost(const QString& host);
    QString getFullPrometheusUrl() const;

    // JMeter
    int getTestDuration() const;
    void setTestDuration(int seconds);
    
    QString getLocalArchivePath() const;
    void setLocalArchivePath(const QString& path);
    
    QString getRemoteTestDir() const;
    void setRemoteTestDir(const QString& path);
    
    bool isGenerateReportEnabled() const;
    void setGenerateReportEnabled(bool enabled);
    
    QString getResultsLocalDir() const;
    void setResultsLocalDir(const QString& path);
    
    QString getResultsRemoteDir() const;
    void setResultsRemoteDir(const QString& path);
    
    bool isCollectCpuMetrics() const;
    void setCollectCpuMetrics(bool enabled);
    
    bool isCollectMemoryMetrics() const;
    void setCollectMemoryMetrics(bool enabled);
    
    QString getJmeterHost() const;        // ТОЛЬКО ОДИН РАЗ
    void setJmeterHost(const QString& host);  // ТОЛЬКО ОДИН РАЗ

    // Ansible
    QString getAnsiblePlaybookPath() const;
    void setAnsiblePlaybookPath(const QString& path);
    
    QString getDefaultSshUser() const;
    void setDefaultSshUser(const QString& user);

    static constexpr int PROMETHEUS_PORT = 9090;

signals:
    void prometheusHostChanged(const QString& newHost);
    void jmeterSettingsChanged();
    void ansibleSettingsChanged();

private:
    AppSettings();
    void loadFromFile();
    void saveToFile() const;

private:
    // Prometheus
    QString m_prometheusHost;
    
    // JMeter
    int m_testDuration;
    QString m_localArchivePath;
    QString m_remoteTestDir;
    bool m_generateReport;
    QString m_resultsLocalDir;
    QString m_resultsRemoteDir;
    bool m_collectCpuMetrics;
    bool m_collectMemoryMetrics;
    QString m_jmeterHost;  // JMeter нагрузочный хост
    
    // Ansible
    QString m_ansiblePlaybookPath;
    QString m_defaultSshUser;

    static const QString SETTINGS_FILE;
};

// ==================== SettingsDialog ====================

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog();

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onTestPrometheusConnection();
    void onSelectArchive();
    void onSelectResultsLocalDir();
    void onSelectAnsiblePlaybook();
    void onRestoreDefaults();

private:
    void setupUI();
    void loadCurrentSettings();

private:
    QTabWidget* m_tabWidget;
    
    // Prometheus widgets
    QLineEdit* m_prometheusIpEdit;
    QLabel* m_currentUrlLabel;
    QPushButton* m_testPrometheusButton;
    QLabel* m_prometheusStatusLabel;
    
    // JMeter widgets
    QSpinBox* m_durationSpinBox;
    QLineEdit* m_archivePathEdit;
    QPushButton* m_browseArchiveButton;
    QLineEdit* m_remoteTestDirEdit;
    QCheckBox* m_generateReportCheckBox;
    QLineEdit* m_resultsLocalDirEdit;
    QPushButton* m_browseResultsButton;
    QLineEdit* m_resultsRemoteDirEdit;
    QCheckBox* m_collectCpuCheckBox;
    QCheckBox* m_collectMemoryCheckBox;
    QLineEdit* m_jmeterHostEdit;  // Добавлено поле для JMeter хоста
    
    // Ansible widgets
    QLineEdit* m_ansiblePlaybookEdit;
    QPushButton* m_browseAnsibleButton;
    QLineEdit* m_defaultSshUserEdit;
    
    // Buttons
    QPushButton* m_defaultsButton;
    QPushButton* m_saveButton;
    QPushButton* m_cancelButton;
};

// ==================== SettingsButton ====================

class SettingsButton : public QPushButton
{
    Q_OBJECT
public:
    explicit SettingsButton(QWidget* parent = nullptr);

signals:
    void settingsChanged();

private slots:
    void onButtonClicked();
    void onPrometheusSettingsChanged();
    void onJmeterSettingsChanged();
};

#endif // APPSETTINGS_H