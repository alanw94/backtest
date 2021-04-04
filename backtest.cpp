#include <bt_require.hpp>
#include <cmdline_parser.hpp>
#include <read_stream.hpp>
#include <analyze_prices.hpp>
#include <bt_utils.hpp>
#include <Position.hpp>
#include <iostream>
#include <fstream>
#include <cmath>

int main(int argc, char**argv)
{
  bt::ParsedOptions opts = bt::parse_command_line(argv, argc);
  std::cout << opts << std::endl;

  std::ofstream outFile(opts.outputFileName);

  bt::out(opts.logFileName) << opts.equityName
    <<", tsp: "<<opts.trailingStopPercent
    <<", sma: "<<opts.smaPeriods<<std::endl;

  bt::DataSet data1(opts.fileName1);
  bt::ComputedData computedData1(data1, opts);

  bt::MyPosition myPosition(opts.trailingStopPercent);
  bt::BuyAndHoldPosition bhPosition(opts.equityName);
  bt::SMAPosition smaPosition;
  bt::TSPPosition tspPosition(opts.trailingStopPercent);

  std::vector<bt::PriceConsumer*> positions =
    {&myPosition, &tspPosition, &smaPosition, &bhPosition};

  bt::print_banner(outFile, positions,
  "SMA"+std::to_string(opts.smaPeriods)+",Deriv1,Deriv2");

  float balance = 10000.0;
  const unsigned smaShares = static_cast<unsigned>(std::floor(balance/data1.closingPrices[1]));
  const float smaCash = balance - (smaShares*data1.closingPrices[1]);
  for(unsigned i=1; i<data1.dates.size(); ++i) {
    const float smaAtPreviousClose = computedData1.sma[i-1];
    bt::print_data(bt::out(), i, data1, smaAtPreviousClose);
    bt::process_day(i, data1, positions, smaAtPreviousClose);
    bt::print_date_and_balances(outFile, i, data1, positions,
                                computedData1, smaCash, smaShares);
  }

  float numYears = (1.0*data1.closingPrices.size())/250;
  bt::print_summary(std::cout, numYears, positions);

  return 0;
}
