#include <gtest/gtest.h>

#include <read_stream.hpp>
#include <analyze_prices.hpp>

#include "utest_utils.hpp"

#include <iostream>
#include <sstream>

constexpr float tol = 1.e-4;

TEST(trade, get_low_high)
{
  std::stringstream ss;
  initialize_test_data(ss);
  std::vector<float> prices = bt::get_prices(ss, bt::ClosingPrice);

  bt::PriceSummary priceSummary = bt::get_price_summary(prices);

  EXPECT_NEAR(39.896667, priceSummary.low, tol);
  EXPECT_NEAR(40.759998, priceSummary.high, tol);
}

TEST(trade, compute_simple_moving_average)
{
  std::vector<float> prices = {1.0, 3.0, 4.0, 2.0, 1.0, 3.0 };
  unsigned numSMAPeriods = 4;
  std::vector<float> sma4 = bt::compute_sma(prices, numSMAPeriods);

  unsigned expectedSize = prices.size() - (numSMAPeriods-1);
  EXPECT_EQ(expectedSize, sma4.size());
  float expectedSmaVal = 2.5;
  for(float smaVal : sma4) {
    EXPECT_NEAR(expectedSmaVal, smaVal, tol);
  }
}

TEST(trade, compute_sma_and_align_arrays)
{
  std::vector<float> prices = {1.0, 3.0, 4.0, 2.0, 1.0, 3.0 };
  unsigned numSMAPeriods = 4;
  std::vector<float> sma4 = bt::compute_sma(prices, numSMAPeriods);

  unsigned oldPricesLength = prices.size();
  bt::align_prices_with_sma(numSMAPeriods, sma4, prices);

  unsigned expectedPricesLength = oldPricesLength - (numSMAPeriods-1);
  EXPECT_EQ(expectedPricesLength, prices.size());
  EXPECT_EQ(sma4.size(), prices.size());
}


TEST(trade, compute_derivative1)
{
  std::vector<float> vals = {1.0, 3.0, 3.0, 2.0};
  std::vector<float> derivs = bt::compute_first_derivative(vals);
  std::vector<float> expectedDerivs = {2.0, 0.0, -1.0};

  EXPECT_EQ(expectedDerivs.size(), derivs.size());

  for(unsigned i=0; i<derivs.size(); ++i) {
    EXPECT_NEAR(expectedDerivs[i], derivs[i], tol);
  }
}

TEST(trade, compute_derivative2)
{
  std::vector<float> vals = {1.0, 3.0, 3.0, 2.0};
  std::vector<float> derivs = bt::compute_second_derivative(vals);
  std::vector<float> expectedDerivs = {-2.0, -1.0};

  EXPECT_EQ(expectedDerivs.size(), derivs.size());

  for(unsigned i=0; i<derivs.size(); ++i) {
    EXPECT_NEAR(expectedDerivs[i], derivs[i], tol);
  }
}

