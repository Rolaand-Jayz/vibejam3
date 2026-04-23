cmake_minimum_required(VERSION 3.21)
project(MyPlaithrough VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Test)

qt_standard_project_setup()

qt_add_executable(my_plaithrough
    src/main.cpp
    src/project.hpp
    src/project_repository.hpp
    src/project_repository.cpp
    src/new_project_dialog.hpp
    src/new_project_dialog.cpp
    src/blueprint_dialog.hpp
    src/blueprint_dialog.cpp
    src/knowledge_workbench_dialog.hpp
    src/knowledge_workbench_dialog.cpp
    src/preview_review_dialog.hpp
    src/preview_review_dialog.cpp
    src/build_bundle_review_dialog.hpp
    src/build_bundle_review_dialog.cpp
    src/main_window.hpp
    src/main_window.cpp
)

target_link_libraries(my_plaithrough
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

target_compile_options(my_plaithrough PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

enable_testing()

qt_add_executable(my_plaithrough_repository_test
    tests/project_repository_test.cpp
    src/project.hpp
    src/project_repository.hpp
    src/project_repository.cpp
)

target_include_directories(my_plaithrough_repository_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(my_plaithrough_repository_test
    PRIVATE
        Qt6::Core
        Qt6::Test
)

target_compile_options(my_plaithrough_repository_test PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_test(NAME my_plaithrough_repository_test COMMAND my_plaithrough_repository_test)

set_target_properties(my_plaithrough PROPERTIES
    OUTPUT_NAME "my-plaithrough"
)

include(GNUInstallDirs)
install(TARGETS my_plaithrough RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
