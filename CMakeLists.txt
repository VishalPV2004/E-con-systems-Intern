# Set the cross-compilation environment
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)
set(CMAKE_C_COMPILER /opt/riscv/eyenix/bin/riscv64-eyenix-linux-gnu-gcc)

# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.0)

# Project name
project(GovComm C)

# Add the source files to the executable
add_executable(GovComm
    main.c
    FrameStreamer.c
    aieventhandler.c
    cJSON.c
    tcp_listener.c
    logger.c
)

# Add include directories
target_include_directories(GovComm PRIVATE
    ${CMAKE_SOURCE_DIR}  # for cJSON.h and other headers in the project dir
)

# Link the required libraries
target_link_directories(GovComm PRIVATE
    /home/vishal/Downloads/APP_latest_AIMODEL_14052024PM_latest_1/APP_latest_AIMODEL
)

target_link_libraries(GovComm PRIVATE
    clovis_enx_3
    pthread
    rt
)
