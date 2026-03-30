#ifndef WINDOWGRAPHICS_H
#define WINDOWGRAPHICS_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>
#include <QTextEdit>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QtCharts>
#include "ProgressManager.h"
#include "graphmanager.h"

// Предварительное объявление
QT_CHARTS_USE_NAMESPACE

class WindowGraphics : public QWidget
{
    Q_OBJECT

public:
    explicit WindowGraphics(QWidget *parent = nullptr);
    ~WindowGraphics();

    void updateFilePathLabel(const QString& text, bool success = true);
    void appendOutput(const QString& text);
    void appendStatusBar(const QString& text);
    void clearOutput();
    void addHostToList(const QString& hostInfo);
    void removeHostFromList(int row);
    
    void processGraphData(const QString& jsonData);
    void clearGraphs();
    void exportGraphs(const QString& filePath);
    void importGraphs(const QString& filePath);
    void setGraphOpacity(double opacity);
    
    QLineEdit* getNewHostEdit() const { return newHostEdit; }
    QLineEdit* getSshUserEdit() const { return sshUserEdit; }
    QLineEdit* getSshPasswordEdit() const { return sshPasswordEdit; }
    QListWidget* getHostsListWidget() const { return hostsListWidget; }
    QPushButton* getAddHostButton() const { return addHostButton; }
    QPushButton* getRemoveHostButton() const { return removeHostButton; }
    QPushButton* getPlayButton() const { return playButton; }
    ProgressManager* getProgressManager() const { return progressManager; }

signals:
    void fileDropped(const QString& filePath);

public slots:
    void onGraphDataReceived(const QString& dataJson);
    void onGraphDataUpdated();

private:
    void setupUI();
    void updatePlayButtonState();
    void plotGraphs(const QVector<GraphSeries>& series);
    
    QLabel *dragDropLabel;
    QLabel *filePathLabel;
    QLineEdit *newHostEdit;
    QLineEdit *sshUserEdit;
    QLineEdit *sshPasswordEdit;
    QLabel *doggySign;
    QPushButton *addHostButton;
    QPushButton *removeHostButton;
    QListWidget *hostsListWidget;
    QProgressBar *progressBar;
    QPushButton *playButton;
    QTextEdit *outputTextEdit;
    QStatusBar *statusBar;
    
    GraphManager *graphManager;
    QChartView *m_chartView;
    QVBoxLayout *m_graphLayout;
    ProgressManager *progressManager;
};

#endif // WINDOWGRAPHICS_H