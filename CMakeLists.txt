cmake_minimum_required(VERSION 2.6)
project(qipp)
find_package(Qt4 REQUIRED)
find_package(Cups 1.7 REQUIRED)

include_directories(${QT_INCLUDES} ${CUPS_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(QIpp_SRCS QIpp.cpp main.cpp)

qt4_automoc(${QIpp_SRCS})
add_executable(qipp ${QIpp_SRCS})
target_link_libraries(qipp ${QT_QTCORE_LIBRARY} ${CUPS_LIBRARIES})

add_subdirectory(tests)

install(TARGETS qipp RUNTIME DESTINATION bin)
