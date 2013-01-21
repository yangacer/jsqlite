#include "gen.hpp"

std::string gen_sql_insertable::operator()(std::string const &s) const
{
  using namespace yangacer;
  std::string result;
  ref_str_stream stream(result);
  stream << "'";
  json::pretty_print(stream, s, json::print::compact);
  stream << "'";
  return result;
}

std::string gen_sql_insertable::operator()(yangacer::json::array_t const &a) const
{
  using namespace yangacer;
  std::string result;
  ref_str_stream stream(result);
  stream << "'";
  json::pretty_print(stream, a, json::print::compact);
  stream << "'";
  return result;
}

std::string gen_sql_insertable::operator()(yangacer::json::object_t const &o) const
{
  std::string names, values;
  names = values = "(";

  for(auto i=o.begin();i!=o.end();++i) {
    // form colum names
    if(i != o.begin()) {
      names += ",";
      values += ",";
    }
    names += i->first;
    values += boost::apply_visitor(*this, i->second);
  }
  names += ")";
  values += ")";
  return names + " VALUES " + values;
}
