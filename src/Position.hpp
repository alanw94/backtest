#ifndef bt_Position_hpp
#define bt_Position_hpp

#include <PriceConsumer.hpp>
#include <analyze_prices.hpp>
#include <PositionStats.hpp>
#include <SMAComputer.hpp>

#include <iostream>

namespace bt {

struct ShouldBuyResult {
  bool buy = false;
  float buyPrice = 0.0;
};

struct ShouldSellResult {
  bool sell = false;
  float sellPrice = 0.0;
};


class Position : public PriceConsumer
{
public:
  Position(const std::string& nm,
           float bal = 10000.0,
           unsigned numSmaPeriods = 0,
           float buyPrc = 0.0,
           unsigned numShrs = 0)
  : PriceConsumer(nm),
    invested(false), balance(bal), buyPrice(buyPrc),
    numShares(numShrs), previousPrice(0.0),
    numProfitableSales(0), numTotalSales(0),
    currentDate(""), m_sma(numSmaPeriods),
    m_isOpeningPrice(true),
    daysSinceSell(2),
    positionStats()
  {}

  virtual ~Position(){}

  virtual void set_current_date(const std::string& date) override
  {
    currentDate = date;
    m_isOpeningPrice = true;
    ++numDays;
    if (!is_invested()) {
      ++daysSinceSell;
    }
  }

  void process_daily_prices(float open, float low,
                            float high, float close) override;

  float get_value() const override { return get_balance(); }

  void print_summary(std::ostream& os) const;

  bool is_invested() const { return invested; }
  float get_balance() const;
  float get_buy_price() const { return buyPrice; }
  unsigned get_num_shares() const { return numShares; }
  float get_sma_at_previous_close() const { return m_sma.get_average(); }
  float get_previous_price() const { return previousPrice; }

  const PositionStats& get_position_stats() const
  { return positionStats; }

private:
  void process_price(float price);

  virtual ShouldBuyResult should_buy(float price, bool isOpeningPrice) = 0;

  virtual ShouldSellResult should_sell(float price, bool isOpeningPrice) = 0;

  void buy(float price);
  void sell(float price);

  bool invested = false;
  unsigned numDays = 0;
  float balance = 10000.0;
  float buyPrice = 0.0;
  unsigned numShares = 0;
  float previousPrice = 0.0;
  unsigned numProfitableSales = 0;
  unsigned numTotalSales = 0;

  std::string currentDate;
  SMAComputer m_sma;
  bool m_isOpeningPrice = true;
  unsigned daysSinceSell = 2;
  PositionStats positionStats;
};

void print_banner(std::ostream& os,
                  const std::vector<PriceConsumer*>& priceConsumers);
 
void process_day(unsigned i, const DataSet& data,
                 std::vector<PriceConsumer*>& priceConsumers);

void print_date_and_balances(std::ostream& os, unsigned i,
                             const DataSet& data,
                             const std::vector<PriceConsumer*>& priceConsumers);

void print_summary(std::ostream& os,
                   std::vector<PriceConsumer*>& priceConsumers);

class MyPosition : public Position
{
public:
  MyPosition(float initialBalance, unsigned numSmaPeriods, float trailingPercent)
  : Position("MyPosition", initialBalance, numSmaPeriods),
    trailingStopPercent(trailingPercent)
  {}

  ~MyPosition(){}

private:
  ShouldBuyResult should_buy(float price, bool isOpeningPrice) override;

  ShouldSellResult should_sell(float price, bool isOpeningPrice) override;

  bool openingPriceBelowSma = false;
  float movingPeak = 0.0;
  float trailingStopPercent = 0.0;
};

class BuyAndHoldPosition : public Position
{
public:
  BuyAndHoldPosition(const std::string& equityName, float initialBalance)
  : Position(equityName, initialBalance)
  {}

  ~BuyAndHoldPosition(){}

private:
  ShouldBuyResult should_buy(float price, bool /*isOpeningPrice*/) override
  {
    if (is_invested()) {
      return {false, price};
    }
    //I want to buy at the first closing price, which is the
    //fourth price we receive...
    static unsigned numPrices = 0;
    ++numPrices;
    if (numPrices == 4) {
      return {true, price};
    }
    return {false, 0.0};
  }

  ShouldSellResult should_sell(float price, bool /*isOpeningPrice*/) override {
    return {false, price};
  }
};

class SMAPosition : public Position
{
public:
  SMAPosition(float initialBalance, unsigned numSmaPeriods)
   : Position("SMASignal", initialBalance, numSmaPeriods) {}
  virtual ~SMAPosition() {}

private:
  virtual ShouldBuyResult should_buy(float price, bool isOpeningPrice);

  virtual ShouldSellResult should_sell(float price, bool isOpeningPrice);
};

class TSPPosition : public Position
{
public:
  TSPPosition(float initialBalance, float trailPercentSell, float trailPercentBuy=-1.0)
  : Position("TSPSignal", initialBalance),
    trailingStopPercentSell(trailPercentSell),
    trailingStopPercentBuy((trailPercentBuy<0.0 ? trailPercentSell : trailPercentBuy)),
    trailPrice(-1.0), movingPeak(-1.0)
  {}
  virtual ~TSPPosition() {}

private:
  virtual ShouldBuyResult should_buy(float price, bool isOpeningPrice);

  virtual ShouldSellResult should_sell(float price, bool isOpeningPrice);

  float trailingStopPercentSell = 0.0;
  float trailingStopPercentBuy = 0.0;
  float trailPrice = -1.0;
  float movingPeak = -1.0;
};

inline
std::ostream& operator<<(std::ostream& os, const Position& pos)
{
  os << "{buyPrice="<<pos.get_buy_price()<<" numShares="<<pos.get_num_shares()
     << " balance="<<pos.get_balance()<<"}";
  return os;
}

} // namespace bt
#endif
