#ifndef JSQLITE_WHERE_HPP_
#define JSQLITE_WHERE_HPP_

#include <string>
#include <boost/variant.hpp>
#include <boost/cstdint.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_rule.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

namespace jsqlite {
namespace where {

enum Op {
 AND, OR, EQ, NEQ, LT, GT, LE, GE, IS_NULL, NOT_NULL
};

typedef std::string identifier;

// FIXME
typedef boost::variant<std::string, boost::int64_t, double> literal;
typedef boost::variant<
  literal, Op, identifier, boost::recursive_variant_
  > expr;


template<typename Iterator>
struct expr_grammar
: boost::spirit::qi::grammar<Iterator, expr(), boost::spirit::qi::space_type>
{
  typedef boost::spirit::qi::space_type space_type;
  expr_grammar();

  boost::spirit::qi::rule<Iterator, expr(), space_type> expr_r;
  boost::spirit::qi::rule<Iterator, identifier(), space_type> id_r;
  boost::spirit::qi::rule<Iterator, literal(), space_type> lit_r;
  boost::spirit::qi::symbols<char const, Op> op_r;
};

template<typename Iter>
bool parse(Iter &beg, Iter &end, expr &e)
{
  namespace qi = boost::spirit::qi;
  expr_grammar<Iter> parser;
  return qi::phrase_parse(beg, end, parser, qi::space);
}

} // namespace where
} // namespace jsqlite

#endif
