cmake_minimum_required(VERSION 3.14)

project(Server CXX)
MESSAGE("=> Star of project's build")

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

include_directories(Server/include)
include_directories(Error)
# include_directories(${SFML_INCLUDE_DIR})

set(PROJECTSRC
    Server/src/Server.cpp
    Error/Error.cpp
)

# set(PROJECTHEADER
#     include/Server.hpp
# )

set(CMAKE_CXX_FLAGS "-std=c++17 -W -Wall -Wextra -Werror -g3")

add_executable(Server ${PROJECTSRC})
# target_link_libraries(bsRtype sfml-window sfml-graphics sfml-audio)

add_custom_target(fclean
    COMMENT "=> cleaning project's folder"
    COMMAND make clean
    COMMAND rm -f ${bsRtype}
)

add_custom_target(re
    COMMENT "=> rebuilding project"
    COMMAND make fclean all
)

add_custom_target(CMClean
    COMMENT "=> cleaning cmake tmp directories"
    COMMAND rm -rf CMAkeFiles
    COMMAND rm cmake_install.cmake
    COMMAND rm CMakeCache.txt
)