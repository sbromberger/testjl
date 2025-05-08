#pragma once
#include <unistd.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

using BenchTiming = std::vector<double>;  // milliseconds

class BenchmarkResult {
 public:
  BenchmarkResult() = default;
  BenchmarkResult(const std::string& name, const BenchTiming& timing)
      : name(name), timing(timing) {}

  void summarize() const {
    std::cout << "Benchmark: " << name << ": ";
    if (timing.empty()) {
      std::cout << "No timing data available.\n";
      return;
    }
    std::cout << "n_runs: " << timing.size() << ", ";
    std::cout << "min: " << timing[0] << " ms, ";
    std::cout << "max: " << timing[timing.size() - 1] << " ms, ";
    auto sum = std::accumulate(timing.begin(), timing.end(), 0.0);
    auto mean = sum / timing.size();
    std::cout << "mean: " << mean << " ms, ";

    std::cout << "std: "
              << std::sqrt(std::accumulate(timing.begin(), timing.end(), 0.0,
                                           [mean](double acc, double val) {
                                             return acc +
                                                    (val - mean) * (val - mean);
                                           }) /
                           timing.size())
              << " ms\n";
  }

 private:
  std::string name;
  BenchTiming timing;
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