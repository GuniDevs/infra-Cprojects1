// quality_test.cpp: deliberate code quality problems for the Infra AI quality check.
  #include <cstddef>
  #include <cstdio>
  #include <string>
  #include <utility>
  #include <vector>

  // Reliability: memory leak on the early return (clang-analyzer NewDeleteLeaks).
  int leaky(int n) {
      int* buffer = new int[n];
      if (n > 10) {
          return n;
      }
      delete[] buffer;
      return 0;
  }

  // Reliability: the vector is used after being moved (bugprone-use-after-move).
  std::size_t moved(std::vector<std::string> items) {
      std::vector<std::string> other = std::move(items);
      return items.size() + other.size();
  }

  // Reliability: possible null pointer dereference (clang-analyzer core).
  int deref(bool flag) {
      int* p = nullptr;
      if (flag) {
          static int value = 5;
          p = &value;
      }
      return *p;
  }

  // Performance: a large object copied on every call (performance-unnecessary-value-param).
  std::size_t total_length(const std::vector<std::string> names) {
      std::size_t total = 0;
      for (auto name : names) {  // performance-for-range-copy
          total += name.size();
      }
      return total;
  }

  // Maintainability: too many parameters (more than 6) and high complexity (more than 15).
  int grade(int a, int b, int c, int d, int e, int f, int g, int mode) {
      int score = 0;
      if (a > 0) score++;
      if (b > 0) score++;
      if (c > 0) score++;
      if (d > 0) score++;
      if (e > 0) score++;
      if (f > 0) score++;
      if (g > 0) score++;
      switch (mode) {
          case 1: score += 1; break;
          case 2: score += 2; break;
          case 3: score += 3; break;
          case 4: score += 4; break;
          case 5: score += 5; break;
          case 6: score += 6; break;
          default: break;
      }
      if (score > 10 && a > b) score--;
      if (score > 5 || c > d) score++;
      return score;
  }

  // Duplication: the same block copied twice.
  void report_first(const std::vector<int>& values) {
      int sum = 0;
      int max = 0;
      for (std::size_t i = 0; i < values.size(); ++i) {
          sum += values[i];
          if (values[i] > max) {
              max = values[i];
          }
      }
      std::printf("sum=%d max=%d count=%zu\n", sum, max, values.size());
      std::printf("average=%d\n", values.empty() ? 0 : sum / static_cast<int>(values.size()));
  }

  void report_second(const std::vector<int>& values) {
      int sum = 0;
      int max = 0;
      for (std::size_t i = 0; i < values.size(); ++i) {
          sum += values[i];
          if (values[i] > max) {
              max = values[i];
          }
      }
      std::printf("sum=%d max=%d count=%zu\n", sum, max, values.size());
      std::printf("average=%d\n", values.empty() ? 0 : sum / static_cast<int>(values.size()));
  }
