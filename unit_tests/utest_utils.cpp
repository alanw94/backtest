#include <sstream>

void initialize_test_data(std::stringstream& ss)
{
  ss << "Date,Open,High,Low,Close,Adj Close,Volume" << std::endl;
  ss << "2017-10-12,39.943333,40.360001,39.753334,39.896667,39.896667,8272500" << std::endl;
  ss << "2017-10-13,40.356667,40.480000,40.216667,40.343334,40.343334,6109200" << std::endl;
  ss << "2017-10-16,40.646667,40.779999,40.376667,40.759998,40.759998,7134900" << std::endl;
}

