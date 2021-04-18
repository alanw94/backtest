#include <bt_require.hpp>
#include <cmdline_parser.hpp>
#include <read_stream.hpp>
#include <analyze_prices.hpp>
#include <bt_utils.hpp>
#include <Position.hpp>
#include <Deriv1Indicator.hpp>
#include <Deriv2Indicator.hpp>
#include <SMAIndicator.hpp>
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

  bt::DataSet data(opts.fileName1);
  bt::ComputedData computedData1(data, opts);

  const float balance = 10000.0;
  bt::BuyAndHoldPosition bhPosition(opts.equityName, balance);
  bt::TSPPosition tspPosition(balance, opts.trailingStopPercent);
  bt::Deriv1Indicator d1Indicator("Deriv1", opts.smaPeriods);
  bt::Deriv2Indicator d2Indicator("Deriv2", opts.smaPeriods);
  bt::SMAIndicator smaIndicator("SMA"+std::to_string(opts.smaPeriods),
                                 opts.smaPeriods, 10000.0);

  std::vector<bt::PriceConsumer*> positions =
    {&bhPosition, &tspPosition, &d1Indicator, &d2Indicator, &smaIndicator};

  bt::print_banner(outFile, positions);

  for(unsigned i=1; i<data.dates.size(); ++i) {
    bt::print_data(bt::out(), i, data);
    bt::process_day(i, data, positions);
    bt::print_date_and_balances(outFile, i, data, positions);
  }

  bt::print_summary(std::cout, positions);
  return 0;
}
