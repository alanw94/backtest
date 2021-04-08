#include <bt_utils.hpp>
#include <bt_require.hpp>
#include <read_stream.hpp>
#include <vector>
#include <string>
#include <fstream>

namespace bt {

DataSet::DataSet(const std::string& fileName)
: dates(), openingPrices(), highPrices(), lowPrices(), closingPrices()
{
  bt_require(read_data(fileName, *this), "Failed to read data from file "<<fileName);
}

std::vector<float>
get_intraday_price_sequence(unsigned i, const DataSet& data)
{
    bt_require(i < data.openingPrices.size(), "get_intraday_price_sequence i out of range");

    const float open = data.openingPrices[i];
    const float low = data.lowPrices[i];
    const float high = data.highPrices[i];
    const float close = data.closingPrices[i];
    const bool upDay = percent_less(close, open) > 0.1;
    std::vector<float> prices =
      {open, (upDay?low:high), (upDay?high:low), close};
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
  bt_require(ofstr.is_open(), "bt::out file-stream not open.");
  return ofstr;
}

void print_data(std::ostream& os, unsigned i, const DataSet& data)
{
    os << "---------------------------------------\n"
       << "i="<<i<<" "<<data.dates[i]
       << ", o "<<data.openingPrices[i]
       << ", l "<<data.lowPrices[i]
       << ", h "<<data.highPrices[i]
       << ", c "<<data.closingPrices[i]
       << std::endl;
}

} // namespace bt
