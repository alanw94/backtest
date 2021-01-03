#include <gtest/gtest.h>

#include <bt_utils.hpp>
#include <PositionStats.hpp>

const float tol = 1.e-4;

TEST(position_stats, max_drawdown)
{
  bt::PositionStats posStats;

  posStats.add_balance(10.0);
  posStats.add_balance(5.0);
  posStats.add_balance(15.0);

  EXPECT_NEAR(50.0, posStats.get_max_drawdown_percent(), tol);
}

TEST(position_stats, max_drawdown2)
{
  bt::PositionStats posStats;

  posStats.add_balance(100.0);
  posStats.add_balance(98.0);
  posStats.add_balance(105.0);
  posStats.add_balance(97.0);
  posStats.add_balance(125.0);
  posStats.add_balance(100.0);

  EXPECT_NEAR(20.0, posStats.get_max_drawdown_percent(), tol);
}

