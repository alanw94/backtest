#include "analyze_prices.hpp"
#include "bt_utils.hpp"

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

std::vector<float> compute_sma(const std::vector<float>& prices, unsigned numPeriods)
{
  std::vector<float> sma;
  if (prices.size() < numPeriods) {
    return sma;
  }

  sma.reserve(prices.size() - (numPeriods - 1));

  for(unsigned i=numPeriods-1; i<prices.size(); ++i) {
    sma.push_back(sum(prices, i-(numPeriods-1), numPeriods)/numPeriods);
  }
  return sma;
}

std::vector<float> compute_first_derivative(const std::vector<float>& vals)
{
  std::vector<float> derivs(vals.size() - 1);
  for(unsigned i=1; i<vals.size(); ++i) {
    derivs[i-1] = vals[i] - vals[i-1];
  }
  return derivs;
}

std::vector<float> compute_second_derivative(const std::vector<float>& vals)
{
  std::vector<float> derivs(vals.size() - 2);
  for(unsigned i=2; i<vals.size(); ++i) {
    derivs[i-2] = (vals[i] - vals[i-1]) - (vals[i-1] - vals[i-2]);
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

