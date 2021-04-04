#ifndef bt_PriceConsumer_hpp
#define bt_PriceConsumer_hpp

#include <bt_utils.hpp>
#include <cmath>

namespace bt {

class PriceConsumer
{
public:
  PriceConsumer(const std::string& name)
  : m_name(name)
  {}

  virtual ~PriceConsumer(){}

  const std::string& get_name() const { return m_name; }

  virtual void set_current_date(const std::string& /*date*/,
                                float /*smaAtPreviousClose*/)
  {}

  virtual void process_daily_prices(float open, float low,
                                    float high, float close) = 0;

  virtual float get_value() const = 0;

private:
  std::string m_name;
};

} // namespace bt
#endif
