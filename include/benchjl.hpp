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

template <typename F>
class Benchmark {
 public:
  Benchmark(const std::string& name, F func) : name(name), func(func) {}

  BenchmarkResult run(int n_runs, auto&&... args) {
    BenchTiming timings;
    std::cout << "Running Benchmark " << name << ": " << n_runs << "\n";
    for (int i = 0; i < n_runs; ++i) {
      std::chrono::steady_clock::time_point start =
          std::chrono::steady_clock::now();
      func(args...);
      std::chrono::steady_clock::time_point end =
          std::chrono::steady_clock::now();

      std::chrono::duration<double, std::milli> elapsed = end - start;
      timings.push_back(elapsed.count());
    }
    std::sort(timings.begin(), timings.end());
    return BenchmarkResult{name, timings};
  }

 private:
  std::string name;
  F func;
};
