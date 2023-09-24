cmake_minimum_required(VERSION 3.2) # Min version CMake

set(CMAKE_CXX_STANDARD 17)             # Стандарт C++
set(CMAKE_CXX_STANDARD_REQUIRED ON)    # Обязательность установки указанного стандарта (если OFF (по умолчанию), то воспринимается как совет)

project(Cauchy_problem_for_system
	VERSION 1.0
	LANGUAGES C CXX
)

add_executable(Cauchy_problem_for_system ${CMAKE_CURRENT_SOURCE_DIR}/source/Cauchy_problem_for_system.cpp)

target_include_directories(
  Cauchy_problem_for_system                           # Цель, при сборке которой учитываются указанные пути к заголовочным файлам
  PUBLIC                                # область видимости (PRIVATE, INTERFACE или PUBLIC)
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(${CMAKE_SOURCE_DIR}/libs)

target_link_libraries(Cauchy_problem_for_system
  PUBLIC
    ImGui-SFML::ImGui-SFML
	implot
	ImGui
)