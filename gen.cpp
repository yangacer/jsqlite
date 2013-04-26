#include "gen.hpp"

gen_sql_insertable::gen_sql_insertable(char quote)
  : quote_(quote)
{}

std::string gen_sql_insertable::operator()(std::string const &s) const
{
  using namespace yangacer;
  std::string result;
  ref_str_stream stream(result);
  stream << quote_;
  json::pretty_print(stream, s, json::print::compact);
  stream << quote_;
  stream.flush();
  if(result.size() > 2){
    for(auto i=result.begin()+1; i < result.end()-1;++i) {
      if('\'' == *i) 
        i = result.insert(i, '\'') + 1;
    }
  }
  return result;
}

std::string gen_sql_insertable::operator()(yangacer::json::array_t const &a) const
{
  using namespace yangacer;
  std::string result;
  ref_str_stream stream(result);
  stream << quote_;
  json::pretty_print(stream, a, json::print::compact);
  stream << quote_;
  stream.flush();
  return result;
}

std::string gen_sql_insertable::operator()(yangacer::json::object_t const &o) const
{
  using namespace yangacer;
  std::string result;
  ref_str_stream stream(result);
  stream << quote_;
  json::pretty_print(stream, o, json::print::compact);
  stream << quote_;
  stream.flush();
  return result;
}
