#ifndef bt_SMAIndicator_hpp
#define bt_SMAIndicator_hpp

#include <PriceConsumer.hpp>
#include <SMAComputer.hpp>

namespace bt {

class SMAIndicator : public PriceConsumer
{
public:
  SMAIndicator(const std::string& name, unsigned numPeriods)
  : PriceConsumer(name), m_sma(numPeriods),
    m_balance(0.0), m_invested(true), m_numShares(1)
  {}

  SMAIndicator(const std::string& name, unsigned numPeriods,
               float balance)
  : PriceConsumer(name),
    m_sma(numPeriods), m_balance(balance),
    m_invested(false), m_numShares(0)
  {}

  virtual ~SMAIndicator(){}

  void process_daily_prices(float /*open*/, float /*low*/,
                            float /*high*/, float close) override
  {
    if (!m_invested) {
      m_numShares = static_cast<unsigned>(m_balance/close);
      m_balance -= m_numShares*close;
      m_invested = true;
    }
    float val = m_balance + m_numShares*close;
    m_sma.add_value(val);
  }

  float get_value() const override
  { return m_sma.get_average(); }

private:
  SMAComputer m_sma;
  float m_balance;
  bool m_invested;
  unsigned m_numShares;
};

} // namespace bt
#endif
