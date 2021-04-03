#include <gtest/gtest.h>

#include <read_stream.hpp>
#include <analyze_prices.hpp>
#include <bt_require.hpp>
#include "utest_utils.hpp"

#include <iostream>
#include <sstream>
#include <limits>
#include <cmath>

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

TEST(analyze_prices, compute_2nd_derivative)
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

std::vector<float> fill_sin(unsigned N)
{
  std::vector<float> vals(N);
  const float twoPI = 2*std::acos(-1.0);
  bt_require(N>1, "fill_sin: N must be > 1.");
  for(unsigned i=0; i<N; ++i)
  {
    vals[i] = std::sin((i*twoPI)/(N-1));
  }
  return vals;
}

unsigned find_min_abs_loc(const std::vector<float>& v)
{
  float minVal = std::numeric_limits<float>::max();
  unsigned minLoc = std::numeric_limits<unsigned>::max();
  //skip end points.
  for(unsigned i=1; i<v.size()-1; ++i) {
    if (std::abs(v[i]) < minVal) {
      minVal = std::abs(v[i]);
      minLoc = i;
    }
  }
  return minLoc;
}

unsigned find_max_abs_loc(const std::vector<float>& v)
{
  float maxVal = std::numeric_limits<float>::min();
  unsigned maxLoc = std::numeric_limits<unsigned>::min();
  for(unsigned i=0; i<v.size(); ++i) {
    if (std::abs(v[i]) > maxVal) {
      maxVal = std::abs(v[i]);
      maxLoc = i;
    }
  }
  return maxLoc;
}

TEST(analyze_prices, compute_2nd_derivative_sin)
{
  const unsigned N = 1000;
  const float tolerance = 5.e-3;
  std::vector<float> vals = fill_sin(N);
  for(unsigned widthDx : {2, 4, 6, 8}) {
    std::vector<float> derivs = bt::compute_second_derivative(vals, widthDx);
    EXPECT_NEAR(0.0, derivs[0], 1.e-12);
    ASSERT_FALSE(derivs.empty());
    EXPECT_NEAR(0.0, derivs[derivs.size()-1], 1.e-12);

    unsigned minLoc = find_min_abs_loc(derivs);
    EXPECT_NEAR(0.0, vals[minLoc], tolerance)<<"widthDx="<<widthDx;
    unsigned maxLoc = find_max_abs_loc(derivs);
    EXPECT_NEAR(1.0, std::abs(vals[maxLoc]), tolerance)<<"widthDx="<<widthDx;
  }
}
