#ifndef bt_SMAIndicator_hpp
#define bt_SMAIndicator_hpp

#include <PriceConsumer.hpp>
#include <SMAComputer.hpp>

namespace bt {

class SMAIndicator : public PriceConsumer
{
public:
  SMAIndicator(const std::string& name, unsigned numPeriods)
  : PriceConsumer(name), m_sma(numPeriods)
  {}

  virtual ~SMAIndicator(){}

  void process_daily_prices(float /*open*/, float /*low*/,
                            float /*high*/, float close) override
  { m_sma.add_value(close); }

  float get_value() const override
  { return m_sma.get_average(); }

private:
  SMAComputer m_sma;
};

} // namespace bt
#endif
