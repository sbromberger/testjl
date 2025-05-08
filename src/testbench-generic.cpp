#include <faker-cxx/faker.h>

#include <benchjl.hpp>
#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <iostream>
#include <jsonlogic/logic.hpp>

const int SEED = 42;
static const size_t N_ = 10000;
static const int N_RUNS = 100;
int main() {
  std::cout << "Hello, world!" << std::endl;

  faker::setSeed(SEED);

  std::vector<uint64_t> xs;
  xs.reserve(N_);
  std::vector<uint64_t> ys;
  ys.reserve(N_);

  for (size_t i = 0; i < N_; ++i) {
    xs.push_back(faker::number::integer(0, 255));
    ys.push_back(faker::number::integer(0, 255));
  }

  std::string expr_str_xy = R"({"==":[{"var": "x"},{"var": "y"}]})";
  auto jv_xy = boost::json::parse(expr_str_xy);
  boost::json::object data_obj;

  size_t matches = 0;
  auto mylambda = [&] {
    for (size_t i = 0; i < N_; ++i) {
      data_obj["x"] = xs[i];
      data_obj["y"] = ys[i];
      auto data = boost::json::value_from(data_obj);
      auto v_xy = jsonlogic::apply(jv_xy, data);

      bool val = jsonlogic::truthy(v_xy);

      if (val) {
        ++matches;
      }
    }
  };

  auto g = GeneralBenchmark("2ints", mylambda);

  auto results = g.run(N_RUNS);
  std::cout << "matches: " << matches << std::endl;
  results.summarize();

  return 0;
}