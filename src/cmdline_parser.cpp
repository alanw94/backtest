#include "cmdline_parser.hpp"
#include "bt_utils.hpp"
#include "bt_require.hpp"
#include <iostream>
#include <sstream>

namespace bt {

std::string concatenate(char** argv, int argc)
{
  std::string result;
  for(int i=0; i<argc; ++i) {
    result += std::string(argv[i]);
    if (i < argc-1) {
      result += " ";
    }
  }

  return result;
}

bool is_delimiter(char c)
{
  return c == ' ' || c == '\n' || c == '\0';
}

std::string::size_type find_next_delimiter(const std::string& str)
{
  std::string::size_type pos = 0;
  while(pos<str.size() && !is_delimiter(str[pos])) {
    ++pos;
  }
  return pos;
}

std::string get_param(const std::string& argstr,
                      const std::string& flag,
                      const std::string& defaultValue)
{
  std::string::size_type pos = argstr.find(flag);
  bool notFound = (pos >= argstr.size());
  if (notFound) {
    return defaultValue;
  }

  pos += flag.size();
  if (argstr[pos] == '=' || argstr[pos] == ' ') {
    ++pos;
  }
  std::string::size_type nextDelimiter = find_next_delimiter(argstr.substr(pos));
  std::string result = argstr.substr(pos, nextDelimiter);
  return result.empty() ? defaultValue : result;
}

ParsedOptions parse_command_line(char** argv, int argc)
{
  std::string args = concatenate(argv, argc);
  ParsedOptions options;
  options.fileName1 = get_param(args, "-file1", "not specified");
  options.fileName2 = get_param(args, "-file2", "not specified");
  options.trailingStopPercent = get_param(args, "-tsp", options.trailingStopPercent);
  options.smaPeriods = get_param(args, "-sma", options.smaPeriods);

  bt_require(options.fileName1 != "not specified", "Faile to parse required arg '-file1'");

  options.equityName = get_basename(options.fileName1);
  std::string insert = "tsp"+std::to_string(static_cast<unsigned>(options.trailingStopPercent))
                     +"_sma"+std::to_string(options.smaPeriods);
  options.outputFileName = get_output_filename(options.fileName1,insert);
  options.logFileName = get_log_filename(options.fileName1,insert);
  return options;
}

std::ostream& operator<<(std::ostream& os, const ParsedOptions& opts)
{
  os << "filename1: " << opts.fileName1 << ", filename2: " << opts.fileName2 << std::endl;
  os << "Using trailingStopPercent = "<<opts.trailingStopPercent<<std::endl;
  os << "Using smaPeriods = "<<opts.smaPeriods<<std::endl;
  os <<"Writing output to: '"<<opts.outputFileName<<"'"<<std::endl;
  os <<"Writing log to: '"<<opts.logFileName<<"'"<<std::endl;
  return os;
}

} // namespace bt
