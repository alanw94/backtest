#include "read_stream.hpp"
#include "bt_utils.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

namespace bt {

std::string get_substr_at_index(const std::string& line, char separator, unsigned index)
{
  std::string::size_type pos = line.find(separator);
  if (index == 0) {
    return line.substr(0, pos);
  }

  unsigned counter = 1;
  while(counter < index)
  {
    pos += 1 + line.substr(pos+1).find(separator);
    ++counter;
  }
  std::string str = line.substr(pos+1);
  return str;
}

std::string get_string(const std::string& line, ColumnIndex whichColumn)
{
  return get_substr_at_index(line, ',', whichColumn);
}

float get_price(const std::string& line, ColumnIndex whichColumn)
{
  std::stringstream ss;
  ss<<get_substr_at_index(line, ',', whichColumn);
  float price = 0.0;
  ss>>price;
  return price;
}

std::vector<std::string> get_dates(std::istream& ss)
{
  std::vector<std::string> dates;
  std::string line;
  while(std::getline(ss, line)) {
    if (line.substr(0,4) == "Date")
    {
      continue;
    }
    dates.push_back(get_string(line, bt::Date));
  }
  return dates;
}

std::vector<float> get_prices(std::istream& ss, ColumnIndex whichColumn)
{
  std::vector<float> prices;
  std::string line;
  while(std::getline(ss, line)) {
    if (line.substr(0,4) == "Date")
    {
      continue;
    }
    prices.push_back(get_price(line, whichColumn));
  }
  return prices;
}

bool read_data(const std::string& filename, DataSet& dataSet)
{
  dataSet.dates = bt::read_dates(filename);
  dataSet.openingPrices = bt::read_prices(filename, bt::OpeningPrice);
  dataSet.highPrices = bt::read_prices(filename, bt::HighPrice);
  dataSet.lowPrices = bt::read_prices(filename, bt::LowPrice);
  dataSet.closingPrices = bt::read_prices(filename, bt::ClosingPrice);

  if (dataSet.dates.empty() ||
      dataSet.dates.size() != dataSet.openingPrices.size() || dataSet.dates.size() != dataSet.closingPrices.size()) {
    std::cout << "Trouble reading "<<filename<<", inconsistent sizes." << std::endl;
    return false;
  }
  return true;
}

} // namespace bt

