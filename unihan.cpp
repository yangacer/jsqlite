#include "unihan.hpp"
#include <fstream>
#include <sstream>
#include <iterator>
#include <cstring>
#include "utf8.h"

using namespace std;

variant::variant(char const *dictionary)
{
  // read dictionary and construct map
  ifstream fin(dictionary, ios::in | ios::binary);
  string line, buf;

  while(getline(fin, line)) {
    if(line[0] == '#') continue;
    stringstream sin(line);
    sin >> buf;
    boost::uint32_t key = strtoul(buf.c_str()+2, 0, 16);
    sin >> buf >> buf;
    boost::uint32_t value = strtoul(buf.c_str()+2, 0, 16);
    if( sin >> buf ) // one-to-many mapping is ignored
      continue;
    auto range = equal_range(key);
    bool dup = false;
    for(auto i = range.first; i != range.second;++i) {
      if( i->second == value ) {
        dup = true;
        break;
      }
    }
    if(!dup)
      insert(make_pair(key, value));
  }
}

boost::uint32_t variant::operator()(boost::uint32_t code_point)
{
  auto range = equal_range(code_point);
  //cout << hex << code_point << ": " << count(code_point) << "\n";
  return (range.first != range.second) ? range.first->second : 0;
}

std::string variant::operator()(string::const_iterator first, string::const_iterator last, size_t &remain)
{
  string::const_iterator end_it = utf8::find_invalid(first, last);
  utf8::unchecked::iterator<char const*> beg(&*first), end(&*end_it);
  remain = last - end_it;
  string rt;
  for(;beg != end; ++beg) {
    boost::uint32_t converted = (*this)(*beg);
    if(!converted) 
      converted = *beg;
    utf8::unchecked::append(converted, back_inserter(rt));
  }
  return move(rt);
}
