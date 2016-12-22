#include <iostream>
#include <string>
#include "../src/Parser.hpp"

using namespace std;
using namespace ks;

int main() {
  std::string a;

  Parser parser;

  std::cout << "ready > ";
  while (getline(cin, a)) {
    parser.set_input(a);
    parser.parse();
    std::cout << "ready > ";
  }
}