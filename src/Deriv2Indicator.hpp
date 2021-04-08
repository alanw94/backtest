#ifndef bt_Deriv2Indicator_hpp
#define bt_Deriv2Indicator_hpp

#include <PriceConsumer.hpp>
#include <SMAComputer.hpp>

namespace bt {

class Deriv2Indicator : public PriceConsumer
{
public:
  Deriv2Indicator(const std::string& name, unsigned widthDx);

  virtual ~Deriv2Indicator(){}

  void process_daily_prices(float open, float low,
                            float high, float close) override;

  float get_value() const override { return m_deriv2; }

private:
  float m_deriv2;
  unsigned m_widthDx;
  SMAComputer m_sma;
  SMAComputer m_smaHistory;
};

} // namespace bt
#endif
