#pragma once
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <format>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <string>
#include <thread>
#include <vector>

using BenchTiming = std::vector<double>;  // milliseconds

class BenchmarkResult {
 public:
  BenchmarkResult() = default;
  BenchmarkResult(const std::string& name, const BenchTiming& timing)
      : name(name), timing(timing) {
    sum = std::accumulate(timing.begin(), timing.end(), 0.0);

    if (timing.empty()) {
      mean = 0;
      stddev = 0;
    } else {
      double mean_ = sum / timing.size();
      stddev = std::sqrt(std::accumulate(timing.begin(), timing.end(), 0.0,
                                         [mean_](double acc, double val) {
                                           return acc +
                                                  (val - mean_) * (val - mean_);
                                         }) /
                         timing.size());
      mean = mean_;
    }
  }

  void summarize() const {
    std::cout << name << ": ";
    if (timing.empty()) {
      std::cout << "No timing data available.\n";
      return;
    }

    std::cout << std::format(
        "n_runs: {}, ttl: {:.3f} ms, min: {:.3f} ms, "
        "max: {:.3f} ms, mean: {:.3f} ms, std: {:.3f} ms\n",
        timing.size(), sum, timing[0], timing[timing.size() - 1], mean, stddev);
  }

  std::optional<double> compare_ratio(const BenchmarkResult& other) const {
    if (timing.empty() || other.timing.empty()) {
      std::cerr << "No timing data available for comparison.\n";
      return std::nullopt;
    }

    if (other.mean == 0) {
      return std::numeric_limits<double>::infinity();
    }

    double ratio = mean / other.mean;
    return ratio;
  }

  void compare_to(const BenchmarkResult& other) const {
    std::optional<double> ratio = compare_ratio(other);
    if (!ratio) {
      std::cout << "No timing data available for comparison.\n";
      return;
    }

    double rat = ratio.value();
    std::string relation = "equal";
    if (rat < 1) {
      relation = std::format("{:.1f}x faster", 1.0 / rat);
    }
    if (rat > 1) {
      relation = std::format("{:.1f}x slower", rat);
    }

    std::cout << std::format("Comparison with {}: {} is {} ({:.3e}:1)\n",
                             other.name, name, relation, rat);
  }

 private:
  std::string name;
  BenchTiming timing;
  double sum;
  double mean;
  double stddev;
};

class Benchmark {
 public:
  BenchmarkResult run(this auto&& self, int n_runs, auto&&... args) {
    auto timings = std::forward<decltype(self)>(self).run_(
        n_runs, std::forward<decltype(args)>(args)...);
    return BenchmarkResult(self.name, timings);
  }
  Benchmark(const std::string& name) : name(name) {}

 protected:
  std::string name;
};

template <typename F>
class GeneralBenchmark : public Benchmark {
 public:
  GeneralBenchmark(const std::string& name, F func)
      : Benchmark(name), func(func) {}
  BenchTiming run_(int n_runs) {
    BenchTiming timings;
    std::cout << "Running Benchmark " << name << ": " << n_runs << "\n";
    for (int i = 0; i < n_runs; ++i) {
      std::chrono::steady_clock::time_point start =
          std::chrono::steady_clock::now();
      func();
      std::chrono::steady_clock::time_point end =
          std::chrono::steady_clock::now();

      std::chrono::duration<double, std::milli> elapsed = end - start;
      timings.push_back(elapsed.count());
    }
    std::sort(timings.begin(), timings.end());
    return timings;
  }

 private:
  F func;
};

// class ProblemInstanceOne : public Benchmark {
//  public:
//   BenchTiming run_(int n_runs, const std::string& data) {
//     BenchTiming timings;
//     std::cout << "Running One: " << N << ", data = " << data << "\n";
//     for (int i = 0; i < n_runs; ++i) {
//       std::chrono::steady_clock::time_point start =
//           std::chrono::steady_clock::now();
//       std::this_thread::sleep_for(std::chrono::microseconds(12345));
//       std::chrono::steady_clock::time_point end =
//           std::chrono::steady_clock::now();

//       timings.push_back(elapsed.count());
//     }
//     return timings;
//   }
// };

// private : size_t N = 0;
// }
// ;

// class ProblemInstanceTwo : public Benchmark {
//  public:
//   BenchTiming run_(int data) {
//     std::cout << "Running Two: " << get_seed() << ", data = " << data <<
//     "\n"; return {};
//   }
// };

// class BadProblemInstance : public Benchmark {};

// // int main() {
// //   ProblemInstanceOne one;
// //   ProblemInstanceTwo two(66);
// //   BadProblemInstance bad;

// //   auto t1 = one.run("hello, world!");
// //   auto t2 = two.run(99934);

// //   for (auto& t : t1) {
// //     std::cout << "Elapsed time for One: " << t.count() << "ms\n";
// //   }

// //   for (auto& t : t2) {
// //     std::cout << "Elapsed time for Two: " << t.count() << " ms\n";
// //   }
// //   // This will not compile
// //   // bad.run(99934);

// //   return 0;
// // }