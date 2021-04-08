#include <Position.hpp>
#include <analyze_prices.hpp>
#include <bt_utils.hpp>

#include <cmath>
#include <iomanip>

namespace bt {

float force_into_range(float price, float lowLimit, float highLimit)
{
  if (lowLimit > highLimit) {
    std::swap(lowLimit, highLimit);
  }

  if (price < lowLimit) {
    price = lowLimit;
  }
  if (price > highLimit) {
    price = highLimit;
  }
  return price;
}

void print_banner(std::ostream& os,
                  const std::vector<PriceConsumer*>& priceConsumers)
{
  os << "Date";
  for(const PriceConsumer* pc : priceConsumers) {
    os << "," << pc->get_name();
  }
  os << std::endl;
}

void process_day(unsigned i, const DataSet& data,
                 std::vector<PriceConsumer*>& priceConsumers,
                 float smaAtPreviousClose)
{
    for(bt::PriceConsumer* pc : priceConsumers) {
      pc->set_current_date(data.dates[i], smaAtPreviousClose);
    }

    std::vector<float> prices = bt::get_intraday_price_sequence(i, data);

    for(bt::PriceConsumer* priceConsumer : priceConsumers) {
      priceConsumer->process_daily_prices(prices[0], prices[1], prices[2], prices[3]);
    }
}

void print_date_and_balances(std::ostream& os, unsigned i,
                             const DataSet& data,
                             const std::vector<PriceConsumer*>& priceConsumers)
{
    os<<data.dates[i];
    for(const bt::PriceConsumer* priceConsumer : priceConsumers) {
      os<<","<<priceConsumer->get_value();
    }
    os << std::endl;
}

void print_summary(std::ostream& os,
                   std::vector<PriceConsumer*>& priceConsumers)
{
  for(const bt::PriceConsumer* pc : priceConsumers) {
    pc->print_summary(os);
  }
}

void Position::process_daily_prices(float open, float low,
                                    float high, float close)
{
  process_price(open);
  process_price(open>close ? high : low);
  process_price(open>close ? low : high);
  process_price(close);
}

void Position::print_summary(std::ostream& os) const
{
  float numYears = static_cast<float>(numDays)/250;
  os << get_name() << ": $" << get_value()
     << ", max drawdown: "
     <<  this->get_position_stats().get_max_drawdown_percent()
     << "%"
     << ", CAGR: "
     << bt::CAGR_percent(10000.0, this->get_balance(), numYears)
     << "%"
     << std::endl;
}

void Position::process_price(float price)
{
  if (!is_invested() && (daysSinceSell >= 2)) {
    ShouldBuyResult result = should_buy(price, m_isOpeningPrice);
    if (result.buy) {
      result.buyPrice = m_isOpeningPrice ? price :
               force_into_range(result.buyPrice, price, previousPrice);
      buy(result.buyPrice);
    }
  }
  else if (is_invested()) {
    ShouldSellResult result = should_sell(price, m_isOpeningPrice);
    if (result.sell) {
      result.sellPrice = m_isOpeningPrice ? price :
               force_into_range(result.sellPrice, price, previousPrice);
      sell(result.sellPrice);
    }
  }

  m_isOpeningPrice = false;
  previousPrice = price;
  positionStats.add_balance(get_balance());
}

void Position::buy(float price)
{
  buyPrice = price;
  float quotient = (balance/buyPrice);
  numShares = static_cast<unsigned>(std::floor(quotient));
  balance -= numShares*buyPrice;
  invested = true;
  bt::out() << "   " << get_name() << " buy @ " << price << std::endl;
}

void Position::sell(float price)
{
  balance += numShares*price;
  numShares = 0;
  invested = false;
  bt::out() << "   " << get_name() << " sell @ " << price << std::endl;
  daysSinceSell = 0;
}

float Position::get_balance() const
{
  if (is_invested()) {
    return balance + numShares*previousPrice;
  }
  return balance;
}

ShouldBuyResult MyPosition::should_buy(float price, bool isOpeningPrice)
{
  bool priceIsAboveSma = isOpeningPrice
       && (get_previous_price() > get_sma_at_previous_close())
       && (price > get_sma_at_previous_close());
  if (isOpeningPrice) {
    openingPriceBelowSma = (price < get_sma_at_previous_close());
  }
  if (priceIsAboveSma) {
    bt::out() << " MyPos::should_buy: prev-prc="<<get_previous_price()
              <<", prevSma="<<get_sma_at_previous_close()
              <<", price="<<price << std::endl;
  }

  ShouldBuyResult shouldBuyResult{priceIsAboveSma, price};
  if (shouldBuyResult.buy) {
    movingPeak = price;
  }
  return shouldBuyResult;
}

ShouldSellResult MyPosition::should_sell(float price, bool isOpeningPrice)
{
  if (!is_invested()) {
    return {false, 0.0};
  }

  if (isOpeningPrice) {
    openingPriceBelowSma = (price < get_sma_at_previous_close());
    bt::out() << "   at open, price="<<price<<", prevSma="<<get_sma_at_previous_close()<<std::endl;
  }

  float trailPercent = openingPriceBelowSma ?
                       0 : trailingStopPercent;

  const bool priceBelowTrailPercent =
                (percent_less(movingPeak, price) > trailPercent);
  bt::out() << "    openedBelowSma="<<(openingPriceBelowSma?"true":"false")<<", trailPercent "<<trailPercent<<"%"<<std::endl;

  if (!priceBelowTrailPercent) {
    movingPeak = std::max(movingPeak, price);
    bt::out() << "    movingPeak -> "<<movingPeak<<std::endl;
  }

  ShouldSellResult result;
  result.sell = priceBelowTrailPercent;
  if (result.sell) {
    result.sellPrice = isOpeningPrice ?
                   price : this->movingPeak*((100.0-trailPercent)/100);
  }

  return result;
}

ShouldBuyResult SMAPosition::should_buy(float price, bool isOpeningPrice)
{
  if (is_invested() || !isOpeningPrice) {
    return {false, price};
  }

  //get_previous_price() is yesterday's closing price.
  //we buy (at today's opening price) if yesterday's closing price
  //was above the simple moving avg, and if today's opening price is
  //also above the simple moving avg.
  bool priceIsAboveSma = (get_previous_price() > get_sma_at_previous_close()) && (price > get_sma_at_previous_close());
  if (priceIsAboveSma) {
    bt::out() << "   " << get_name()
                  << " should_buy at " << price << std::endl;
  }
  return {priceIsAboveSma, price};
}

ShouldSellResult SMAPosition::should_sell(float price, bool isOpeningPrice)
{
  //we sell if yesterday's closing price was below the simple moving avg.
  if (isOpeningPrice &&
      (get_previous_price() < get_sma_at_previous_close()))
  {
    bt::out() << "   " << get_name()
                  << " should_sell at " << price << std::endl;
    return {true, price};
  }

  return {false, price};
}

ShouldBuyResult TSPPosition::should_buy(float price, bool isOpeningPrice)
{
  if (trailPrice < 0.0) {
    trailPrice = price;
    return {false, 0.0};
  }
  if (is_invested()) {
    return {false, 0.0};
  }

  const bool priceAboveTrailPercent =
                (percent_less(price, trailPrice) >= trailingStopPercentBuy);
  bt::out() << "   TSPsb: price="<<price<<" trailPrice="<<trailPrice
                << ", tsp="<<trailingStopPercentBuy<<"%, priceAbove="
                <<(priceAboveTrailPercent?"true":"false")<<std::endl;

  if (!priceAboveTrailPercent) {
    trailPrice = std::min(trailPrice, price);
    bt::out() << "   TSP trailPrice -> "<<trailPrice<<std::endl;
  }

  ShouldBuyResult result;
  result.buy = priceAboveTrailPercent;
  if (result.buy) {
    result.buyPrice = isOpeningPrice ?
                   price : trailPrice*((100.0+trailingStopPercentBuy)/100);
    movingPeak = std::max(result.buyPrice, price);
  }
  return result;
}

ShouldSellResult TSPPosition::should_sell(float price, bool isOpeningPrice)
{
  if (movingPeak < 0.0) {
    movingPeak = price;
    return {false, 0.0};
  }
  if (!is_invested()) {
    return {false, 0.0};
  }

  const bool priceBelowTrailPercent =
                (percent_less(movingPeak, price) > trailingStopPercentSell);
  bt::out() << "   TSPss: price="<<price<<" movingPeak="<<movingPeak
                << ", tsp="<<trailingStopPercentSell<<"%, priceBelow="
                <<(priceBelowTrailPercent?"true":"false")<<std::endl;
  if (!priceBelowTrailPercent) {
    movingPeak = std::max(movingPeak, price);
    bt::out() << "   TSP movingPeak -> "<<movingPeak<<std::endl;
  }
  ShouldSellResult result;
  result.sell = priceBelowTrailPercent;
  if (result.sell) {
    result.sellPrice = isOpeningPrice ?
                   price : this->movingPeak*((100.0-trailingStopPercentSell)/100);
    trailPrice = std::min(result.sellPrice, price);
  }
  return result;
}

}//namespace bt

