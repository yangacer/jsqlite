#ifndef JSQLITE_UNIHAN_HPP_
#define JSQLITE_UNIHAN_HPP_

#include <string>
#include "boost/unordered_map.hpp"
#include "boost/cstdint.hpp"

class variant 
: private
  boost::unordered_multimap<boost::uint32_t, boost::uint32_t>
{
  typedef 
  boost::unordered_multimap<boost::uint32_t, boost::uint32_t> super_;
public:
  using typename super_::iterator;
  variant(char const *dictionary);
  boost::uint32_t operator()(boost::uint32_t code_point);
  std::string operator()(
    std::string::const_iterator first, std::string::const_iterator last, size_t &remain);
  std::string operator()(char const *first, char const *last, size_t &remain);
  using super_::equal_range;
};

#endif
