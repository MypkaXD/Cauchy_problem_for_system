include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)

FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.89.9)

FetchContent_Declare(ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG 2.6.x)

FetchContent_Declare(implot
    GIT_REPOSITORY https://github.com/epezent/implot
    GIT_TAG v0.16)


message(STATUS "Fetching SFML...")

FetchContent_MakeAvailable(SFML)
##############

message(STATUS "Fetching ImGui...")
FetchContent_MakeAvailable(ImGui)

set(IMGUI_SRC
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui.cpp
)
set(IMGUI_HEADERS
  ${imgui_SOURCE_DIR}/imconfig.h
  ${imgui_SOURCE_DIR}/imgui_internal.h
  ${imgui_SOURCE_DIR}/imgui.h
  ${imgui_SOURCE_DIR}/imstb_rectpack.h
  ${imgui_SOURCE_DIR}/imstb_textedit.h
  ${imgui_SOURCE_DIR}/imstb_truetype.h
)
add_library(ImGui STATIC
  ${IMGUI_HEADERS}
  ${IMGUI_SRC}
)
###############
message(STATUS "Fetching ImPlot...")
FetchContent_MakeAvailable(implot)

set(IMPLOT_SRC
  ${implot_SOURCE_DIR}/implot.cpp
  ${implot_SOURCE_DIR}/implot_items.cpp
  ${implot_SOURCE_DIR}/implot_demo.cpp
)
set(IMPLOT_HEADERS
  ${implot_SOURCE_DIR}/implot.h
  ${implot_SOURCE_DIR}/implot_internal.h
)
add_library(implot STATIC
  ${IMPLOT_HEADERS}
  ${IMPLOT_SRC}
)
target_include_directories(implot PRIVATE ${imgui_SOURCE_DIR})
target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
###########

option(IMGUI_SFML_IMGUI_DEMO "Build imgui_demo.cpp" ON)
# No need to build audio and network modules
option(SFML_BUILD_AUDIO "Build audio" OFF)
option(SFML_BUILD_NETWORK "Build network" OFF)
option(SFML_BUILD_SYSTEM "Build system" ON)
option(SFML_BUILD_EXAMPLES "Build examples" OFF)

message(STATUS "Fetching ImGui-SFML...")
set(IMGUI_DIR ${imgui_SOURCE_DIR})
option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)

FetchContent_MakeAvailable(ImGui-SFML)
