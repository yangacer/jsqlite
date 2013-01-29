#include "where.hpp"
#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;

namespace jsqlite {
namespace where {

template<typename I>
expr_grammar<I>::expr_grammar()
: expr_grammar::base_type(expr_r)
{
  typedef qi::int_parser< boost::int64_t > int64_parser;
  int64_parser int64_;
  using qi::lit;
  using qi::uint_;
  using qi::char_;
  using qi::lexeme;
  char const quote_mark('`');
  
  op_r.add
    ("&&", AND) ("||", OR) ("==", EQ)
    ("!=", NEQ) ("<", LT) (">", GT)
    ("<=", LE) (">=", GE) ("IS NULL", IS_NULL)
    ("NOT NULL", NOT_NULL)
  ;

  lit_r %= 
    int64_ | 
    lexeme['"' >> *(char_ - '"') >> '"'] 
    ;

  id_r %= lit("`") >> +(char_ - quote_mark) >> quote_mark ;

  expr_r %= 
    lit_r 
    | id_r >> *( op_r >> expr_r )
    | lit("(") >> expr_r >> lit(")")
    ; 
  
  id_r.name("id");
  lit_r.name("lit");
  op_r.name("op");
  debug(id_r);
  debug(lit_r);
}

template expr_grammar<std::string::const_iterator>::expr_grammar();
template expr_grammar<std::string::iterator>::expr_grammar();

} // namespace where
} // namespace jsqlite
