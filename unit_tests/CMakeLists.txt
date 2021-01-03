file(GLOB SOURCES *.cpp)

add_executable(unittests.exe ${SOURCES})

target_link_libraries(unittests.exe PUBLIC
       backtest
       GTest::Main
       GTest::GTest
       )

add_test(NAME "Unit-tests" COMMAND unittests.exe)

include_directories(
	"${PROJECT_SOURCE_DIR}/src"
	"${GTEST_INCLUDE_DIRS}"
	)

