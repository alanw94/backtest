#include <iostream>
#include <fstream>
#include <cmath>
#include <cmdline_parser.hpp>
#include <read_stream.hpp>
#include <analyze_prices.hpp>
#include <bt_utils.hpp>
#include <Position.hpp>

int main(int argc, char**argv)
{
  std::string args = bt::concatenate(argv, argc);
  std::string notSpecified("not specified");
  std::string filename1 = bt::get_param(args, "-file1", notSpecified);
  std::string equityName = bt::get_basename(filename1);
  std::string filename2 = bt::get_param(args, "-file2", notSpecified);
  std::cout << "filename1: " << filename1 << ", filename2: " << filename2 << std::endl;
  const float defaultTrStopPct = 3.0;
  const float trailingStopPercent = bt::get_param(args, "-tsp", defaultTrStopPct);
  std::cout << "Using trailingStopPercent = "<<trailingStopPercent<<std::endl;
  const unsigned defaultSma = 4;
  const unsigned smaPeriods = bt::get_param(args, "-sma", defaultSma);
  std::cout << "Using smaPeriods = "<<smaPeriods<<std::endl;

  std::string insert = "tsp"+std::to_string(static_cast<unsigned>(trailingStopPercent))+"_sma"+std::to_string(smaPeriods);
  std::string outputFileName = bt::get_output_filename(filename1,insert);
  std::cout<<"Writing output to: '"<<outputFileName<<"'"<<std::endl;
  std::ofstream outFile(outputFileName);

  std::string logFileName = bt::get_log_filename(filename1, insert);
  std::cout<<"Writing log to: '"<<logFileName<<"'"<<std::endl;
  bt::out(logFileName) << equityName<<", "<<insert<<std::endl;

  if (notSpecified != filename1) {
    bt::DataSet data1, data2;
    if (!read_data(filename1, data1)) {
      return -1;
    }
    if (notSpecified != filename2) {
      if (!read_data(filename2, data2)) {
        return -1;
      }
    }

    bt::ComputedData computedData1, computedData2;
    bt::compute_data(data1, smaPeriods, computedData1);
    if (!data2.empty()) {
      bt::compute_data(data2, smaPeriods, computedData2);
    }

    computedData1.second_derivs.insert(computedData1.second_derivs.begin(), 0.0);
    if (!computedData2.empty()) {
      computedData2.second_derivs.insert(computedData2.second_derivs.begin(), 0.0);
    }
    bt::align_with_sma(data1, computedData1.sma, smaPeriods);
    if (!data2.empty()) {
      bt::align_with_sma(data2, computedData2.sma, smaPeriods);
    }

    bt::MyPosition myPosition(trailingStopPercent);
    bt::BuyAndHoldPosition bhPosition(equityName);
    bt::SMAPosition smaPosition;
    bt::TSPPosition tspPosition(trailingStopPercent);

    std::vector<bt::Position*> positions =
      {&myPosition, &tspPosition, &smaPosition, &bhPosition};

    outFile << "Date,";
    for(const bt::Position* pos : positions) {
      outFile<<pos->get_name()<<",";
    }
    outFile<<"SMA"+std::to_string(smaPeriods)<<std::endl;

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

    for(const bt::Position* pos : positions) {
      std::cout<<pos->get_name()<<": $"<<pos->get_balance()<<std::endl;
    }

//    bt::Histogram hist = bt::compute_percentage_histogram(data1.openingPrices, data1.closingPrices);
//    for(auto h : hist) {
//      std::cout << h.first << "%: " << h.second.numDays
//              << ", then up "<<h.second.followedByUp
//              << ", dn "<<h.second.followedByDown << std::endl;
//    }
  }

  return 0;
}

