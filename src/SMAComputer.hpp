#ifndef bt_SMAComputer_hpp
#define bt_SMAComputer_hpp

#include <bt_require.hpp>
#include <vector>

namespace bt {

inline
float sum(const std::vector<float>& prices, unsigned start, unsigned numVals)
{
  float result = 0.0;
  for(unsigned i=start; i<start+numVals; ++i) {
    result += prices[i];
  }
  return result;
}

class SMAComputer {
public:
  SMAComputer(unsigned numPeriods)
  : m_data(numPeriods, 0.0),
    m_curIdx(0),
    m_startingUp(true)
  {}

  void add_value(float val)
  {
    m_data[m_curIdx++] = val;
    if (m_curIdx >= m_data.size()) {
      m_curIdx = 0;
      if (m_startingUp) {
        m_startingUp = false;
      }
    }
  }

  float get_average() const
  {
    unsigned num = get_num_periods();
    return num > 0 ? sum(m_data, 0, num)/num : 0.0;
  }

  float get_oldest() const
  {
    int oldestIdx = m_startingUp ? 0 : m_curIdx;
    return m_data[oldestIdx];
  }

  float get_value_at(unsigned age) const
  {
    bt_require(age<get_num_periods(), "age="<<age<<" too large, num_periods="<<get_num_periods());
    int idx = m_curIdx - 1 - age;
    if (idx < 0) {
      idx += get_num_periods();
    }
    return m_data[idx];
  }

  unsigned get_num_periods() const
  {
    return m_startingUp ? m_curIdx : m_data.size();
  }

private:
  std::vector<float> m_data;
  unsigned m_curIdx;
  bool m_startingUp;
};

} // namespace bt

#endif
