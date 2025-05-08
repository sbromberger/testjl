#include <faker-cxx/faker.h>

#include <benchjl.hpp>
#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <iostream>
#include <jsonlogic/logic.hpp>

const int SEED_ = 42;
static const size_t N_ = 100000;
static const int N_RUNS_ = 10;
int main(int argc, const char **argv) {
  size_t N = N_;
  if (argc > 1) {
    N = std::stoul(argv[1]);
  }
  size_t N_RUNS = N_RUNS_;
  if (argc > 2) {
    N_RUNS = std::stoul(argv[2]);
  }

  int SEED = SEED_;
  if (argc > 3) {
    SEED = std::stoul(argv[3]);
  }
  std::cout << "Hello, world!" << std::endl;

  faker::getGenerator().seed(SEED);
  std::vector<uint64_t> xs;
  xs.reserve(N);
  std::vector<uint64_t> ys;
  ys.reserve(N);

  for (size_t i = 0; i < N; ++i) {
    xs.push_back(faker::number::integer(0, 255));
    ys.push_back(faker::number::integer(0, 255));
  }

  std::string expr_str_xy = R"({"==":[{"var": "x"},{"var": "y"}]})";
  auto jv_xy = boost::json::parse(expr_str_xy);
  boost::json::object data_obj;

  size_t matches = 0;
  auto mylambda = [&] {
    matches = 0;
    for (size_t i = 0; i < N; ++i) {
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