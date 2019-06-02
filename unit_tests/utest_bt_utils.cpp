#include <gtest/gtest.h>

#include <bt_utils.hpp>

TEST(bt_utils, get_path)
{
  std::string filename("/path/TQQQ.csv");
  std::string expectedPath("/path");
  std::string path = bt::get_path(filename);
  EXPECT_EQ(expectedPath, path);

  filename = "TQQQ.csv";
  expectedPath = "";
  path = bt::get_path(filename);
  EXPECT_EQ(expectedPath, path);
}

TEST(bt_utils, get_basename)
{
  std::string filename("/path/TQQQ.csv");
  std::string expectedBasename("TQQQ");
  std::string basename = bt::get_basename(filename);
  EXPECT_EQ(expectedBasename, basename);

  filename = "TQQQ.csv";
  basename = bt::get_basename(filename);
  EXPECT_EQ(expectedBasename, basename);
}

TEST(bt_utils, get_suffix)
{
  std::string filename("/path/TQQQ.csv");
  std::string expectedSuffix("csv");
  std::string suffix = bt::get_suffix(filename);
  EXPECT_EQ(expectedSuffix, suffix);

  filename = "TQQQ.csv";
  suffix = bt::get_suffix(filename);
  EXPECT_EQ(expectedSuffix, suffix);
}

TEST(bt_utils, get_output_filename)
{
  std::string filename("/path/TQQQ.csv");
  std::string expectedLogFilename("/path/TQQQ.csv");
  std::string logFilename = bt::get_output_filename(filename,"");
  EXPECT_EQ(expectedLogFilename, logFilename);

  filename = "TQQQ.csv";
  logFilename = bt::get_output_filename(filename,"foo");
  expectedLogFilename = "TQQQ_foo.csv";
  EXPECT_EQ(expectedLogFilename, logFilename);
}

