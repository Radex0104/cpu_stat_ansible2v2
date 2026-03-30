import datetime
import requests
import json
import sys
import traceback
import math


def request_prometheus(prometheus_url: str, date_start="2026-01-15 10:32:40", date_end="2026-01-15 10:52:57"):
    """Получить среднее время ответа по каждому запросу из Prometheus"""

    query = 'avg(jmeter_rt_summary) by (label, code)'

    # Преобразуем строки даты в объекты datetime
    dt_start = datetime.datetime.strptime(date_start, "%Y-%m-%d %H:%M:%S")
    dt_end = datetime.datetime.strptime(date_end, "%Y-%m-%d %H:%M:%S")

    # Проверяем, что начальная дата меньше конечной
    if dt_start >= dt_end:
        raise ValueError(
            f"Начальная дата ({date_start}) должна быть РАНЬШЕ конечной даты ({date_end})!"
        )

    # Получаем временные метки в секундах
    start_timestamp = int(dt_start.timestamp())
    end_timestamp = int(dt_end.timestamp())

    print(f"Начальное время: {date_start} (сек: {start_timestamp})", file=sys.stderr)
    print(f"Конечное время: {date_end} (сек: {end_timestamp})", file=sys.stderr)
    print(f"Длительность: {(dt_end - dt_start).total_seconds()} секунд", file=sys.stderr)

    try:
        response = requests.get(
            f"{prometheus_url}/api/v1/query_range",
            params={
                'query': query,
                'start': start_timestamp,
                'end': end_timestamp,
                'step': '15s'
            },
            timeout=30
        )

        # Проверяем успешность запроса
        response.raise_for_status()

        data = response.json()

        # Проверяем статус ответа от Prometheus
        if data.get('status') == 'success':
            return data
        else:
            error_msg = data.get('error', 'Неизвестная ошибка Prometheus')
            raise Exception(f"Ошибка Prometheus: {error_msg}")

    except requests.exceptions.RequestException as e:
        print(f"Ошибка HTTP запроса: {e}", file=sys.stderr)
        raise
    except ValueError as e:
        print(f"Ошибка данных: {e}", file=sys.stderr)
        raise


def clean_value(value):
    if isInstance(value, float):
            if math.isnan(value) or math.isinf(value):
                return 0.0
            return value

def convert_to_graph_data(prometheus_data):
    """Преобразование данных Prometheus в формат для Qt графика"""
    
    result_data = {
        "results": []
    }
    
    if "results" in prometheus_data and isInstance(prometheus_data["results"], list):
        for item in prometheus_data["results"]:
            if "values" in item:
                item["values"] = [clean_value(v) for v in item["values"]]
            if item.get("timestamps") and len(item["timestamps"]) > 0:
                result_data["results"].append(item)
        return result_data

    if prometheus_data.get('status') == 'success':
        for result in prometheus_data['data']['result']:
            metric = result['metric']
            label = metric.get('label', 'unknown')
            code = metric.get('code', 'unknown')
            values = result['values']
            
            timestamps = []
            metric_values = []
            
            for timestamp_str, value_str in values:
                timestamp = float(timestamp_str)
                value = float(value_str)
                
                timestamps.append(timestamp)
                metric_values.append(value)
            
            graph_entry = {
                'label': label,
                'code': code,
                'timestamps': timestamps,
                'values': metric_values
            }
            
            result_data['results'].append(graph_entry)
    
    return result_data


# Пример использования с правильными датами
if __name__ == "__main__":
    try:
        # ПРАВИЛЬНО: дата начала РАНЬШЕ даты окончания
        result = request_prometheus(
            prometheus_url="http://172.16.178.6:9090",
            date_start="2026-03-23 08:42:04",  # ← РАНЬШЕ
            date_end="2026-03-23 09:39:41"  # ← ПОЗЖЕ
        )
        
        # Преобразуем в формат для графика
        graph_data = convert_to_graph_data(result)
        
        # Преобразуем в JSON и выводим в stdout для Qt
        json_data = json.dumps(graph_data)
        
        print(f"Успешно обработано {len(graph_data['results'])} метрик", file=sys.stderr)
        
        # Выводим JSON в stdout - Qt прочитает его через readyReadStandardOutput
        print(json_data)
        
        print("Данные получены успешно!", file=sys.stderr)

    except Exception as e:
        print(f"Произошла ошибка: {e}", file=sys.stderr)
        traceback.print_exc(file=sys.stderr)
        sys.exit(1)