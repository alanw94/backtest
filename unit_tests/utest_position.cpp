#include <gtest/gtest.h>

#include <bt_utils.hpp>
#include <Position.hpp>

const float tol = 1.e-4;

void check_whether_invested(bt::Position& pos,
                            const std::vector<float>& prices,
                            const std::vector<bool>& expected)
{
  for(unsigned i=0; i<prices.size(); ++i) {
    pos.process_price(prices[i]);
    EXPECT_EQ(expected[i], pos.is_invested())<<"prices["<<i<<"]="<<prices[i];
  }
}

TEST(position, buyAndHold)
{
  bt::BuyAndHoldPosition pos("test");
  EXPECT_FALSE(pos.is_invested());

  float prevSMA = 100.0;
  pos.set_current_date("2019-05-26", prevSMA);
  std::vector<float> prices = {99.0, 78.0, 121.0, 100.0};

  check_whether_invested(pos, prices, {false, false, false, true});

  EXPECT_NEAR(prevSMA, pos.get_sma_at_previous_close(), tol);

  EXPECT_NEAR(prices[3], pos.get_previous_price(), tol);
  EXPECT_NEAR(10000.0, pos.get_balance(), tol);
  EXPECT_NEAR(100.0, pos.get_buy_price(), tol);
}

TEST(position, simpleMovingAverage)
{
  bt::SMAPosition pos;
  EXPECT_FALSE(pos.is_invested());

  float prevSMA = 100.0;
  pos.set_current_date("2019-05-26", prevSMA);
  std::vector<float> prices = {99.0, 98.0, 102.0, 101.0};

  check_whether_invested(pos, prices, {false, false, false, false});

  prevSMA = 99.0;
  prices = {100.0, 98.0, 105.0, 100.0};

  pos.set_current_date("2019-05-27", prevSMA);

  check_whether_invested(pos, prices, {true, true, true, true});
  EXPECT_NEAR(10000.0, pos.get_balance(), tol);

  prevSMA = 101.0;
  prices = {98.0, 88.0, 104.0, 98.0};

  pos.set_current_date("2019-05-28", prevSMA);

  pos.process_price(prices[0]);
  EXPECT_FALSE(pos.is_invested());
  EXPECT_NEAR(9800.0, pos.get_balance(), tol);

  pos.process_price(prices[1]);
  EXPECT_FALSE(pos.is_invested());

  pos.process_price(prices[2]);
  EXPECT_FALSE(pos.is_invested());

  pos.process_price(prices[3]);
  EXPECT_FALSE(pos.is_invested());
  EXPECT_NEAR(9800.0, pos.get_balance(), tol);
}

TEST(position, trailingStopPercentage_buy_sell)
{
  const float trailPercent = 2.0;
  bt::TSPPosition pos(trailPercent);
  EXPECT_FALSE(pos.is_invested());

  float prevSMA = 100.0;
  pos.set_current_date("2019-05-26", prevSMA);

  std::vector<float> prices = {99.0, 78.0, 121.0, 100.0};

  check_whether_invested(pos, prices, {false, false, true, false});

  EXPECT_NEAR(14877.50, pos.get_balance(), tol);
}

TEST(position, trailingStopPercentage_buy_sell_buy)
{
  const float trailPercent = 2.0;
  bt::TSPPosition pos(trailPercent);
  EXPECT_FALSE(pos.is_invested());

  float prevSMA = 100.0;
  pos.set_current_date("2019-05-26", prevSMA);

  std::vector<float> prices = {100.0, 103.0, 98.0, 105.0};

  //for these prices should be at 102 (100 + 2%)
  //and then sell at 100.94 (103 - 2%).
  check_whether_invested(pos, prices, {false, true, false, false});

  //can't buy until 2 days after selling:
  pos.set_current_date("2019-05-27", prevSMA);
  check_whether_invested(pos, prices, {false, false, false, false});

  prices = {98.0, 98.0, 105.0, 105.0};
  pos.set_current_date("2019-05-28", prevSMA);
  //now should buy at 99.96 (98 + 2%)
  check_whether_invested(pos, prices, {false, false, true, true});

  //balance calculation:
  //should have bought 98 shares at 102, then sold at 100.94,
  //so loss of 1.06*98 = 103.88.
  //then bought 99 shares at 99.96, and final price is 105,
  //so profit of 5.04*99 = 498.96.
  //So final balance = 10000 - 103.88 + 498.96 = 10395.08
  EXPECT_NEAR(10395.08, pos.get_balance(), tol);
}

