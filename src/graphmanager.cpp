#include "graphmanager.h"
#include "windowgraphics.h"

QT_CHARTS_USE_NAMESPACE

GraphManager::GraphManager(QObject *parent)
    : QObject(parent)
{
}

class AxisResizeFilter : public QObject {
private:
    QPointer<QChart> m_chart;
    qint64 m_minTime;
    qint64 m_maxTime;
    QDateTimeAxis* m_axisX;  // Сохраняем ссылку на существующую ось
    
public:
    AxisResizeFilter(QChart* chart, QDateTimeAxis* axisX, qint64 minTime, qint64 maxTime, QObject* parent = nullptr) 
        : QObject(parent), m_chart(chart), m_axisX(axisX), m_minTime(minTime), m_maxTime(maxTime) {}
    
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (event->type() == QEvent::Resize && !m_chart.isNull() && m_axisX) {
            QWidget* widget = qobject_cast<QWidget*>(obj);
            if (widget) {
                int width = widget->width();
                int tickCount;
                
                if (width < 400) {
                    tickCount = 3;
                } else if (width < 700) {
                    tickCount = 5;
                } else {
                    tickCount = 8;
                }
                
                qDebug() << "Resize: width=" << width << "setting tickCount=" << tickCount 
                         << "(range unchanged:" << m_minTime << "-" << m_maxTime << ")";
                
                // Меняем только количество меток, НЕ трогаем диапазон!
                m_axisX->setTickCount(tickCount);
            }
        }
        return false;
    }
};
double cleanValue(double value) {
    if (std::isnan(value) || std::isinf(value)) {
        return 0.0;
    }
    return value;
}

double calculateMean(const QVector<double>& values) {
    if (values.isEmpty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

// Структура для хранения данных серии
struct SeriesData {
    QString label;
    QMap<qint64, double> values;
    double maxValue;
    double meanValue;
};

void GraphManager::buildPrometheusChart(QtCharts::QChartView* chartView, const QString& query) {
    if (!chartView) {
        qDebug() << "ChartView is null!";
        return;
    }
    
    qDebug() << "Building chart with query:" << query;
    
    // ============ ПРЯМОЙ ЗАПРОС К PROMETHEUS ============
    QString prometheusUrl = "http://192.168.0.121:9090";
    QDateTime now = QDateTime::currentDateTime();

// Вычисляем время час назад
QDateTime oneHourAgo = now.addSecs(-3600); // -3600 секунд = -1 час

// Форматируем в нужный формат
// QString dateStart = oneHourAgo.toString("yyyy-MM-dd HH:mm:ss");
// QString dateEnd = now.toString("yyyy-MM-dd HH:mm:ss");
QString dateStart = "2026-04-28 17:00:00";
QString dateEnd = "2026-04-28 19:00:00";

// qDebug() << "Time range:" << dateStart << "-" << dateEnd;
    
    QJsonObject response = requestPrometheus(prometheusUrl, dateStart, dateEnd, query);
    
    bool isRPSQuery = query.contains("rate(", Qt::CaseInsensitive) || 
                      query.contains("requests_total", Qt::CaseInsensitive);
    bool isCPUQuery = query.contains("cpu", Qt::CaseInsensitive);
    bool isMemoryQuery = query.contains("memory", Qt::CaseInsensitive);

    qDebug() << "Query type:" << (isRPSQuery ? "RPS" : isCPUQuery ? "CPU" : isMemoryQuery ? "Memory" : "Response Time");

    if (response.isEmpty()) {
        qDebug() << "Failed to get data from Prometheus, falling back to file...";
        
        // Определяем имя тестового файла
        QString testFileName;
        if (query.contains("avg(jmeter_rt_summary)", Qt::CaseInsensitive)) {
            testFileName = "data_response_time.json";
        } else if (query.contains("by (label)", Qt::CaseInsensitive) && query.contains("rate(", Qt::CaseInsensitive)) {
            testFileName = "data_rps_by_endpoint.json";
        } else if (query.contains("rate(", Qt::CaseInsensitive)) {
            testFileName = "data_rps_total.json";
        } else if (isCPUQuery) {
            testFileName = "data_cpu.json";
        } else if (isMemoryQuery) {
            testFileName = "data_memory.json";
        } else {
            testFileName = "data_response_time.json";
        }
        
        qDebug() << "Looking for test file:" << testFileName;
        
        QFile file(testFileName);
        if (!file.exists()) {
            // Пробуем с ../ если не нашли в текущей папке
            testFileName = "../" + testFileName;
            file.setFileName(testFileName);
        }
        if (!file.exists()) {
            testFileName = "../../" + testFileName;
            file.setFileName(testFileName);
        }
        
        if (!file.exists()) {
            qDebug() << "Test file not found:" << testFileName;
            return;
        }
        
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open file:" << file.errorString();
            return;
        }
        
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        response = doc.object();
        file.close();
        
        qDebug() << "Loaded test data from:" << testFileName;
    } else {
        qDebug() << "Successfully got data from Prometheus";
        
        // Сохраняем ответ для отладки
        QFile file("prometheus_response.json");
        if (file.open(QIODevice::WriteOnly)) {
            QJsonDocument doc(response);
            file.write(doc.toJson(QJsonDocument::Indented));
            file.close();
            qDebug() << "Response saved to prometheus_response.json";
        }
    }
    
    qDebug() << "JSON parsed, status:" << response["status"].toString();
    
    // ============ ОБРАБОТКА ДАННЫХ ============
    struct SeriesData {
        QString label;
        QMap<qint64, double> values;
        double maxValue;
        double meanValue;
    };
    
    QVector<SeriesData> allSeries;
    QSet<qint64> allTimestamps;
    
    if (response["status"].toString() == "success" && 
        response["data"].toObject()["result"].toArray().size() > 0) {
        
        QJsonArray resultArray = response["data"].toObject()["result"].toArray();
        
        for (const QJsonValue& seriesValue : resultArray) {
            QJsonObject series = seriesValue.toObject();
            QJsonObject metric = series["metric"].toObject();
            
            QString label = metric.contains("label") ? metric["label"].toString() : "unknown";
            QString code = metric.contains("code") ? metric["code"].toString() : "";
            
            // Фильтрация
            // if (label.contains("Transaction", Qt::CaseInsensitive) || 
            //     label.contains("Debug", Qt::CaseInsensitive) || 
            //     label.contains("complex", Qt::CaseInsensitive)) {
            //     continue;
            // }
            
            if (!code.isEmpty() && code != "200") {
                continue;
            }
            
            QJsonArray values = series["values"].toArray();
            if (values.isEmpty()) continue;
            
            SeriesData sd;
            sd.label = label;
            sd.maxValue = 0;
            
            QVector<double> numValues;
            
            for (const QJsonValue& val : values) {
                QJsonArray point = val.toArray();
                if (point.size() >= 2) {
                    qint64 ts = point[0].toDouble();
                    double v = point[1].toString().toDouble();
                    
                    if (v > 0 && v < 100000) {
                        sd.values[ts] = v;
                        allTimestamps.insert(ts);
                        numValues.append(v);
                        if (v > sd.maxValue) sd.maxValue = v;
                    }
                }
            }
            
            if (!sd.values.isEmpty()) {
                sd.meanValue = calculateMean(numValues);
                allSeries.append(sd);
            }
        }
    }
    
    // Сортируем серии по среднему значению
    std::sort(allSeries.begin(), allSeries.end(), 
              [](const SeriesData& a, const SeriesData& b) {
                  return a.meanValue > b.meanValue;
              });
    
    // Ограничиваем количество серий
    int maxSeries = 100;
    if (allSeries.size() > maxSeries) {
        allSeries = allSeries.mid(0, maxSeries);
    }
    
    qDebug() << "Total series:" << allSeries.size();
    qDebug() << "Total timestamps:" << allTimestamps.size();
    
    if (allSeries.isEmpty() || allTimestamps.isEmpty()) {
        qDebug() << "No data to display!";
        return;
    }
    
    QList<qint64> sortedTimestamps = allTimestamps.values();
    std::sort(sortedTimestamps.begin(), sortedTimestamps.end());
    
    // ============ ПРЕДОПРЕДЕЛЕННЫЕ ЦВЕТА ДЛЯ ЛИНИЙ ============
    QList<QColor> lineColors = {
        QColor(0, 114, 189),    // Синий
        QColor(217, 83, 25),    // Оранжевый
        QColor(237, 177, 32),   // Желтый
        QColor(126, 47, 142),   // Фиолетовый
        QColor(119, 172, 48),   // Зеленый
        QColor(77, 190, 238),   // Голубой
        QColor(162, 20, 47),    // Бордовый
        QColor(0, 128, 128),    // Бирюзовый
        QColor(255, 105, 180),  // Розовый
        QColor(139, 69, 19),    // Коричневый
        QColor(64, 64, 64),     // Серый
        QColor(255, 140, 0)     // Темно-оранжевый
    };
    
    // ============ СОЗДАНИЕ ГРАФИКА ============
    QChart* chart = new QChart();
    chart->setTheme(QChart::ChartThemeLight);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTitle("API Response Time");
    
    int topCount = qMin(3, allSeries.size());
    QList<QAreaSeries*> areaSeriesList;
    
    for (int idx = 0; idx < allSeries.size(); ++idx) {
    const SeriesData& sd = allSeries[idx];
    
    // Выбираем цвет из списка или генерируем
    QColor seriesColor;
    if (idx < lineColors.size()) {
        seriesColor = lineColors[idx];
    } else {
        int hue = (idx * 37) % 360;
        seriesColor = QColor::fromHsv(hue, 200, 255);
    }
    
    // Создаем верхнюю линию
    QLineSeries* upperSeries = new QLineSeries();
    upperSeries->setName(sd.label);
    
    // Нижняя линия всегда 0
    QLineSeries* lowerSeries = new QLineSeries();
    
    for (qint64 ts : sortedTimestamps) {
        double value = sd.values.value(ts, 0.0);
        lowerSeries->append(ts * 1000, 0);
        upperSeries->append(ts * 1000, value);
    }
    
    // Создаем AreaSeries
    QAreaSeries* areaSeries = new QAreaSeries(upperSeries, lowerSeries);
    
    // Сокращаем имя для легенды
    QString seriesName = sd.label;
    if (seriesName.length() > 35) {
        seriesName = seriesName.left(32) + "...";
    }
    areaSeries->setName(seriesName);
    
    // ============ НАСТРОЙКА ЦВЕТА ============
    areaSeries->setColor(seriesColor);
    areaSeries->setBorderColor(seriesColor);
    
    // ============ ТОЛЬКО ВЕРХНЯЯ ЛИНИЯ ТОЛСТАЯ ============
    QPen upperLinePen(seriesColor, 2);  // Верхняя линия - 2px
    upperLinePen.setStyle(Qt::SolidLine);
    upperSeries->setPen(upperLinePen);
    
    QPen areaBorderPen(seriesColor, 1);  // Граница области - 1px
    areaSeries->setPen(areaBorderPen);
    
    // ============ ЗАЛИВКА ДЛЯ ВСЕХ ============
    QColor fillColor = seriesColor;
    fillColor.setAlpha(50);
    areaSeries->setBrush(QBrush(fillColor));
    areaSeries->setOpacity(1.0);
    
    chart->addSeries(areaSeries);
    areaSeriesList.append(areaSeries);
    
    qDebug() << "Added series:" << seriesName 
             << "color:" << seriesColor.name();
}
    
    // ============ НАСТРОЙКА ОСЕЙ ============
    QDateTimeAxis* axisX = new QDateTimeAxis();
    axisX->setFormat("HH:mm:ss");
    axisX->setTitleText("Время");

    qint64 minTime = sortedTimestamps.first();
    qint64 maxTime = sortedTimestamps.last();
    axisX->setRange(QDateTime::fromSecsSinceEpoch(minTime), 
                    QDateTime::fromSecsSinceEpoch(maxTime));

    // Расчет начального количества меток
    int chartWidth = chartView->width();
    int tickCount = (chartWidth < 400) ? 3 : (chartWidth < 700) ? 5 : 8;
    if (tickCount > sortedTimestamps.size()) tickCount = sortedTimestamps.size();
    axisX->setTickCount(tickCount);

    chart->addAxis(axisX, Qt::AlignBottom);

    // СОЗДАЕМ ФИЛЬТР - НОВАЯ ВЕРСИЯ (НЕ ПЕРЕСОЗДАЕТ ОСЬ)
    AxisResizeFilter* filter = new AxisResizeFilter(chart, axisX, minTime, maxTime, chartView);
    chartView->installEventFilter(filter);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("Response Time (ms)");
    axisY->setLabelFormat("%.0f");

    // Вычисляем максимальное значение для оси Y на основе всех данных
    double globalMaxValue = 0;
    for (const SeriesData& sd : allSeries) {
        if (sd.maxValue > globalMaxValue) {
            globalMaxValue = sd.maxValue;
        }
    }
    axisY->setRange(0, globalMaxValue * 1.1);

    chart->addAxis(axisY, Qt::AlignLeft);

    // Привязываем оси
    for (QAreaSeries* areaSeries : areaSeriesList) {
        areaSeries->attachAxis(axisX);
        areaSeries->attachAxis(axisY);
    }
    

    // Сохраняем оригинальные стили
    struct SeriesStyle {
        QAreaSeries* series;
        QColor color;
        bool isTop3;  // находится ли в топ-3
    };

    QVector<SeriesStyle> seriesStyles;
    for (int idx = 0; idx < areaSeriesList.size(); ++idx) {
        SeriesStyle style;
        style.series = areaSeriesList[idx];
        style.color = areaSeriesList[idx]->color();
        style.isTop3 = (idx < 3);
        seriesStyles.append(style);
        
        qDebug() << "Series" << idx << "color:" << style.color.name() 
                << "isTop3:" << style.isTop3;
        // Оригинальная линия
        QPen normalPen(style.color, 2);
        style.series->setPen(normalPen);
        style.series->upperSeries()->setPen(normalPen);
        style.series->setBorderColor(style.color);
        
        // Восстанавливаем заливку только для топ-3
        if (style.isTop3) {
            QColor fillColor = style.color;
            fillColor.setAlpha(50);  // 20% прозрачности
            style.series->setBrush(QBrush(fillColor));
        } else {
            style.series->setBrush(QBrush(Qt::NoBrush));
        }
    }
    // Подключаем обработчики для маркеров легенды
    for (QLegendMarker* marker : chart->legend()->markers()) {
        QAreaSeries* series = qobject_cast<QAreaSeries*>(marker->series());
        if (!series) continue;
        
        QObject::connect(marker, &QLegendMarker::hovered, 
                        [marker, seriesStyles](bool hovered) {
            
            // Получаем серию из маркера
            QAreaSeries* hoveredSeries = qobject_cast<QAreaSeries*>(marker->series());
            if (!hoveredSeries) return;
            
            // Находим индекс наведенной серии
            int hoveredIndex = -1;
            for (int i = 0; i < seriesStyles.size(); ++i) {
                if (seriesStyles[i].series == hoveredSeries) {
                    hoveredIndex = i;
                    break;
                }
            }
            
            if (hoveredIndex == -1) return;
            
            if (hovered) {
                // ПРИ НАВЕДЕНИИ - ВЫДЕЛЯЕМ ВЫБРАННУЮ СЕРИЮ
                for (int i = 0; i < seriesStyles.size(); ++i) {
                    const SeriesStyle& style = seriesStyles[i];
                    if (i == hoveredIndex) {
                        // ВЫБРАННАЯ СЕРИЯ - КАК ТОП-3 (С ЗАЛИВКОЙ)
                        QColor brightColor = style.color;

                        // Толстая линия
                        QPen thickPen(brightColor, 4);
                        style.series->setPen(thickPen);
                        style.series->upperSeries()->setPen(thickPen);
                        style.series->setBorderColor(brightColor);
                        
                        // ВСЕГДА ДОБАВЛЯЕМ ЗАЛИВКУ ПРИ НАВЕДЕНИИ
                        QColor fillColor = style.color;
                        fillColor.setAlpha(120);  // 47% прозрачности
                        style.series->setBrush(QBrush(fillColor));
                        
                    } else {
                        // ОСТАЛЬНЫЕ СЕРИИ - ЗАТЕМНЕННЫЕ И ТОНКИЕ
                        QColor dimColor = style.color;
                        dimColor.setAlpha(80);  // 31% прозрачности
                        
                        QPen thinPen(dimColor, 1);
                        style.series->setPen(thinPen);
                        style.series->upperSeries()->setPen(thinPen);
                        style.series->setBorderColor(dimColor);
                        
                        // Слабая заливка только для топ-3
                        if (style.isTop3) {
                            QColor fillColor = style.color;
                            fillColor.setAlpha(20);  // 8% прозрачности
                            style.series->setBrush(QBrush(fillColor));
                        } else {
                            style.series->setBrush(QBrush(Qt::NoBrush));
                        }
                    }
                }
            } else {
                // ВОЗВРАЩАЕМ ВСЕ НА МЕСТА
                for (int i = 0; i < seriesStyles.size(); ++i) {
                    const SeriesStyle& style = seriesStyles[i];
                    
                    // Оригинальная линия
                    QPen normalPen(style.color, 2);
                    style.series->setPen(normalPen);
                    style.series->upperSeries()->setPen(normalPen);
                    style.series->setBorderColor(style.color);
                    
                    // Восстанавливаем заливку только для топ-3
                    if (style.isTop3) {
                        QColor fillColor = style.color;
                        fillColor.setAlpha(50);  // 20% прозрачности
                        style.series->setBrush(QBrush(fillColor));
                    } else {
                        style.series->setBrush(QBrush(Qt::NoBrush));
                    }
                }
            }
        });
    }
    
    // Масштабирование только по X
    chartView->setRubberBand(QChartView::HorizontalRubberBand);
    chartView->setRenderHint(QPainter::Antialiasing);
    // После привязки осей и перед chartView->setChart
    QVector<QAreaSeries*> seriesVector;
    for (QAreaSeries* series : areaSeriesList) {
        seriesVector.append(series);
    }

    // Создаем обновлятель
    YAxisAutoUpdater* yUpdater = new YAxisAutoUpdater(chartView, axisX, axisY, seriesVector, chartView);

    // Принудительно вызываем обновление через 1 секунду
    QTimer::singleShot(1000, yUpdater, &YAxisAutoUpdater::updateYAxis);

    QChart* oldChart = chartView->chart();
    ZoomableChartView* zoomView = qobject_cast<ZoomableChartView*>(chartView);
if (zoomView) {
    zoomView->setOriginalRange(minTime * 1000, maxTime * 1000);
    zoomView->setOriginalMaxValue(globalMaxValue * 1.1);
}

// Создаем менеджер легенды
LegendManager* legendManager = new LegendManager(chart, axisX, axisY);

// Устанавливаем колбэк для ZoomableChartView
if (zoomView) {
    zoomView->setOnResetCallback([legendManager]() {
        if (legendManager) {
            legendManager->resetAllSeries();
        }
    });
}

chartView->setChart(chart);
delete oldChart;
    
    qDebug() << "Chart created successfully with" << allSeries.size() << "series";
}

QJsonObject GraphManager::requestPrometheus(const QString& prometheusUrl, 
                                           const QString& dateStart, 
                                           const QString& dateEnd,
                                           const QString& query) {
    
    // Преобразуем строки даты в QDateTime
    QDateTime dtStart = QDateTime::fromString(dateStart, "yyyy-MM-dd HH:mm:ss");
    QDateTime dtEnd = QDateTime::fromString(dateEnd, "yyyy-MM-dd HH:mm:ss");
    
    if (!dtStart.isValid() || !dtEnd.isValid()) {
        qDebug() << "Invalid date format!";
        return QJsonObject();
    }
    
    if (dtStart >= dtEnd) {
        qDebug() << "Start date must be before end date!";
        return QJsonObject();
    }
    
    qint64 startTimestamp = dtStart.toSecsSinceEpoch();
    qint64 endTimestamp = dtEnd.toSecsSinceEpoch();
    
    qDebug() << "Querying Prometheus:" << prometheusUrl;
    qDebug() << "Time range:" << dateStart << "-" << dateEnd;
    
    // Формируем URL запроса
    QUrl url(prometheusUrl + "/api/v1/query_range");
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("query", query);
    urlQuery.addQueryItem("start", QString::number(startTimestamp));
    urlQuery.addQueryItem("end", QString::number(endTimestamp));
    urlQuery.addQueryItem("step", "15s");
    url.setQuery(urlQuery);
    
    // Выполняем HTTP запрос
    QNetworkAccessManager manager;
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply* reply = manager.get(request);
    
    // Синхронное ожидание ответа с таймаутом
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start(10000); // 10 секунд таймаут
    loop.exec();
    
    if (!timer.isActive()) {
        qDebug() << "Prometheus request timeout";
        reply->abort();
        reply->deleteLater();
        return QJsonObject();
    }
    timer.stop();
    
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "HTTP error:" << reply->errorString();
        reply->deleteLater();
        return QJsonObject();
    }
    
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode != 200) {
        qDebug() << "Prometheus returned error:" << statusCode;
        qDebug() << "Response:" << reply->readAll();
        reply->deleteLater();
        return QJsonObject();
    }
    
    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    reply->deleteLater();
    
    if (!doc.isObject()) {
        qDebug() << "Invalid JSON response";
        return QJsonObject();
    }
    
    qDebug() << "Prometheus response received, size:" << responseData.size() << "bytes";
    
    return doc.object();
}

void GraphManager::forceUpdateYAxis() {
    qDebug() << "Force update Y axis called for all charts";
    
    // Обновляем все активные графики
    for (auto it = m_updaters.begin(); it != m_updaters.end(); ++it) {
        if (it.value() && !it.value().isNull()) {
            it.value()->updateYAxis();
            qDebug() << "Updated Y axis for chart:" << it.key();
        }
    }
}