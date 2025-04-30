// Create two vectors of random integers between 0 and 255 and compare pairwise
// using jsonlogic

#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <iostream>
#include <jsonlogic/logic.hpp>
#include <vector>

#include "faker-cxx/faker.h"

static const size_t N_ = 10;
static const size_t SEED = 42;

int main(int argc, const char **argv) {
  size_t N = N_;
  if (argc > 1) {
    N = std::stoul(argv[1]);
  }

  std::cout << "N: " << N << std::endl;

  faker::setSeed(SEED);
  std::string expr_str_xy = R"({"==":[{"var": "x"},{"var": "y"}]})";
  // std::string expr_str_set_contains = R"({"==":[1,2]})";

  std::vector<int> xs;
  std::vector<int> ys;

  for (size_t i = 0; i < N; ++i) {
    xs.push_back(faker::number::integer(0, 255));
    ys.push_back(faker::number::integer(0, 255));
  }

  auto jv_xy = boost::json::parse(expr_str_xy);
  auto data_obj = boost::json::object();

  size_t matches = 0;
  //   size_t j = 0;
  auto start = std::chrono::steady_clock::now();
  for (size_t i = 0; i < N; ++i) {
    data_obj["x"] = xs[i];
    data_obj["y"] = ys[i];
    // if (i < 5) {
    //   std::cout << "x: " << data_obj["x"] << ", y: " << data_obj["y"]
    //             << std::endl;
    // }
    auto data = boost::json::value_from(data_obj);
    auto v_xy = jsonlogic::apply(jv_xy, data);

    bool val = jsonlogic::truthy(v_xy);

    if (val) {
      ++matches;
      //   if (j < 5) {
      //     std::cout << "match: index = " << i << ", val = " << data_obj["x"]
      //               << std::endl;
      //   }
      //   ++j;
    }
  }
  auto end = std::chrono::steady_clock::now();
  auto elapsed =
      std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
  std::cout << "elapsed time: " << elapsed.count() << " seconds" << std::endl;
  std::cout << "matches: " << matches << std::endl;

  auto start2 = std::chrono::steady_clock::now();
  matches = 0;
  for (size_t i = 0; i < N; ++i) {
    if (xs[i] == ys[i]) {
      ++matches;
    }
  }
  auto end2 = std::chrono::steady_clock::now();
  auto elapsed2 =
      std::chrono::duration_cast<std::chrono::duration<double>>(end2 - start2);

  std::cout << "elapsed time: " << elapsed2.count() << " seconds" << std::endl;
  std::cout << "matches: " << matches << std::endl;
}
