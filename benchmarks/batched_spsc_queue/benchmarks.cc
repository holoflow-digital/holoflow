#include "batched_spsc_queue/batched_spsc_queue.hh"

#include <array>
#include <cstdint>
#include <cstring>
#include <memory>
#include <span>
#include <thread>
#include <vector>

#include <benchmark/benchmark.h>

namespace holoflow {

constexpr size_t NB_SLOTS = 1000;
constexpr size_t ENQUEUE_BATCH_SIZE = 1;
constexpr size_t DEQUEUE_BATCH_SIZE = 1;
constexpr size_t ELEMENT_SIZE = sizeof(uint8_t);
constexpr size_t BUFFER_SIZE = NB_SLOTS * ELEMENT_SIZE;
constexpr size_t ENQUEUE_BYTES = ENQUEUE_BATCH_SIZE * ELEMENT_SIZE;
constexpr size_t DEQUEUE_BYTES = DEQUEUE_BATCH_SIZE * ELEMENT_SIZE;

static void BM_SPSC(benchmark::State &state) {
  std::vector<uint8_t> buffer(BUFFER_SIZE);
  BatchedSPSCQueue queue(NB_SLOTS, ENQUEUE_BATCH_SIZE, DEQUEUE_BATCH_SIZE,
                         ELEMENT_SIZE, buffer.data());

  std::array<uint8_t, ENQUEUE_BATCH_SIZE * ELEMENT_SIZE> source = {0};
  std::array<uint8_t, DEQUEUE_BATCH_SIZE * ELEMENT_SIZE> dest = {0};

  benchmark::DoNotOptimize(source);
  benchmark::DoNotOptimize(dest);
  benchmark::DoNotOptimize(buffer.data());

  std::atomic<bool> run = true;
  size_t p_count = 0;
  size_t c_count = 0;

  auto producer = [&queue, &source, &p_count, &run]() {
    while (run) {
      auto batch = queue.write_ptr();
      while (batch == nullptr && run)
        batch = queue.write_ptr();

      if (batch == nullptr)
        break;

      std::copy(source.begin(), source.end(), batch);
      queue.commit_write();
      p_count++;
    }
  };

  auto consumer = [&queue, &dest, &c_count, &run]() {
    while (run) {
      auto batch = queue.read_ptr();
      while (batch == nullptr && run)
        batch = queue.read_ptr();

      if (batch == nullptr)
        break;

      std::copy(batch, batch + ENQUEUE_BYTES, dest.begin());
      queue.commit_read();
      c_count++;
    }
  };

  std::thread prod_thread(producer);
  std::thread cons_thread(consumer);

  for (auto _ : state) {
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + std::chrono::nanoseconds(10);
    while (std::chrono::high_resolution_clock::now() < end) {
      // Busy-wait loop
    }
  }

  run = false;
  prod_thread.join();
  cons_thread.join();

  state.counters["En/De"] = benchmark::Counter(
      static_cast<double>(p_count + c_count), benchmark::Counter::kIsRate);

  state.counters["Bandwidth"] = benchmark::Counter(
      static_cast<double>((p_count + c_count) *
                          (DEQUEUE_BYTES + ENQUEUE_BYTES)),
      benchmark::Counter::kIsRate, benchmark::Counter::kIs1024);
}

static void BM_Enqueue(benchmark::State &state) {
  std::vector<uint8_t> buffer(BUFFER_SIZE);
  BatchedSPSCQueue queue(NB_SLOTS, ENQUEUE_BATCH_SIZE, DEQUEUE_BATCH_SIZE,
                         ELEMENT_SIZE, buffer.data());

  std::array<uint8_t, ENQUEUE_BATCH_SIZE * ELEMENT_SIZE> source = {0};

  benchmark::DoNotOptimize(source);
  benchmark::DoNotOptimize(buffer.data());

  for (auto _ : state) {
    auto batch = queue.write_ptr();
    if (!batch) {
      queue.reset();
      batch = queue.write_ptr();
    }

    std::copy(source.begin(), source.end(), batch);
    queue.commit_write();
  }

  state.counters["Enqueues"] = benchmark::Counter(
      static_cast<double>(state.iterations()), benchmark::Counter::kIsRate);

  state.counters["Bandwidth"] = benchmark::Counter(
      static_cast<double>(state.iterations()) * ENQUEUE_BYTES,
      benchmark::Counter::kIsRate, benchmark::Counter::kIs1024);
}

static void BM_Dequeue(benchmark::State &state) {
  std::vector<uint8_t> buffer(BUFFER_SIZE);
  BatchedSPSCQueue queue(NB_SLOTS, ENQUEUE_BATCH_SIZE, DEQUEUE_BATCH_SIZE,
                         ELEMENT_SIZE, buffer.data());

  std::array<uint8_t, DEQUEUE_BATCH_SIZE * ELEMENT_SIZE> dest = {0};
  queue.fill();

  benchmark::DoNotOptimize(dest);
  benchmark::DoNotOptimize(buffer.data());

  for (auto _ : state) {
    auto batch = queue.read_ptr();
    if (!batch) {
      queue.fill();
      batch = queue.read_ptr();
    }

    std::copy(batch, batch + ENQUEUE_BYTES, dest.begin());
    queue.commit_read();
  }

  state.counters["Dequeues"] = benchmark::Counter(
      static_cast<double>(state.iterations()), benchmark::Counter::kIsRate);

  state.counters["Bandwidth"] = benchmark::Counter(
      static_cast<double>(state.iterations()) * DEQUEUE_BYTES,
      benchmark::Counter::kIsRate, benchmark::Counter::kIs1024);
}

// NOLINTBEGIN
BENCHMARK(BM_SPSC)->MinTime(20.0);
BENCHMARK(BM_Enqueue)->MinTime(20.0);
BENCHMARK(BM_Dequeue)->MinTime(20.0);
// NOLINTEND

} // namespace holoflow

BENCHMARK_MAIN();
