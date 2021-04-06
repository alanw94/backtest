#include <Deriv1Indicator.hpp>
#include <bt_require.hpp>

namespace bt {

Deriv1Indicator::Deriv1Indicator(const std::string& name,
                                 unsigned widthDx)
: PriceConsumer(name), m_deriv1(0.0), m_widthDx(widthDx),
  m_sma(widthDx), m_smaHistory(widthDx)
{
}

void Deriv1Indicator::process_daily_prices(float /*open*/, float /*low*/,
                                           float /*high*/, float close)
{
  unsigned denom = m_sma.get_num_periods();
  float oldest = m_smaHistory.get_oldest();
  m_sma.add_value(close);
  float sma = m_sma.get_average();
  m_smaHistory.add_value(sma);
  float numer = sma - oldest;
  m_deriv1 = denom==0 ? 0.0 : (numer/denom);
//  std::cout<<"d1i close="<<close<<" sma="<<sma
//     <<" oldest="<<oldest
//     <<" denom="<<denom<<", numer="<<numer
//     <<", m_deriv1="<<m_deriv1<<std::endl;
}

} // namespace bt
