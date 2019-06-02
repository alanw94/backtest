#ifndef bt_analyze_prices_hpp
#define bt_analyze_prices_hpp

#include "read_stream.hpp"

#include <vector>
#include <map>
#include <string>
#include <sstream>

namespace bt {

constexpr float min_price = 0.0;
constexpr float max_price = 999999.99;

struct PriceSummary {
  size_t numPrices;
  float low = 0.0;
  float high = 0.0;
};

struct ComputedData {
  std::vector<float> sma;
  std::vector<float> first_derivs;
  std::vector<float> second_derivs;
  bool empty() const { return sma.empty(); }
};

PriceSummary get_price_summary(const std::vector<float>& prices);

std::vector<float> compute_sma(const std::vector<float>& prices, unsigned numPeriods);

std::vector<float> compute_first_derivative(const std::vector<float>& vals);
std::vector<float> compute_second_derivative(const std::vector<float>& vals);

inline
void compute_data(const bt::DataSet& dataSet, unsigned smaPeriods,
                  ComputedData& computed)
{
    computed.sma = bt::compute_sma(dataSet.closingPrices, smaPeriods);
    computed.first_derivs = bt::compute_first_derivative(computed.sma);
    computed.second_derivs = bt::compute_second_derivative(computed.sma);
}

struct StrIntLess {
template<typename StringType1, typename StringType2>
bool operator()(const StringType1& str1, const StringType2& str2) const
{
  float i1;
  std::stringstream ss1;
  ss1 << str1;
  ss1 >> i1;
  if (str1 == "-0") {
    i1 = -0.1;
  }
  float i2;
  std::stringstream ss2;
  ss2 << str2;
  ss2 >> i2;
  if (str2 == "-0") {
    i2 = -0.1;
  }
  return i1 < i2;
}
};

struct Stats {
  int numDays;
  int followedByUp;
  int followedByDown;

  Stats& operator+=(const Stats& rhs) {
    numDays += rhs.numDays;
    followedByUp += rhs.followedByUp;
    followedByDown += rhs.followedByDown;
    return *this;
  }
};

typedef std::map<std::string, Stats, StrIntLess> Histogram;
Histogram
compute_percentage_histogram(const std::vector<float>& open,
                             const std::vector<float>& close);

} // namespace bt

#endif

