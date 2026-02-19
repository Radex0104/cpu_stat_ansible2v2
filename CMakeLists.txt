cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Указываем путь к Qt (ваш конкретный путь)
set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.12.12/5.12.12/mingw73_64")

# Ищем Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers)
# Добавляем исполняемый файл

file(GLOB SOURCES
    "src/*.cpp"
)

file(GLOB HEADERS
    "headers/*.h"
)

file(GLOB UI_FILES
"   ui/*.ui"
)

# Добавляем исполняемый файл
add_executable(CpuStatCheck
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

# Подключаем библиотеки Qt
target_link_libraries(CpuStatCheck
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
)

# Копируем ansible.yml в папку сборки
add_custom_command(TARGET CpuStatCheck POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/ansible.yml
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ansible.yml
)

# Создаем папку для результатов
add_custom_command(TARGET CpuStatCheck POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/results
)