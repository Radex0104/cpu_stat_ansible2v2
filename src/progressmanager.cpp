#include "progressmanager.h"
#include <QApplication>

ProgressManager::ProgressManager(QObject *parent)
    : QObject(parent)
    , m_progressBar(nullptr)
    , m_indeterminateTimer(new QTimer(this))
    , m_progressValue(0)
    , m_maxSteps(100)
    , m_isRunning(false)
    , m_isIndeterminateMode(false)
{
    m_indeterminateTimer->setInterval(100);
    connect(m_indeterminateTimer, &QTimer::timeout, this, &ProgressManager::onIndeterminateTimer);
}

void ProgressManager::setProgressBar(QProgressBar* progressBar)
{
    m_progressBar = progressBar;
    if (m_progressBar) {
        m_progressBar->setVisible(false);
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(0);
    }
    m_progressBar->setStyleSheet(
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
}

void ProgressManager::startProgress(int maxSteps)
{
    m_isRunning = true;
    m_maxSteps = maxSteps;
    m_progressValue = 0;
    m_isIndeterminateMode = false;
    
    if (m_progressBar) {
        m_progressBar->setVisible(true);
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(0);
        m_progressBar->setStyleSheet(
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
    }
    
    m_elapsedTimer.start();
    
    emit runningStateChanged(true);
    emit progressChanged(0);
    setStatusText("Запуск процесса...");
}

void ProgressManager::stopProgress(bool success)
{
    if (!m_isRunning) return;
    
    m_isRunning = false;
    m_isIndeterminateMode = false;
    m_indeterminateTimer->stop();
    
    if (success) {
        m_progressValue = 100;
        if (m_progressBar) {
            m_progressBar->setValue(100);
        }
        setStatusText("Процесс завершен успешно");
    } else {
        setErrorMode(true);
        setStatusText("Процесс завершен с ошибкой");
    }
    
    emit progressChanged(m_progressValue);
    emit runningStateChanged(false);
    emit progressCompleted(success);
    
    // Автоматически скрываем прогресс-бар через 3 секунды
    // QTimer::singleShot(3000, this, [this]() {
    //     if (!m_isRunning && m_progressBar) {
    //         m_progressBar->setVisible(false);
    //     }
    // });
}

void ProgressManager::setErrorMode(bool isError)
{
    if (!m_progressBar) return;
    
    if (isError) {
        // Красный стиль для ошибки
        m_progressBar->setStyleSheet(
            "QProgressBar {"
            "    border: 1px solid #bbb;"
            "    border-radius: 5px;"
            "    text-align: center;"
            "    height: 25px;"
            "}"
            "QProgressBar::chunk {"
            "    background-color: #f44336;"  // Красный цвет
            "    border-radius: 5px;"
            "}"
        );
        
        // Если процесс еще не завершен, можно установить текст ошибки
        if (m_isRunning) {
            setStatusText("ОШИБКА! Процесс прерван");
        }
    } else {
        // Возвращаем зеленый стиль
        m_progressBar->setStyleSheet(
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
    }
}

void ProgressManager::updateProgress(int step, const QString& stepDescription)
{
    if (!m_isRunning) return;
    
    m_progressValue = qMin(100, static_cast<int>((static_cast<float>(step) / m_maxSteps) * 100));
    
    if (m_progressBar) {
        m_progressBar->setValue(m_progressValue);
    }
    
    if (!stepDescription.isEmpty()) {
        setStatusText(stepDescription);
    }
    
    emit progressChanged(m_progressValue);
    emit stepReached(step, stepDescription);
}

void ProgressManager::incrementProgress(const QString& stepDescription)
{
    if (!m_isRunning) return;
    
    int currentStep = static_cast<int>((static_cast<float>(m_progressValue) / 100) * m_maxSteps) + 1;
    updateProgress(currentStep, stepDescription);
}

void ProgressManager::setStatusText(const QString& text)
{
    m_statusText = text;
    emit statusChanged(text);
}

void ProgressManager::reset()
{
    m_isRunning = false;
    m_isIndeterminateMode = false;
    m_indeterminateTimer->stop();
    m_progressValue = 0;
    m_statusText.clear();
    
    if (m_progressBar) {
        m_progressBar->setVisible(false);
        m_progressBar->setValue(0);
        m_progressBar->setStyleSheet(
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
    }
    
    emit progressChanged(0);
    emit runningStateChanged(false);
}

void ProgressManager::enterIndeterminateMode()
{
    if (!m_isRunning || m_isIndeterminateMode) return;
    
    m_isIndeterminateMode = true;
    m_indeterminateTimer->start();
    
    if (m_progressBar) {
        m_progressBar->setRange(0, 0); // Неопределенный режим
    }
    
    setStatusText("Выполняется...");
}

void ProgressManager::exitIndeterminateMode()
{
    if (!m_isIndeterminateMode) return;
    
    m_isIndeterminateMode = false;
    m_indeterminateTimer->stop();
    
    if (m_progressBar) {
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(m_progressValue);
    }
}

void ProgressManager::onIndeterminateTimer()
{
    // Анимируем неопределенный прогресс
    static int pulse = 0;
    if (m_progressBar && m_isIndeterminateMode) {
        // В Qt нет встроенной анимации для неопределенного режима,
        // поэтому просто обновляем для визуального эффекта
        m_progressBar->repaint();
    }
}

void ProgressManager::updateProgressBar()
{
    if (m_progressBar && !m_isIndeterminateMode) {
        m_progressBar->setValue(m_progressValue);
    }
}