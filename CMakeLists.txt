cmake_minimum_required(VERSION 3.16)

project(CpuStatCheck)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Указываем путь к Qt
set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.12.12/5.12.12/mingw73_64")

# Ищем Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Charts)

# Добавляем директории с заголовками
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers)

# Общие исходники
set(COMMON_SOURCES
    src/graphmanager.cpp
)

# Основное приложение
file(GLOB MAIN_SOURCES
    src/*.cpp
)

# Удаляем тестовые файлы из основного приложения
list(REMOVE_ITEM MAIN_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/test_prometheus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/test_prometheus_gui.cpp"
)

# Удаляем общие файлы, чтобы не дублировать
list(REMOVE_ITEM MAIN_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphmanager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphwidget.cpp"
)

file(GLOB HEADERS
    headers/*.h
)

file(GLOB UI_FILES
    ui/*.ui
)

# Добавляем исполняемый файл
add_executable(CpuStatCheck
    ${MAIN_SOURCES}
    ${COMMON_SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

# Подключаем библиотеки Qt
target_link_libraries(CpuStatCheck
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::Charts
)

# Создаем папку для результатов
add_custom_command(TARGET CpuStatCheck POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/results
)

# ============================================
# ТЕСТОВАЯ ПРОГРАММА (консольная)
# ============================================

# Добавляем тестовую программу (консольная)
add_executable(test_prometheus
    src/test_prometheus.cpp
    ${COMMON_SOURCES}
)

target_link_libraries(test_prometheus
    Qt5::Core
    Qt5::Charts
)

set_target_properties(test_prometheus PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

# ============================================
# ТЕСТОВАЯ ПРОГРАММА С GUI
# ============================================

# Добавляем тестовую программу с GUI
add_executable(test_prometheus_gui
    src/test_prometheus_gui.cpp
    src/graphmanager.cpp
)

target_link_libraries(test_prometheus_gui
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::Charts
)

set_target_properties(test_prometheus_gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

# Копируем debug.json в папку с тестом
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/debug.json" 
     DESTINATION "${CMAKE_BINARY_DIR}/tests")

# Добавляем тесты в CTest
enable_testing()
add_test(NAME PrometheusDataTest 
         COMMAND test_prometheus "${CMAKE_BINARY_DIR}/tests/debug.json")
add_test(NAME PrometheusDataTest2 
         COMMAND test_prometheus_gui "${CMAKE_BINARY_DIR}/tests/debug.json")