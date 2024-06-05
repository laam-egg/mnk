cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(GNUInstallDirs)
include(GenerateExportHeader)
include(FetchContent)
include(ExternalProject)

##########################
######### libmnk #########
##########################

add_library(mnk STATIC
    Agent.cpp
    CellIterator.cpp
    State.cpp
)
target_include_directories(mnk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

set_target_properties(mnk PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../bin"
    PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/../include/Application.h"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET mnk PROPERTY CXX_STANDARD 17)
endif()

##########################
######## mnks.so #########
##########################

add_library(mnks SHARED
    shared.cpp
)

target_include_directories(mnks PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

set_target_properties(mnks PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../bin"
    PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/../include/shared.h"
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN 1
)

generate_export_header(mnks)
install(TARGETS mnks DESTINATION ${LIBRARY_INSTALL_DIR})
target_include_directories(mnks PRIVATE ${CMAKE_CURRENT_BINARY_DIR}) # for mnks_export.h

target_link_libraries(mnks PRIVATE mnk)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET mnks PROPERTY CXX_STANDARD 17)
endif()

##########################
########## main ##########
##########################

add_executable(main
    main.cpp
    Application.cpp
    Configuration.cpp
    GameClient.cpp
    ServerForFrontend.cpp
)
target_link_libraries(main PRIVATE mnk)

set_target_properties(main PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../bin"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET main PROPERTY CXX_STANDARD 17)
endif()

############################
######### httplib ##########
############################

FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.15.3
)

FetchContent_MakeAvailable(httplib)

target_include_directories(mnk PUBLIC ${httplib_SOURCE_DIR})
target_link_libraries(mnk PUBLIC httplib::httplib)

############################
###### nlohmann/json #######
############################

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)
target_link_libraries(mnk PUBLIC nlohmann_json::nlohmann_json)

############################
########### cpr ############
############################

FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)
target_link_libraries(mnk PUBLIC cpr::cpr)

############################
######### libenvpp #########
############################

FetchContent_Declare(
    libenvpp
    GIT_REPOSITORY https://github.com/ph3at/libenvpp.git
    GIT_TAG v1.4.1
)
FetchContent_MakeAvailable(libenvpp)
target_link_libraries(mnk PUBLIC libenvpp::libenvpp)

############################
########## dotenv ##########
############################

FetchContent_Declare(
    dotenv
    GIT_REPOSITORY https://github.com/laserpants/dotenv-cpp.git
    GIT_TAG 432def2fb9c2a5e51e6d491f35f2e8f35fa900b0
)
FetchContent_MakeAvailable(dotenv)
target_link_libraries(mnk PUBLIC dotenv)
