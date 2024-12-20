cmake_minimum_required(VERSION 3.10)
project(sql-injection-tester)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)
find_package(GTest REQUIRED)

include_directories(${CURL_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(include)

add_executable(sql-injection-tester
        src/main.cpp
        src/sql_injection_tester.cpp
        src/utils.cpp
)

target_link_libraries(sql-injection-tester ${CURL_LIBRARIES})

add_executable(sql-injection-tester-tests
        tests/sql_injection_tester_tests.cpp
        src/sql_injection_tester.cpp
        src/utils.cpp
)

target_link_libraries(sql-injection-tester-tests ${CURL_LIBRARIES} ${GTEST_LIBRARIES} pthread)

add_custom_target(run-tests
        COMMAND sql-injection-tester-tests
        DEPENDS sql-injection-tester-tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_dependencies(sql-injection-tester run-tests)

install(TARGETS sql-injection-tester DESTINATION bin)

install(DIRECTORY files DESTINATION /var/lib/sql-injection-tester)
install(DIRECTORY examples DESTINATION /var/lib/sql-injection-tester)

include(GNUInstallDirs)
set(CMAKE_INSTALL_PREFIX "/usr/local")

include(CPack)
