#include <gtest/gtest.h>

#include <Deriv1Indicator.hpp>
#include <IndicatorObserver.hpp>
#include <bt_require.hpp>

#include <iostream>
#include <sstream>
#include <limits>
#include <cmath>

constexpr float tol = 1.e-4;

class TestIndicator : public bt::PriceConsumer
{
public:

};

TEST(TestObservers, basic)
{
  std::vector<float> prices = {1.0, 3.0, 4.0, 2.0, 1.0, 3.0};
  const unsigned numSMAPeriods = 4;
  std::vector<float> expectedSma4
                            = {1.0, 2.0, 2.6667, 2.5, 2.5, 2.5};
  bt::SMAIndicator smaIndicator("SMA", numSMAPeriods);

  for(unsigned i=0; i<expectedSma4.size(); ++i) {
    float open=0.0, low=0.0, high=0.0, close=prices[i];
    smaIndicator.process_daily_prices(open, low, high, close);
    EXPECT_NEAR(expectedSma4[i], smaIndicator.get_value(), tol);
  }
}
