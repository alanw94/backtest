list(APPEND SOURCES
	Deriv1Indicator.cpp
	Position.cpp
	analyze_prices.cpp
	bt_utils.cpp
	cmdline_parser.cpp
	read_stream.cpp
	)

add_library(backtest ${SOURCES})

include_directories(${PROJECT_SOURCE_DIR}/src)

