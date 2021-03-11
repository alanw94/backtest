#include "analyze_prices.hpp"
#include "bt_utils.hpp"
#include "bt_require.hpp"

#include <iostream>
#include <cmath>
#include <cstdlib>

namespace bt {

PriceSummary get_price_summary(const std::vector<float>& prices)
{
  PriceSummary summary = {prices.size(), max_price, min_price};

  for(float price : prices) {
    summary.low = std::min(summary.low, price);
    summary.high = std::max(summary.high, price);
  }

  return summary;
}

float sum(const std::vector<float>& prices, unsigned start, unsigned numVals)
{
  float result = 0.0;
  for(unsigned i=start; i<start+numVals; ++i) {
    result += prices[i];
  }
  return result;
}

class SmaComputer {
public:
  SmaComputer(unsigned numPeriods)
  : m_data(numPeriods, 0.0),
    m_curIdx(0),
    m_startingUp(true)
  {}

  void add_value(float val)
  {
    m_data[m_curIdx++] = val;
    if (m_curIdx >= m_data.size()) {
      m_curIdx = 0;
      if (m_startingUp) {
        m_startingUp = false;
      }
    }
  }

  float get_average() const
  {
    unsigned num = m_startingUp ? m_curIdx : m_data.size();
    return num > 0 ? sum(m_data, 0, num)/num : 0.0;
  }

private:
  std::vector<float> m_data;
  unsigned m_curIdx;
  bool m_startingUp;
};

std::vector<float> compute_sma(const std::vector<float>& prices, unsigned numPeriods)
{
  std::vector<float> sma;
  SmaComputer smaComputer(numPeriods);

  sma.reserve(prices.size());

  for(float price : prices) {
    smaComputer.add_value(price);
    sma.push_back(smaComputer.get_average());
  }
  return sma;
}

float deriv1(const std::vector<float>& vals, unsigned i, unsigned dx)
{
  bt_require(i<vals.size(), "deriv1: i out of range ("<<i<<", vals.size()="<<vals.size()<<")");
  unsigned denom = std::min(i, dx);
  float numer = vals[i]-vals[i-denom];
  return denom==0 ? 0 : numer/denom;
}

std::vector<float> compute_first_derivative(const std::vector<float>& vals,
                                            unsigned widthDx)
{
  std::vector<float> derivs(vals.size());
  for(unsigned i=0; i<vals.size(); ++i) {
    derivs[i] = deriv1(vals, i, widthDx);
  }
  return derivs;
}

std::vector<float> compute_second_derivative(const std::vector<float>& vals,
                                             unsigned widthDx)
{
  std::vector<float> derivs(vals.size());
  unsigned halfDx = widthDx/2;
  const unsigned len = vals.size();
  for(unsigned i=0; i<len; ++i) {
    unsigned x2minusx1 = std::min(i, halfDx);
    unsigned x3minusx2 = std::min(halfDx, len - i - 1);
    if (x2minusx1 == 0 || x3minusx2 == 0) {
      derivs[i] = 0.0; continue;
    }
    float d1 = deriv1(vals, i, x2minusx1);
    unsigned i2 = std::min(i+halfDx, len-1);
    float d2 = deriv1(vals, i2, x3minusx2);
    derivs[i] = (d2-d1)/(x3minusx2+x2minusx1);
  }
  return derivs;
}

Histogram
compute_percentage_histogram(const std::vector<float>& open,
                             const std::vector<float>& close)
{
  Histogram hist;
  if (open.size() != close.size()) {
    std::cout << "Error, open and close are different lengths."<<std::endl;
    return hist;
  }

  for(size_t i=0; i<open.size(); ++i) {
    float percentGain = 100.0*((close[i]-open[i])/open[i]);
    float followedByPercentGain = 0.0;
    if (i < open.size()-1) {
      followedByPercentGain = 100.0*((close[i+1]-open[i+1])/open[i+1]);
    }
    int followedByUp = followedByPercentGain > 0.0 ? 1 : 0;
    int followedByDown = 1 - followedByUp;
    int intPercent = static_cast<int>(std::floor(std::abs(percentGain)));
    if (percentGain < 0.0) {
      intPercent = -intPercent;
    }
    std::string str = std::to_string(intPercent);
    if (str == "0" && percentGain < 0.0) {
      str = "-0";
    }
    Stats stats = {1, followedByUp, followedByDown};
    Histogram::iterator iter = hist.find(str);
    if (iter == hist.end()) {
      hist.insert(std::make_pair(str, stats));
    }
    else {
      iter->second += stats;
    }
  }

  return hist;
}

} // namespace bt

