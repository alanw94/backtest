#include <gtest/gtest.h>

#include <read_stream.hpp>
#include "utest_utils.hpp"

#include <iostream>
#include <sstream>

TEST(trade, get_price)
{
  std::string line("date1, 2.0");
  float price = bt::get_price(line, bt::OpeningPrice);
  EXPECT_NEAR( 2.0, price, 1.e-9);
}

TEST(trade, get_opening_prices)
{
  std::stringstream ss;
  initialize_test_data(ss);
  std::vector<float> prices = bt::get_prices(ss,bt::OpeningPrice);
  EXPECT_NEAR( 39.943333, prices[0], 1.e-4);
  EXPECT_NEAR( 40.356667, prices[1], 1.e-4);
  EXPECT_NEAR( 40.646667, prices[2], 1.e-4);
}

TEST(trade, read_stream)
{
  std::stringstream ss;
  initialize_test_data(ss);
  std::vector<float> prices = bt::get_prices(ss,bt::ClosingPrice);
  EXPECT_EQ(3u, prices.size());
  EXPECT_NEAR(39.896667, prices[0], 1.e-4);
  EXPECT_NEAR(40.343334, prices[1], 1.e-4);
  EXPECT_NEAR(40.759998, prices[2], 1.e-4);
}

