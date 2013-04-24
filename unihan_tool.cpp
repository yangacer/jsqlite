#include <iostream>
#include <string>
#include <algorithm>
#include "unihan.hpp"

using namespace std;

int main(int argc, char **argv)
{
  variant vmap(argv[1]);

  size_t remain = 0;
  string buffer(2048, 0);
  while( !cin.eof() ) {
    cin.read(&buffer[remain], 2048 - remain);
    auto end = buffer.begin() + remain + cin.gcount();
    cout << vmap(buffer.begin(), end, remain);
    copy(end - remain, end, buffer.begin());
  }

  return 0;
}
