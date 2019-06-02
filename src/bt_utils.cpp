#include <bt_utils.hpp>
#include <vector>
#include <string>
#include <fstream>

namespace bt {

std::vector<float>
get_intraday_price_sequence(float open, float low,
                            float high, float close)
{
    const bool upDay = percent_less(close, open) > 0.1;
    std::vector<float> prices =
             {open,
              (upDay?low:high),
              (upDay?high:low),
              close};
    return prices;
}

std::string get_path(const std::string& filename)
{
  size_t lastSlashPos = filename.find_last_of('/');
  if (lastSlashPos == std::string::npos) {
    return "";
  }
  return filename.substr(0,lastSlashPos);
}

std::string get_basename(const std::string& filename)
{
  size_t lastSlashPos = filename.find_last_of('/');
  size_t dotPos = filename.find_last_of('.');
  size_t baseNameStartPos =
           lastSlashPos==std::string::npos ? 0 : lastSlashPos+1;
  return filename.substr(baseNameStartPos, dotPos-baseNameStartPos);
}

std::string get_suffix(const std::string& filename)
{
  size_t dotPos = filename.find_last_of('.');
  if (dotPos == std::string::npos) {
    return "";
  }
  return filename.substr(dotPos+1);
}

std::string get_output_filename(const std::string& filename,
                             const std::string& insert)
{
  std::string path = get_path(filename);
  if (!path.empty()) {
    path += "/";
  }

  std::string baseName = get_basename(filename);
  std::string suffix = get_suffix(filename);

  std::string logFileName = path + baseName;

  if (!insert.empty()) {
    logFileName += "_" + insert;
  }

  if (!suffix.empty()) {
    logFileName += "." + suffix;
  }

  return logFileName;
}

std::string get_log_filename(const std::string& filename,
                             const std::string& insert)
{
  std::string path = get_path(filename);
  if (!path.empty()) {
    path += "/";
  }

  std::string baseName = get_basename(filename);

  std::string logFileName = path + baseName;

  if (!insert.empty()) {
    logFileName += "_" + insert;
  }

  logFileName += ".log";

  return logFileName;
}

std::ostream& out(const std::string& filename)
{
  static std::ofstream ofstr(filename);
  return ofstr;
}

void align_with_sma(DataSet& data,
                    const std::vector<float>& sma,
                    unsigned numSMAPeriods)
{
  align_prices_with_sma(numSMAPeriods, sma, data.dates);
  align_prices_with_sma(numSMAPeriods, sma, data.openingPrices);
  align_prices_with_sma(numSMAPeriods, sma, data.highPrices);
  align_prices_with_sma(numSMAPeriods, sma, data.lowPrices);
  align_prices_with_sma(numSMAPeriods, sma, data.closingPrices);
}

} // namespace bt

