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

# --- JSON
include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)


# ============================================================
# Binaires
# ============================================================

######################################



###########################################
# --- Test JSON
add_executable(test test.cpp)
target_link_libraries(test PRIVATE nlohmann_json::nlohmann_json)

#########################################
# --- Generate lund files
add_executable(generate_lund_file 
    generate_lund_file.cpp
    fOptions.cpp
)

target_include_directories(generate_lund_file PRIVATE
    ${CMAKE_SOURCE_DIR}/
    ${HIPO_DIR}
    ${ROOT_INCLUDE_DIRS}
) # header

target_link_libraries(generate_lund_file PRIVATE 
    ${HIPO_DIR}/lib/libhipo4.so
    ${HIPO_DIR}/lib/liblz4.so
    ${ROOT_LIBRARIES}
    nlohmann_json::nlohmann_json
) # libray

target_link_libraries(generate_lund_file PRIVATE nlohmann_json::nlohmann_json)