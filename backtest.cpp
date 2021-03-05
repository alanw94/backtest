#include <iostream>
#include <fstream>
#include <cmath>
#include <bt_require.hpp>
#include <cmdline_parser.hpp>
#include <read_stream.hpp>
#include <analyze_prices.hpp>
#include <bt_utils.hpp>
#include <Position.hpp>

int main(int argc, char**argv)
{
  bt::ParsedOptions opts = bt::parse_command_line(argv, argc);
  std::string notSpecified("not specified");
  bt_require(notSpecified != opts.fileName1, "Failed to parse file name 1.");

  std::cout << "filename1: " << opts.fileName1 << ", filename2: " << opts.fileName2 << std::endl;
  std::cout << "Using trailingStopPercent = "<<opts.trailingStopPercent<<std::endl;
  std::cout << "Using smaPeriods = "<<opts.smaPeriods<<std::endl;

  std::cout<<"Writing output to: '"<<opts.outputFileName<<"'"<<std::endl;
  std::ofstream outFile(opts.outputFileName);

  std::cout<<"Writing log to: '"<<opts.logFileName<<"'"<<std::endl;
  bt::out(opts.logFileName) << opts.equityName
    <<", tsp: "<<opts.trailingStopPercent
    <<", sma: "<<opts.smaPeriods<<std::endl;

  bt::DataSet data1, data2;
  if (!bt::read_data(opts.fileName1, data1)) {
    return -1;
  }
  if (notSpecified != opts.fileName2) {
    if (!bt::read_data(opts.fileName2, data2)) {
      return -1;
    }
  }

  bt::ComputedData computedData1, computedData2;
  bt::compute_data(data1, opts.smaPeriods, computedData1);
  if (!data2.empty()) {
    bt::compute_data(data2, opts.smaPeriods, computedData2);
  }

  computedData1.second_derivs.insert(computedData1.second_derivs.begin(), 0.0);
  if (!computedData2.empty()) {
    computedData2.second_derivs.insert(computedData2.second_derivs.begin(), 0.0);
  }
  bt::align_with_sma(data1, computedData1.sma, opts.smaPeriods);
  if (!data2.empty()) {
    bt::align_with_sma(data2, computedData2.sma, opts.smaPeriods);
  }

  bt::MyPosition myPosition(opts.trailingStopPercent);
  bt::BuyAndHoldPosition bhPosition(opts.equityName);
  bt::SMAPosition smaPosition;
  bt::TSPPosition tspPosition(opts.trailingStopPercent);

  std::vector<bt::Position*> positions =
    {&myPosition, &tspPosition, &smaPosition, &bhPosition};

  outFile << "Date,";
  for(const bt::Position* pos : positions) {
    outFile<<pos->get_name()<<",";
  }
  outFile<<"SMA"+std::to_string(opts.smaPeriods)<<std::endl;

  float balance = 10000.0;
  const unsigned smaShares = static_cast<unsigned>(std::floor(balance/data1.closingPrices[1]));
  const float smaCash = balance - (smaShares*data1.closingPrices[1]);
  for(unsigned i=1; i<data1.dates.size(); ++i) {
    const float smaAtPreviousClose = computedData1.sma[i-1];
    bt::out()<<"---------------------------------------\n"
                  <<"i="<<i<<" "<<data1.dates[i]
                  <<", o "<<data1.openingPrices[i]
                  <<", l "<<data1.lowPrices[i]
                  <<", h "<<data1.highPrices[i]
                  <<", c "<<data1.closingPrices[i]
                  <<", smaPrev "<<smaAtPreviousClose
                  <<std::endl;

    for(bt::Position* pos : positions) {
      pos->set_current_date(data1.dates[i], smaAtPreviousClose);
    }

    std::vector<float> prices = bt::get_intraday_price_sequence(
                                      data1.openingPrices[i],
                                      data1.lowPrices[i],
                                      data1.highPrices[i],
                                      data1.closingPrices[i]);

    for(float price : prices) {
      for(bt::Position* pos : positions) {
        pos->process_price(price);
      }
    }

    outFile<<data1.dates[i]<<",";
    for(const bt::Position* pos : positions) {
      outFile<<pos->get_balance()<<",";
    }

    const float smaBalance = smaCash + smaShares*computedData1.sma[i];
    outFile<<smaBalance<<std::endl;
  }

  float numYears = (1.0*data1.closingPrices.size())/250;

  for(const bt::Position* pos : positions) {
    std::cout<<pos->get_name()<<": $"<<pos->get_balance()
             <<", max drawdown: "
             << pos->get_position_stats().get_max_drawdown_percent()
             <<"%"
             <<", CAGR: "
             <<bt::CAGR_percent(10000.0, pos->get_balance(), numYears)
             <<"%"
             <<std::endl;
  }

//    bt::Histogram hist = bt::compute_percentage_histogram(data1.openingPrices, data1.closingPrices);
//    for(auto h : hist) {
//      std::cout << h.first << "%: " << h.second.numDays
//              << ", then up "<<h.second.followedByUp
//              << ", dn "<<h.second.followedByDown << std::endl;
//    }

  return 0;
}
