#include "where.hpp"
#include <iostream>

int main()
{
  std::string input="`arr` == 123 && `g` == 3344";
  
  auto beg(input.begin()), end(input.end());
  jsqlite::where::expr e;

  if(parse(beg,end, e)) {
    std::cout << "parsing sucess\n";
  } else {
    std::cerr << "parsing failed\n";
  }
  return 0;
}
