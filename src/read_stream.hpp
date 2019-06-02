#ifndef bt_read_stream_hpp
#define bt_read_stream_hpp

#include "bt_utils.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

namespace bt {

std::string get_substr_at_index(const std::string& line,
                                char separator, unsigned index);
float get_price(const std::string& line, ColumnIndex whichColumn);
std::vector<std::string> get_dates(std::istream& ss);
std::vector<float> get_prices(std::istream& ss, ColumnIndex whichColumn);

inline
std::vector<std::string> read_dates(const std::string& filename)
{
  std::ifstream file(filename);
  return bt::get_dates(file);
}

inline
std::vector<float> read_prices(const std::string& filename, bt::ColumnIndex whichPrice)
{
  std::ifstream file(filename);
  return bt::get_prices(file, whichPrice);
}

bool read_data(const std::string& filename, DataSet& dataSet);

} // namespace bt

#endif

