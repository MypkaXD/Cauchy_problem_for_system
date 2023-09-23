cmake_minimum_required(VERSION 3.2) # Min version CMake

set(CMAKE_CXX_STANDARD 17)             # Стандарт C++
set(CMAKE_CXX_STANDARD_REQUIRED ON)    # Обязательность установки указанного стандарта (если OFF (по умолчанию), то воспринимается как совет)

project(Dynamic_2
	VERSION 1.0
	LANGUAGES C CXX
)

add_executable(Cauchy_problem_for_system ${CMAKE_CURRENT_SOURCE_DIR}/source/Dynamic_2.cpp)

target_include_directories(
  Dynamic_2                           # Цель, при сборке которой учитываются указанные пути к заголовочным файлам
  PUBLIC                                # область видимости (PRIVATE, INTERFACE или PUBLIC)
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(${CMAKE_SOURCE_DIR}/libs)

target_link_libraries(Dynamic_2
  PUBLIC
    ImGui-SFML::ImGui-SFML
)