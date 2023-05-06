cmake_minimum_required(VERSION 3.25)
project(NumbersConverter)

set(SOURCE_FILES src/main.cpp)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PRIVATE
        ${SOURCE_FILES})

find_package(Curses REQUIRED)
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES} 
                                      ${CURSES_FORM_LIBRARY})
