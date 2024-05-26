cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main
    main.cpp
    Agent.cpp
    Application.cpp
    BoardQuery.cpp
    Configuration.cpp
    GameClient.cpp
    ServerForFrontend.cpp
)
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

set_target_properties(main PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../bin"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET main PROPERTY CXX_STANDARD 17)
endif()

include(FetchContent)
include(ExternalProject)

############################
######### httplib ##########
############################

FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.15.3
)

FetchContent_MakeAvailable(httplib)

target_include_directories(main PRIVATE ${httplib_SOURCE_DIR})
target_link_libraries(main PRIVATE httplib::httplib)

############################
###### nlohmann/json #######
############################

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)
target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json)

############################
########### cpr ############
############################

FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)
target_link_libraries(main PRIVATE cpr::cpr)

############################
######### libenvpp #########
############################

FetchContent_Declare(
    libenvpp
    GIT_REPOSITORY https://github.com/ph3at/libenvpp.git
    GIT_TAG v1.4.1
)
FetchContent_MakeAvailable(libenvpp)
target_link_libraries(main PRIVATE libenvpp::libenvpp)

############################
########## dotenv ##########
############################

FetchContent_Declare(
    dotenv
    GIT_REPOSITORY https://github.com/laserpants/dotenv-cpp.git
    GIT_TAG 432def2fb9c2a5e51e6d491f35f2e8f35fa900b0
)
FetchContent_MakeAvailable(dotenv)
target_link_libraries(main PRIVATE dotenv)
