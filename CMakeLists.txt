cmake_minimum_required(VERSION 2.8.8)
project(qipp)
find_package(Qt4 REQUIRED)
find_package(Cups REQUIRED)
if(${CUPS_VERSION_STRING} VERSION_GREATER 1.7)
	set(CUPS_17 1)
else(${CUPS_VERSION_STRING} VERSION_GREATER 1.7)
	set(CUPS_17 0)
endif(${CUPS_VERSION_STRING} VERSION_GREATER 1.7)

configure_file ("${PROJECT_SOURCE_DIR}/config.h.in" "${PROJECT_BINARY_DIR}/config.h" )

include_directories(${QT_INCLUDES} ${CUPS_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(QIpp_SRCS QIpp.cpp main.cpp)

qt4_automoc(${QIpp_SRCS})
add_executable(qipp ${QIpp_SRCS})
target_link_libraries(qipp ${QT_QTCORE_LIBRARY} ${CUPS_LIBRARIES})

install(TARGETS qipp RUNTIME DESTINATION bin)
