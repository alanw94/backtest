#ifndef bt_utils_hpp
#define bt_utils_hpp

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>

namespace bt {

enum ColumnIndex {
  Date = 0,
  OpeningPrice,
  HighPrice,
  LowPrice,
  ClosingPrice,
  Adj_ClosingPrice,
  Volume
};

struct DataSet {
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
  return (std::pow((endBal/startBal), (1.0/numYears)) -1.0) *100;
}

inline
float percent_less(float baselinePrice, float newPrice)
{
    return (100.0*((baselinePrice - newPrice)/baselinePrice));
}

std::vector<float>
get_intraday_price_sequence(float open, float low,
                            float high, float close);

std::string get_path(const std::string& filename);

std::string get_basename(const std::string& filename);

std::string get_suffix(const std::string& filename);

std::string get_output_filename(const std::string& filename,
                             const std::string& insert);

std::string get_log_filename(const std::string& filename,
                             const std::string& insert);

std::ostream& out(const std::string& filename="");

template<typename T>
void erase_front(std::vector<T>& vec, unsigned numToErase)
{
  vec.erase(vec.begin(), vec.begin()+numToErase);
}

template<typename VecType>
void align_prices_with_sma(unsigned numSMAPeriods, const std::vector<float>& sma, VecType& prices)
{
  erase_front(prices, numSMAPeriods-1);
  if (prices.size() != sma.size()) {
    std::cerr<<"!!! ERROR, prices and sma arrays not sized consistently."<<std::endl;
    std::abort();
  }
}

void align_with_sma(DataSet& data,
                    const std::vector<float>& sma,
                    unsigned numSMAPeriods);

} // namespace bt

#endif

