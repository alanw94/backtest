#include <gtest/gtest.h>

#include <bt_utils.hpp>
#include <bt_require.hpp>
#include <Position.hpp>

const float tol = 1.e-4;

void check_whether_invested(bt::Position& pos,
                            const std::vector<float>& prices,
                            bool expected)
{
  bt_require(prices.size()==4, "Prices expected to be open,low,high,close");
  pos.process_daily_prices(prices[0],prices[1],prices[2],prices[3]);
  EXPECT_EQ(expected, pos.is_invested())<<"prices[3]="<<prices[3];
}

TEST(position, buyAndHold)
{
  bt::out("utest_position.buyAndHold.log");
  const float bal = 10000.0;
  bt::BuyAndHoldPosition pos("test", bal);
  EXPECT_FALSE(pos.is_invested());

  pos.set_current_date("2019-05-26");
  pos.process_daily_prices(99.0, 78.0, 121.0, 100.0);

  EXPECT_TRUE(pos.is_invested());

  EXPECT_NEAR(100.0, pos.get_previous_price(), tol);
  EXPECT_NEAR(10000.0, pos.get_balance(), tol);
  EXPECT_NEAR(100.0, pos.get_buy_price(), tol);
}

TEST(position, simpleMovingAverage)
{
  bt::out("utest_position.simpleMovingAverage.log");
  bt::SMAPosition pos(10000.0, 1);
  EXPECT_FALSE(pos.is_invested());

  pos.set_current_date("2019-05-26");
  std::vector<float> prices = {99.0, 98.0, 102.0, 99.0};

  check_whether_invested(pos, prices, false);
  EXPECT_EQ(99.0, pos.get_sma_at_previous_close());

  prices = {100.0, 98.0, 105.0, 100.0};

  pos.set_current_date("2019-05-27");

  check_whether_invested(pos, prices, true);
  EXPECT_NEAR(10000.0, pos.get_balance(), tol);
  EXPECT_EQ(100.0, pos.get_sma_at_previous_close());

  prices = {98.0, 88.0, 104.0, 98.0};

  pos.set_current_date("2019-05-28");

  check_whether_invested(pos, prices, false);
  EXPECT_FALSE(pos.is_invested());
  EXPECT_NEAR(9800.0, pos.get_balance(), tol);
}

TEST(position, trailingStopPercentage_buy_sell)
{
  bt::out("utest_position.trailingStopPercentage_buy_sell.log");
  const float trailPercent = 2.0;
  const float bal = 10000.0;
  bt::TSPPosition pos(bal, trailPercent);
  EXPECT_FALSE(pos.is_invested());

  pos.set_current_date("2019-05-26");

  std::vector<float> prices = {99.0, 78.0, 121.0, 100.0};

  check_whether_invested(pos, prices, false);

  EXPECT_NEAR(14877.50, pos.get_balance(), tol);
}

TEST(position, trailingStopPercentage_buy_sell_buy)
{
  bt::out("utest_position.trailingStopPercentage_buy_sell_buy.log");
  const float trailPercent = 2.0;
  const float bal = 10000.0;
  bt::TSPPosition pos(bal, trailPercent);
  EXPECT_FALSE(pos.is_invested());

  pos.set_current_date("2019-05-26");

  std::vector<float> prices = {100.0, 103.0, 98.0, 105.0};

  //for these prices should buy at 102 (100 + 2%)
  //and then sell at 100.94 (103 - 2%).
  check_whether_invested(pos, prices, false);

  //can't buy until 2 days after selling:
  pos.set_current_date("2019-05-27");
  check_whether_invested(pos, prices, false);

  prices = {98.0, 98.0, 105.0, 105.0};
  pos.set_current_date("2019-05-28");
  //now should buy at 99.96 (98 + 2%)
  check_whether_invested(pos, prices, true);

  //balance calculation:
  //should have bought 98 shares at 102, then sold at 100.94,
  //so loss of 1.06*98 = 103.88.
  //then bought 99 shares at 99.96, and final price is 105,
  //so profit of 5.04*99 = 498.96.
  //So final balance = 10000 - 103.88 + 498.96 = 10395.08
  EXPECT_NEAR(10395.08, pos.get_balance(), tol);
}
