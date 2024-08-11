cmake_minimum_required(VERSION 3.15)

project(FFmpegPipeEncoder VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC)

set(PUBLIC_HEADERS ${PROJECT_NAME}/PipeEncoder.hpp)

target_sources(${PROJECT_NAME}
    PRIVATE
        ${PUBLIC_HEADERS}
        ${PROJECT_NAME}/PipeEncoder.cpp
        ${PROJECT_NAME}/EncoderSettings.hpp
        ${PROJECT_NAME}/LockFreeQueue.hpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/${PROJECT_NAME}
)

install(FILES ${PUBLIC_HEADERS} DESTINATION include/${PROJECT_NAME})