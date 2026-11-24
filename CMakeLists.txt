cmake_minimum_required(VERSION 3.16)
project(actividad_integral_3 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(actividad_integral_3 main.cpp)

# Copia la bitacora de la actividad anterior junto al ejecutable para que el
# programa la encuentre sin importar desde donde se compile
configure_file(bitacora_ordenada_ip.csv ${CMAKE_CURRENT_BINARY_DIR}/bitacora_ordenada_ip.csv COPYONLY)
