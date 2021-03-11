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

TEST(analyze_prices, compute_simple_moving_average)
{
  std::vector<float> prices = {1.0, 3.0, 4.0, 2.0, 1.0, 3.0};
  const unsigned numSMAPeriods = 4;
  std::vector<float> expectedSma4
                            = {1.0, 2.0, 2.6667, 2.5, 2.5, 2.5};
  std::vector<float> sma4 = bt::compute_sma(prices, numSMAPeriods);

  ASSERT_EQ(expectedSma4.size(), sma4.size());
  for(unsigned i=0; i<expectedSma4.size(); ++i) {
    EXPECT_NEAR(expectedSma4[i], sma4[i], tol);
  }
}

TEST(analyze_prices, compute_1st_derivative)
{
  std::vector<float> vals = {1.0, 3.0, 5.0, 2.0, 6.0};
  unsigned dxWidth = 2;
  std::vector<float> derivs = bt::compute_first_derivative(vals, dxWidth);
  std::vector<float> expectedDerivs
                          = {0.0, 2.0, 2.0, -0.5, 0.5};

  EXPECT_EQ(expectedDerivs.size(), derivs.size());

  for(unsigned i=0; i<derivs.size(); ++i) {
    EXPECT_NEAR(expectedDerivs[i], derivs[i], tol);
  }
}

TEST(trade, compute_2nd_derivative)
{
  std::vector<float> vals = {1.0, 3.0, 3.0, 2.0};
  unsigned widthDx = 2;
  std::vector<float> derivs = bt::compute_second_derivative(vals,widthDx);
  std::vector<float> expectedDerivs
                          = {0.0, -1.0, -0.5, 0.0};

  EXPECT_EQ(expectedDerivs.size(), derivs.size());

  for(unsigned i=0; i<derivs.size(); ++i) {
    EXPECT_NEAR(expectedDerivs[i], derivs[i], tol);
  }
}
