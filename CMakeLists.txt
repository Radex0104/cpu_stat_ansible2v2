cmake_minimum_required(VERSION 3.16)

project(ADM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Указываем путь к Qt
set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.12.12/5.12.12/mingw73_64")

# Ищем Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Charts Network)

# Добавляем директории с заголовками
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers)

# Основное приложение
file(GLOB SOURCES
    src/*.cpp
)

# Удаляем тестовые файлы из основного приложения
list(REMOVE_ITEM SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/test_prometheus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/test_prometheus_gui.cpp"
)

file(GLOB HEADERS
    headers/*.h
)

file(GLOB UI_FILES
    ui/*.ui
)

# Добавляем исполняемый файл
add_executable(ADM
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

# Подключаем библиотеки Qt
target_link_libraries(ADM
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::Charts
    Qt5::Network
)
