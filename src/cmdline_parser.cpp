#include "cmdline_parser.hpp"
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
  return argstr.substr(pos, nextDelimiter);
}

float get_param(const std::string& argstr,
                const std::string& flag,
                float defaultValue)
{
  std::string paramStr = get_param(argstr, flag, flag);
  if (paramStr == flag) {
    return defaultValue;
  }
  std::stringstream ss;
  ss<<paramStr;
  float result;
  ss>>result;
  return result;
}

unsigned get_param(const std::string& argstr,
                const std::string& flag,
                unsigned defaultValue)
{
  std::string paramStr = get_param(argstr, flag, flag);
  if (paramStr == flag) {
    return defaultValue;
  }
  std::stringstream ss;
  ss<<paramStr;
  unsigned result;
  ss>>result;
  return result;
}

} // namespace bt

