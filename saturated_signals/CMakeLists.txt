set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# --- CMake version ---
cmake_minimum_required(VERSION 3.16)
project(saturatedSignals LANGUAGES CXX)

# --- Standard C++ ---
set(CMAKE_CXX_STANDARD 17)             # équivalent de -std=c++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)    # échoue si le compilateur ne supporte pas C++17
                                        # plutôt que de silencieusement revenir à une version antérieure

# --- Chemin vers HIPO, surchageable via: cmake -DHIPO_DIR=/autre/chemin .. ---
set(HIPO_DIR "/u/scigroup/cvmfs/geant4/almalinux9-gcc11/hipo/4.1.0" CACHE PATH "Chemin vers HIPO")

# --- ROOT : cherche l'installation via ROOTConfig.cmake fourni par ROOT ---
find_package(ROOT REQUIRED COMPONENTS Core)
include(${ROOT_USE_FILE})   # charge les macros ROOT et définit ROOT_INCLUDE_DIRS / ROOT_LIBRARIES


# ============================================================
# Binaires
# ============================================================

# --- run only hit from track

add_executable(run 
    run.cpp
    fOptions.cpp
) # cpp code

target_include_directories(run PRIVATE
    ${CMAKE_SOURCE_DIR}/
    ${HIPO_DIR}
    ${ROOT_INCLUDE_DIRS}
) # header

target_link_libraries(run PRIVATE 
    ${HIPO_DIR}/lib/libhipo4.so
    ${HIPO_DIR}/lib/liblz4.so
    ${ROOT_LIBRARIES}
) # libray


# --- run all hits for more statistics

add_executable(run_all_hits 
    run_all_hits.cpp
    fOptions.cpp
) # cpp code

target_include_directories(run_all_hits  PRIVATE
    ${CMAKE_SOURCE_DIR}/
    ${HIPO_DIR}
    ${ROOT_INCLUDE_DIRS}
) # header

target_link_libraries(run_all_hits  PRIVATE 
    ${HIPO_DIR}/lib/libhipo4.so
    ${HIPO_DIR}/lib/liblz4.so
    ${ROOT_LIBRARIES}
) # libray