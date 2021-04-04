#ifndef bt_utils_hpp
#define bt_utils_hpp

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstdint>

namespace bt {

enum ColumnIndex : uint8_t {
  Date = 0,
  OpeningPrice,
  HighPrice,
  LowPrice,
  ClosingPrice,
  Adj_ClosingPrice,
  Volume
};

struct DataSet {
  DataSet(const std::string& fileName);
  std::vector<std::string> dates;
  std::vector<float> openingPrices;
  std::vector<float> highPrices;
  std::vector<float> lowPrices;
  std::vector<float> closingPrices;
  bool empty() const { return dates.empty(); }
};

inline
float CAGR_percent(float startBal, float endBal, int numYears)
{
  return numYears > 0 ? (std::pow((endBal/startBal), (1.0/numYears)) -1.0) *100 : 0.0;
}

inline
float percent_less(float baselinePrice, float newPrice)
{
    return baselinePrice > 0.0 ? (100.0*((baselinePrice - newPrice)/baselinePrice)) : 0.0;
}

std::vector<float>
get_intraday_price_sequence(unsigned i, const DataSet& data);

std::string get_path(const std::string& filename);

std::string get_basename(const std::string& filename);

std::string get_suffix(const std::string& filename);

std::string get_output_filename(const std::string& filename,
                             const std::string& insert);

std::string get_log_filename(const std::string& filename,
                             const std::string& insert);

std::ostream& out(const std::string& filename="");

void print_data(std::ostream& os, unsigned i,
                const DataSet& data, float smaAtPreviousClose);

} // namespace bt

#endif

