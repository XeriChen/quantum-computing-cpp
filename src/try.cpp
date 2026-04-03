#include <print>
int main() {
  std::print("My name is {}.\n", "Simon");
  double val = 6.22;
  std::string hello = "Hello";
  std::print("{} Dos {} !\n", hello, val);
  return 0;
}