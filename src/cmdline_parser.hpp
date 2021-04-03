#ifndef bt_cmdline_parser_hpp
#define bt_cmdline_parser_hpp

#include <string>
#include <sstream>
#include <iosfwd>

namespace bt {

std::string concatenate(char** argv, int argc);

template<typename T>
T get_param(const std::string& argstr,
            const std::string& flag,
            const T& defaultValue);

std::string get_param(const std::string& argstr,
                      const std::string& flag,
                      const std::string& defaultValue);

struct ParsedOptions
{
  std::string fileName1;
  std::string equityName;
  std::string outputFileName;
  std::string logFileName;
  float trailingStopPercent = 3.0;
  unsigned smaPeriods = 4;
};

ParsedOptions parse_command_line(char** argv, int argc);

std::ostream& operator<<(std::ostream& os, const ParsedOptions& opts);

template<typename T>
T get_param(const std::string& argstr,
            const std::string& flag,
            const T& defaultValue)
{
  std::string paramStr = get_param(argstr, flag, flag);
  if (paramStr == flag) {
    return defaultValue;
  }
  std::stringstream ss;
  ss<<paramStr;
  T result;
  ss>>result;
  return result;
}

}// namespace bt

#endif
