#ifndef JSQLITE_GEN_HPP_
#define JSQLITE_GEN_HPP_

#include <string>
#include "boost/variant.hpp"
#include "json/util.hpp"
#include "ref_stream.hpp"

struct gen_sql_insertable
: boost::static_visitor<std::string>
{
  gen_sql_insertable(char quote);
  template<typename T>
  std::string operator()(T const &v) const
  {
    using namespace yangacer;
    std::string result;
    ref_str_stream stream(result);
    json::pretty_print(stream, v, json::print::compact);
    return result;
  }

  std::string operator()(std::string const &s) const;
  std::string operator()(yangacer::json::array_t const &a) const;
  std::string operator()(yangacer::json::object_t const &o) const;
private:
  char quote_;
};

#endif
