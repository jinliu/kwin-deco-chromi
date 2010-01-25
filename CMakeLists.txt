cmake_minimum_required(VERSION 2.8)

project(chromi)

find_package(KDE4 REQUIRED)
find_package(KDE4Workspace REQUIRED)
include (KDE4Defaults)
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

add_subdirectory(src)
add_subdirectory(img)
