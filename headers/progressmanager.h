#ifndef PROGRESSMANAGER_H
#define PROGRESSMANAGER_H

#include <QObject>
#include <QProgressBar>
#include <QTimer>
#include <QElapsedTimer>

class ProgressManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int progressValue READ progressValue NOTIFY progressChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY runningStateChanged)

public:
    explicit ProgressManager(QObject *parent = nullptr);
    
    int progressValue() const { return m_progressValue; }
    QString statusText() const { return m_statusText; }
    bool isRunning() const { return m_isRunning; }
    
    void setProgressBar(QProgressBar* progressBar);
    void setErrorMode(bool isError);
public slots:
    void startProgress(int maxSteps = 100);
    void stopProgress(bool success = true);
    void updateProgress(int step, const QString& stepDescription = QString());
    void incrementProgress(const QString& stepDescription = QString());
    void setStatusText(const QString& text);
    void reset();

private slots:
    void onIndeterminateTimer();

private:
    QProgressBar* m_progressBar;
    QTimer* m_indeterminateTimer;
    QElapsedTimer m_elapsedTimer;
    
    int m_progressValue;
    int m_maxSteps;
    QString m_statusText;
    bool m_isRunning;
    bool m_isIndeterminateMode;
    
    void updateProgressBar();
    void enterIndeterminateMode();
    void exitIndeterminateMode();
    
signals:
    void progressChanged(int value);
    void statusChanged(const QString& text);
    void runningStateChanged(bool running);
    void progressCompleted(bool success);
    void stepReached(int step, const QString& description);
};

#endif // PROGRESSMANAGER_H