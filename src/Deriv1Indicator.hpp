#ifndef bt_Deriv1Indicator_hpp
#define bt_Deriv1Indicator_hpp

#include <PriceConsumer.hpp>
#include <SMAComputer.hpp>

namespace bt {

class Deriv1Indicator : public PriceConsumer
{
public:
  Deriv1Indicator(const std::string& name, unsigned widthDx);

  virtual ~Deriv1Indicator(){}

  void process_daily_prices(float open, float low,
                            float high, float close) override;

  float get_value() const override { return m_deriv1; }

private:
  float m_deriv1;
  unsigned m_widthDx;
  SMAComputer m_sma;
  SMAComputer m_smaHistory;
};

} // namespace bt
#endif
