cmake_minimum_required(VERSION 3.10)
project(sql-injection-tester)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
add_executable(sql-injection-tester main.cpp)
target_link_libraries(sql-injection-tester ${CURL_LIBRARIES})

install(TARGETS sql-injection-tester DESTINATION bin)

include(GNUInstallDirs)
set(CMAKE_INSTALL_PREFIX "/usr/local")

include(CPack)
