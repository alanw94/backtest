#ifndef bt_PositionStats_hpp
#define bt_PositionStats_hpp

#include <bt_utils.hpp>
#include <cmath>

namespace bt {

class PositionStats
{
public:
  PositionStats()
  : maxDrawDownPercent(0.0), movingPeak(0.0)
  {}

  virtual ~PositionStats(){}

  void add_balance(float balance)
  {
    movingPeak = std::max(movingPeak, balance);
    maxDrawDownPercent = std::max(maxDrawDownPercent,
                                  percent_less(movingPeak, balance));
  }

  float get_max_drawdown_percent() const { return maxDrawDownPercent; }

private:
  float maxDrawDownPercent = 0.0;
  float movingPeak = 0.0;
};

} // namespace bt
#endif

