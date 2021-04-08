#include <gtest/gtest.h>
#include <SMAComputer.hpp>

TEST(smacomputer, basic)
{
  unsigned numPeriods = 3;
  std::vector<float> vals = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<float> expected =
                          {0, 0.5, 1, 2, 3, 4, 5, 6, 7};
  bt::SMAComputer sma(numPeriods);

  for(unsigned i=0; i<vals.size(); ++i) {
    sma.add_value(vals[i]);
    EXPECT_EQ(expected[i], sma.get_average());
    unsigned num = sma.get_num_periods();
    EXPECT_EQ(std::min(i+1, numPeriods), num);
    float expectedOldest = vals[i+1-num];
    EXPECT_EQ(expectedOldest, sma.get_oldest());
    for(unsigned age=0; age<num; ++age) {
      float expectedValAtAge = vals[i-age];
      EXPECT_EQ(expectedValAtAge, sma.get_value_at(age))
      <<"i="<<i<<" vals["<<i<<"]="<<vals[i]<<" age="<<age;
    }
  }
}