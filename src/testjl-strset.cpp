// Create two vectors of random integers between 0 and 255 and compare pairwise
// using jsonlogic

#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <iostream>
#include <jsonlogic/logic.hpp>
#include <set>
#include <vector>

#include "faker-cxx/faker.h"

static const size_t N_ = 10;
static const size_t SEED = 42;
static const std::string CHARS = "abcdefghij";
static const size_t STRLEN = 3;
static const size_t SET_N = 100;
static const std::vector<std::string> RINGER{"aaa", "bbb", "ccc"};

int main(int argc, const char **argv) {
  size_t N = N_;
  if (argc > 1) {
    N = std::stoul(argv[1]);
  }

  std::cout << "N: " << N << std::endl;

  faker::setSeed(SEED);
  std::string expr_str_xy =
      // R"({"some":[{"var": "y"},{"==":[{"var":""},{"var": "x"}]}]})";  // x in
      // y
      R"({"some":[{"var": "y"},{">":["azz", {"var": "x"}]}]})";  // "bbb"
                                                                 // in y
  // std::string expr_str_xy = R"({"in":[{"var": "x"},{"var": "y"}]})";  // x in
  // y std::string expr_str_set_contains = R"({"==":[1,2]})";

  std::set<std::string> sset;
  std::vector<std::string> ss;

  for (size_t i = 0; i < SET_N; ++i) {
    sset.insert(faker::string::fromCharacters(CHARS, STRLEN));
  }

  std::vector<std::string> svec(sset.begin(), sset.end());

  for (size_t i = 0; i < N; ++i) {
    ss.push_back(faker::string::fromCharacters(CHARS, STRLEN));
  }

  auto jv_xy = boost::json::parse(expr_str_xy);
  auto data_obj = boost::json::object();

  // auto svec_json = boost::json::array(svec.begin(), svec.end());
  auto svec_json = boost::json::array(RINGER.begin(), RINGER.end());
  size_t matches = 0;
  size_t j = 0;
  auto start = std::chrono::steady_clock::now();
  for (size_t i = 0; i < N; ++i) {
    data_obj["x"] = ss[i];
    data_obj["y"] = svec_json;
    if (i < 5) {
      std::cout << "x: " << data_obj["x"] << ", y: " << data_obj["y"]
                << std::endl;
    }
    auto data = boost::json::value_from(data_obj);
    auto v_xy = jsonlogic::apply(jv_xy, data);

    bool val = jsonlogic::truthy(v_xy);

    if (val) {
      ++matches;
      if (j < 5) {
        std::cout << "match: index = " << i << ", val = " << data_obj["x"]
                  << std::endl;
      }
      ++j;
    }
  }
  auto end = std::chrono::steady_clock::now();
  auto elapsed =
      std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
  std::cout << "elapsed time: " << elapsed.count() << " seconds" << std::endl;
  std::cout << "matches: " << matches << std::endl;

  auto start2 = std::chrono::steady_clock::now();
  matches = 0;
  j = 0;
  for (size_t i = 0; i < N; ++i) {
    if (sset.contains(ss[i])) {
      ++matches;
      if (j < 5) {
        std::cout << "match: index = " << i << ", val = " << ss[i] << std::endl;
      }
      ++j;
    }
  }
  auto end2 = std::chrono::steady_clock::now();
  auto elapsed2 =
      std::chrono::duration_cast<std::chrono::duration<double>>(end2 - start2);

  std::cout << "elapsed time: " << elapsed2.count() << " seconds" << std::endl;
  std::cout << "matches: " << matches << std::endl;
}
