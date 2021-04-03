#include <gtest/gtest.h>

#include <cmdline_parser.hpp>

#include <iostream>
#include <sstream>

std::pair<char**,int> initialize_test_args()
{
  int argc = 5;
  char** argv = new char*[argc];
  argv[0] = new char[3]; sprintf(argv[0], "-i");
  argv[1] = new char[4]; sprintf(argv[1], "foo");
  argv[2] = new char[3]; sprintf(argv[2], "-f");
  argv[3] = new char[4]; sprintf(argv[3], "bar");
  argv[4] = new char[3]; sprintf(argv[4], "-m");

  return std::make_pair(argv, argc);
}

void destroy_args(std::pair<char**,int>& args)
{
  for(int i=0; i<args.second; ++i) {
    delete [] args.first[i];
  }
  delete [] args.first;
}

TEST(trade, cmdline_concatenate)
{
  std::pair<char**,int> args = initialize_test_args();
  std::string str = bt::concatenate(args.first, args.second);

  std::string expected("-i foo -f bar -m");
  EXPECT_EQ(expected, str);

  destroy_args(args);
}

TEST(trade, cmdline_parse1)
{
  std::pair<char**,int> args = initialize_test_args();
  std::string str = bt::concatenate(args.first, args.second);

  std::string param = bt::get_param(str, "-i", "bar");
  std::string expected("foo");
  EXPECT_EQ(expected, param);

  destroy_args(args);
}

TEST(trade, cmdline_parseNotFound)
{
  std::pair<char**,int> args = initialize_test_args();
  std::string str = bt::concatenate(args.first, args.second);

  std::string param = bt::get_param(str, "-g", "bar");
  std::string expected("bar");
  EXPECT_EQ(expected, param);

  destroy_args(args);
}

TEST(trade, cmdline_flagFoundButValueEmpty)
{
  std::pair<char**,int> args = initialize_test_args();
  std::string str = bt::concatenate(args.first, args.second);

  std::string param = bt::get_param(str, "-m", "bar");
  std::string expected("bar");
  EXPECT_EQ(expected, param);

  destroy_args(args);
}

std::pair<char**,int> initialize_cmdline_options()
{
  int argc = 8;
  char** argv = new char*[argc];
  argv[0] = new char[7]; sprintf(argv[0], "-file1");
  argv[1] = new char[8]; sprintf(argv[1], "foo.csv");
  argv[2] = new char[7]; sprintf(argv[2], "-file2");
  argv[3] = new char[8]; sprintf(argv[3], "bar.csv");
  argv[4] = new char[5]; sprintf(argv[4], "-tsp");
  argv[5] = new char[4]; sprintf(argv[5], "4.0");
  argv[6] = new char[5]; sprintf(argv[6], "-sma");
  argv[7] = new char[2]; sprintf(argv[7], "5");

  return std::make_pair(argv, argc);
}

TEST(trade, cmdline_parsedOptions)
{
  std::pair<char**,int> args = initialize_cmdline_options();
  bt::ParsedOptions opts = bt::parse_command_line(args.first, args.second);
  EXPECT_EQ("foo.csv",opts.fileName1);
  EXPECT_EQ("foo", opts.equityName);
  EXPECT_EQ(4.0, opts.trailingStopPercent);
  EXPECT_EQ(5, opts.smaPeriods);
  EXPECT_EQ("foo_tsp4_sma5.csv", opts.outputFileName);
  EXPECT_EQ("foo_tsp4_sma5.log", opts.logFileName);
}
