#include <gtest/gtest.h>

#include <cmdline_parser.hpp>

#include <iostream>
#include <sstream>

std::pair<char**,int> initialize_test_args()
{
  int argc = 4;
  char** argv = new char*[argc];
  argv[0] = new char[3]; sprintf(argv[0], "-i");
  argv[1] = new char[4]; sprintf(argv[1], "foo");
  argv[2] = new char[3]; sprintf(argv[2], "-f");
  argv[3] = new char[4]; sprintf(argv[3], "bar");

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

  std::string expected("-i foo -f bar");
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

