#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QCommandLineParser>
#include <QDir>
#include <QTextStream>
#include <QRegularExpression>
#include "graphmanager.h"

class PrometheusTest {
public:
    static bool runTest(const QString& filePath, bool verbose = true) {
        QTextStream out(stdout);
        
        if (verbose) {
            out << "\n========================================\n";
            out << "Prometheus Data Processor Test\n";
            out << "========================================\n\n";
        }
        
        // Проверяем существование файла
        if (!QFile::exists(filePath)) {
            out << "❌ ERROR: File not found: " << filePath << "\n";
            out << "   Working directory: " << QDir::currentPath() << "\n";
            return false;
        }
        
        // Читаем файл
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            out << "❌ ERROR: Cannot open file: " << filePath << "\n";
            return false;
        }
        
        QByteArray jsonData = file.readAll();
        file.close();
        
        if (verbose) {
            out << "📁 File: " << QFileInfo(filePath).fileName() << "\n";
            out << "📊 Size: " << jsonData.size() << " bytes\n\n";
        }
        
        // Очищаем JSON от NaN
        QString jsonString = QString::fromUtf8(jsonData);
        jsonString.replace(QRegularExpression("\\bNaN\\b"), "null");
        jsonString.replace(QRegularExpression("\\bnan\\b"), "null");
        jsonString.replace(QRegularExpression("\\bInfinity\\b"), "null");
        jsonString.replace(QRegularExpression("\\b-infinity\\b"), "null");
        
        QByteArray cleanedData = jsonString.toUtf8();
        
        // Парсим JSON
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(cleanedData, &parseError);
        
        if (parseError.error != QJsonParseError::NoError) {
            out << "❌ Invalid JSON: " << parseError.errorString() << "\n";
            return false;
        }
        
        if (verbose) {
            out << "✅ JSON is valid\n";
            
            // Выводим структуру JSON для отладки
            QJsonObject root = doc.object();
            out << "🔍 JSON structure:\n";
            out << "   Keys: " << root.keys().join(", ") << "\n";
            
            if (root.contains("status")) {
                out << "   status: " << root["status"].toString() << "\n";
            }
            if (root.contains("data")) {
                QJsonObject data = root["data"].toObject();
                out << "   data keys: " << data.keys().join(", ") << "\n";
                if (data.contains("result")) {
                    QJsonArray resultArray = data["result"].toArray();
                    out << "   result count: " << resultArray.size() << "\n";
                }
            }
            out << "\n";
        }
        
        // Создаем GraphManager и обрабатываем данные
        GraphManager graphManager;
        
        // Подключаем сигналы для отладки
        QObject::connect(&graphManager, &GraphManager::errorOccurred, 
            [&out](const QString& error) {
                out << "❌ GraphManager error: " << error << "\n";
            });
        
        QObject::connect(&graphManager, &GraphManager::graphDataUpdated,
            [&out]() {
                out << "✅ Graph data updated\n";
            });
        
        // Обрабатываем данные
        bool success = graphManager.processPrometheusData(doc);
        
        if (!success) {
            out << "❌ Failed to process Prometheus data\n";
            return false;
        }
        
        // Получаем обработанные данные
        QVector<GraphData> graphs = graphManager.getAllGraphs();
        
        if (graphs.isEmpty()) {
            out << "⚠️  WARNING: No data processed\n";
            return false;
        }
        
        // Получаем статистику
        QVector<MetricStats> stats = graphManager.getStatistics();
        
        // Выводим статистику
        printStatistics(stats, verbose);
        
        // Выводим детали графиков
        if (verbose) {
            printGraphDetails(graphs);
        }
        
        // Проверяем фильтрацию
        bool filterPassed = checkFiltering(graphs);
        
        if (verbose) {
            out << "\n========================================\n";
            if (filterPassed && !graphs.isEmpty()) {
                out << "✅ TEST PASSED\n";
                out << QString("   Processed %1 metrics\n").arg(graphs.size());
            } else {
                out << "❌ TEST FAILED\n";
            }
            out << "========================================\n\n";
        }
        
        return filterPassed && !graphs.isEmpty();
    }
    
private:
    static void printStatistics(const QVector<MetricStats>& stats, bool verbose) {
        QTextStream out(stdout);
        
        if (verbose) {
            out << "📈 STATISTICS:\n";
            out << "----------------------------------------\n";
        }
        
        for (const MetricStats& stat : stats) {
            out << QString("  • %1").arg(stat.label);
            out << "\n";
            out << QString("    Min:  %1 ms\n").arg(stat.min, 0, 'f', 2);
            out << QString("    Max:  %1 ms\n").arg(stat.max, 0, 'f', 2);
            out << QString("    Mean: %1 ms\n").arg(stat.mean, 0, 'f', 2);
            out << QString("    Points: %1\n").arg(stat.dataPointsCount);
            
            if (verbose) {
                out << "    ---\n";
            }
        }
        
        if (verbose) {
            out << "----------------------------------------\n\n";
        }
    }
    
    static void printGraphDetails(const QVector<GraphData>& graphs) {
        QTextStream out(stdout);
        
        out << "🎨 GRAPH DETAILS:\n";
        out << "----------------------------------------\n";
        
        for (const GraphData& graph : graphs) {
            out << QString("  Label: %1\n").arg(graph.label);
            if (!graph.code.isEmpty()) {
                out << QString("  Code: %1\n").arg(graph.code);
            }
            out << QString("  Color: %1\n").arg(graph.color.name());
            out << QString("  Opacity: %1\n").arg(graph.opacity);
            out << QString("  Range: [%1, %2] ms\n")
                .arg(graph.minValue, 0, 'f', 2)
                .arg(graph.maxValue, 0, 'f', 2);
            
            if (!graph.values.isEmpty()) {
                out << "  Values (first 3):\n";
                int showCount = qMin(3, graph.values.size());
                for (int i = 0; i < showCount; ++i) {
                    out << QString("    [%1] %2 ms\n")
                        .arg(i)
                        .arg(graph.values[i], 0, 'f', 2);
                }
            }
            out << "  ---\n";
        }
        out << "----------------------------------------\n\n";
    }
    
    static bool checkFiltering(const QVector<GraphData>& graphs) {
        QTextStream out(stdout);
        
        bool hasTransaction = false;
        bool hasDebug = false;
        bool hasComplex = false;
        
        for (const GraphData& graph : graphs) {
            if (graph.label.contains("Transaction", Qt::CaseInsensitive)) hasTransaction = true;
            if (graph.label.contains("Debug", Qt::CaseInsensitive)) hasDebug = true;
            if (graph.label.contains("complex", Qt::CaseInsensitive)) hasComplex = true;
        }
        
        if (!hasTransaction && !hasDebug && !hasComplex) {
            out << "✅ Filter test passed: Transaction, Debug, complex metrics were filtered out\n";
            return true;
        } else {
            out << "❌ Filter test failed: Some filtered metrics remain\n";
            if (hasTransaction) out << "   - Transaction metrics found\n";
            if (hasDebug) out << "   - Debug metrics found\n";
            if (hasComplex) out << "   - Complex metrics found\n";
            return false;
        }
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("Prometheus Data Test");
    QCoreApplication::setApplicationVersion("1.0");
    
    QCommandLineParser parser;
    parser.setApplicationDescription("Test Prometheus data processing with GraphManager");
    parser.addHelpOption();
    parser.addVersionOption();
    
    parser.addPositionalArgument("file", "JSON file with Prometheus data", "file");
    
    parser.process(app);
    
    const QStringList args = parser.positionalArguments();
    
    QString filePath;
    if (args.isEmpty()) {
        // Пробуем найти файл в разных местах
        if (QFile::exists("debug.json")) {
            filePath = "debug.json";
        } else if (QFile::exists("../src/debug.json")) {
            filePath = "../src/debug.json";
        } else if (QFile::exists("src/debug.json")) {
            filePath = "src/debug.json";
        } else {
            qDebug() << "No file specified and debug.json not found";
            parser.showHelp();
            return 1;
        }
        qDebug() << "Using default file:" << filePath;
    } else {
        filePath = args.first();
    }
    
    // Запускаем тест
    bool success = PrometheusTest::runTest(filePath, true);
    
    return success ? 0 : 1;
}