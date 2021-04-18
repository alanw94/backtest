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

TEST(analyze_prices, get_price_summary)
{
  std::stringstream ss;
  initialize_test_data(ss);
  std::vector<float> prices = bt::get_prices(ss, bt::ClosingPrice);

  bt::PriceSummary priceSummary = bt::get_price_summary(prices);

  EXPECT_NEAR(39.896667, priceSummary.low, tol);
  EXPECT_NEAR(40.759998, priceSummary.high, tol);
}
