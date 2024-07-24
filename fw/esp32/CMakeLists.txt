cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(COOLKIDSBADGE)
idf_build_set_property(COMPILE_OPTIONS "-fdiagnostics-color=always" APPEND)
