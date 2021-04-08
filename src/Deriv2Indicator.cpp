#include <Deriv2Indicator.hpp>
#include <bt_require.hpp>

namespace bt {

Deriv2Indicator::Deriv2Indicator(const std::string& name,
                                 unsigned widthDx)
: PriceConsumer(name), m_deriv2(0.0), m_widthDx(widthDx),
  m_sma(widthDx), m_smaHistory(widthDx)
{
  bt_require(widthDx >= 3, "Deriv2Indicator needs widthDx >= 3.");
}

void Deriv2Indicator::process_daily_prices(float /*open*/, float /*low*/,
                                           float /*high*/, float close)
{
  m_sma.add_value(close);
  float sma = m_sma.get_average();
  m_smaHistory.add_value(sma);
  unsigned numP = m_sma.get_num_periods();
  if (numP < 3) {
    m_deriv2 = 0.0;
    return;
  }
  unsigned numIntervals = numP - 1;
  unsigned halfDx = numIntervals/2;
  float mid = m_smaHistory.get_value_at(halfDx);
  float oldest = m_smaHistory.get_oldest();
  float dy1 = sma - mid;
  float dy2 = mid - oldest;
  m_deriv2 = halfDx==0 ? 0.0 : ((dy1/halfDx)-(dy2/(numIntervals-halfDx)));
//  std::cout<<"d2i close="<<close<<" sma="<<sma
//     <<" oldest="<<oldest<<" mid="<<mid
//     <<" numIntervals="<<numIntervals<<", halfDx="<<halfDx
//     <<" dy1="<<dy1<<", dy2="<<dy2
//     <<", m_deriv2="<<m_deriv2<<std::endl;
}

} // namespace bt
