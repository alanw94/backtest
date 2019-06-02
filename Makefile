# Points to the root of Google Test
GTEST_DIR = ../googletest/googletest

# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include -isystem src

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -Werror -pthread

LIB = libbacktest.a

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS = unittest.exe

BT_EXEC = backtest.exe

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

all : $(LIB) $(TESTS) $(BT_EXEC)

clean :
	rm -f $(BT_EXEC) $(TESTS) *.o *.a

cmdline_parser.o : src/cmdline_parser.cpp src/cmdline_parser.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c src/cmdline_parser.cpp

analyze_prices.o : src/analyze_prices.cpp src/analyze_prices.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c src/analyze_prices.cpp

read_stream.o : src/read_stream.cpp src/read_stream.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c src/read_stream.cpp

Position.o : src/Position.cpp src/Position.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c src/Position.cpp

bt_utils.o : src/bt_utils.cpp src/bt_utils.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c src/bt_utils.cpp

libbacktest.a : analyze_prices.o read_stream.o Position.o bt_utils.o cmdline_parser.o
	$(AR) $(ARFLAGS) $@ $^

utest_utils.o : unit_tests/utest_utils.cpp $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c unit_tests/utest_utils.cpp

utest_cmdline_parser.o : unit_tests/utest_cmdline_parser.cpp $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c unit_tests/utest_cmdline_parser.cpp

utest_read_stream.o : unit_tests/utest_read_stream.cpp $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c unit_tests/utest_read_stream.cpp

utest_analyze_prices.o : unit_tests/utest_analyze_prices.cpp $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c unit_tests/utest_analyze_prices.cpp

utest_position.o : unit_tests/utest_position.cpp src/Position.hpp $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c unit_tests/utest_position.cpp

utest_bt_utils.o : src/bt_utils.hpp unit_tests/utest_bt_utils.cpp $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c unit_tests/utest_bt_utils.cpp

unittest.exe : utest_utils.o utest_read_stream.o utest_analyze_prices.o utest_position.o utest_bt_utils.o utest_cmdline_parser.o libbacktest.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(GTEST_DIR)/make/gtest_main.a $^ -lpthread -o $@

backtest.exe : src/Position.hpp src/bt_utils.hpp backtest.cpp libbacktest.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) backtest.cpp -L. -lbacktest -lpthread -o $@

