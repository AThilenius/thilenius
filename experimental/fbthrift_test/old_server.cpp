
#include <iostream>

#include "gen-cpp/Calculator.h"
#include "cloud/utils/thrift_http_server.hh"

using namespace example;

class CalculatorHandler : public CalculatorIf {
public:
  CalculatorHandler() {}

  int64_t add(const int32_t n1, const int32_t n2) {
    std::cout << "add(" << n1 << ", " << n2 << ")" << std::endl;
    return n1 + n2;
  }
};

using ::thilenius::cloud::utils::ThriftHttpServer;

int main() {
  ThriftHttpServer<CalculatorHandler, CalculatorProcessor> http_server(80, "/");
  http_server.ServeBlocking();
}
