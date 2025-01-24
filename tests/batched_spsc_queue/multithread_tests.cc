#include "batched_spsc_queue/batched_spsc_queue.hh"

#include <chrono>
#include <cstdint>
#include <future>
#include <numeric>
#include <stdexcept>

#include <gtest/gtest.h>

namespace holoflow {

class BatchedSPSCQueueMultiThreadingTest
    : public ::testing::TestWithParam<
          std::tuple<std::chrono::seconds, std::chrono::microseconds,
                     std::chrono::microseconds, size_t, size_t, size_t>> {};

TEST_P(BatchedSPSCQueueMultiThreadingTest, MT) {
  // Test parameters.
  auto [test_duration, enqueue_delay, dequeue_delay, nb_slots,
        enqueue_batch_size, dequeue_batch_size] = GetParam();

  // Create the queue.
  size_t element_size = sizeof(uint8_t);
  size_t buffer_size = nb_slots * element_size;
  std::vector<uint8_t> buffer(buffer_size);
  BatchedSPSCQueue queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                         element_size, buffer.data());

  // Enqueue thread.
  std::thread enqueue_thread(
      [&queue, test_duration, enqueue_batch_size, enqueue_delay]() {
        auto start_time = std::chrono::steady_clock::now();
        uint8_t data = 0;

        // Loop for the specified duration.
        while (std::chrono::steady_clock::now() - start_time < test_duration) {
          // Try to get a write pointer.
          auto write_ptr = queue.write_ptr();
          if (!write_ptr)
            continue;

          // Write data to the buffer.
          for (size_t j = 0; j < enqueue_batch_size; j++)
            write_ptr[j] = data++;

          // Commit the write.
          queue.commit_write();

          // Sleep for the specified duration.
          while (std::chrono::steady_clock::now() - start_time < test_duration)
            std::this_thread::sleep_for(enqueue_delay);
        }
      });

  // Dequeue thread.
  std::thread dequeue_thread(
      [&queue, test_duration, dequeue_batch_size, dequeue_delay]() {
        auto start_time = std::chrono::steady_clock::now();
        uint8_t expected = 0;

        // Loop for the specified duration.
        while (std::chrono::steady_clock::now() - start_time < test_duration) {

          // Try to get a read pointer.
          auto read_ptr = queue.read_ptr();
          if (!read_ptr)
            continue;

          // Check the data.
          for (size_t j = 0; j < dequeue_batch_size; j++)
            ASSERT_EQ(read_ptr[j], expected++);

          // Commit the read.
          queue.commit_read();

          // Sleep for the specified duration.
          while (std::chrono::steady_clock::now() - start_time < test_duration)
            std::this_thread::sleep_for(dequeue_delay);
        }
      });

  enqueue_thread.join();
  dequeue_thread.join();
}

INSTANTIATE_TEST_SUITE_P(
    BatchedSPSCQueueTestSuite, BatchedSPSCQueueMultiThreadingTest,
    ::testing::Values(
        // 00
        std::make_tuple(std::chrono::seconds(10),     // test_duration
                        std::chrono::microseconds(0), // enqueue_delay
                        std::chrono::microseconds(0), // dequeue_delay
                        3000,                         // nb_slots
                        2,                            // enqueue_batch_size
                        3),                           // dequeue_batch_size
        // 01
        std::make_tuple(std::chrono::seconds(10),     // test_duration
                        std::chrono::microseconds(2), // enqueue_delay
                        std::chrono::microseconds(0), // dequeue_delay
                        3000,                         // nb_slots
                        2,                            // enqueue_batch_size
                        3),                           // dequeue_batch_size
        // 02
        std::make_tuple(std::chrono::seconds(10),     // test_duration
                        std::chrono::microseconds(0), // enqueue_delay
                        std::chrono::microseconds(1), // dequeue_delay
                        3000,                         // nb_slots
                        2,                            // enqueue_batch_size
                        3),                           // dequeue_batch_size
        // 03
        std::make_tuple(std::chrono::seconds(10),     // test_duration
                        std::chrono::microseconds(0), // enqueue_delay
                        std::chrono::microseconds(0), // dequeue_delay
                        3000,                         // nb_slots
                        3,                            // enqueue_batch_size
                        2),                           // dequeue_batch_size
        // 04
        std::make_tuple(std::chrono::seconds(10),     // test_duration
                        std::chrono::microseconds(2), // enqueue_delay
                        std::chrono::microseconds(0), // dequeue_delay
                        3000,                         // nb_slots
                        3,                            // enqueue_batch_size
                        2),                           // dequeue_batch_size
        // 05
        std::make_tuple(std::chrono::seconds(10),     // test_duration
                        std::chrono::microseconds(0), // enqueue_delay
                        std::chrono::microseconds(1), // dequeue_delay
                        3000,                         // nb_slots
                        3,                            // enqueue_batch_size
                        2),                           // dequeue_batch_size
        // 06
        std::make_tuple(std::chrono::seconds(10),     // test_duration
                        std::chrono::microseconds(0), // enqueue_delay
                        std::chrono::microseconds(0), // dequeue_delay
                        3000,                         // nb_slots
                        10,                           // enqueue_batch_size
                        1000),                        // dequeue_batch_size
        // 07
        std::make_tuple(std::chrono::seconds(10),     // test_duration
                        std::chrono::microseconds(2), // enqueue_delay
                        std::chrono::microseconds(0), // dequeue_delay
                        3000,                         // nb_slots
                        10,                           // enqueue_batch_size
                        1000),                        // dequeue_batch_size
        // 08
        std::make_tuple(std::chrono::seconds(10),     // test_duration
                        std::chrono::microseconds(0), // enqueue_delay
                        std::chrono::microseconds(1), // dequeue_delay
                        3000,                         // nb_slots
                        10,                           // enqueue_batch_size
                        1000),                        // dequeue_batch_size
        // 09
        std::make_tuple(std::chrono::seconds(10),     // test_duration
                        std::chrono::microseconds(0), // enqueue_delay
                        std::chrono::microseconds(0), // dequeue_delay
                        3000,                         // nb_slots
                        1000,                         // enqueue_batch_size
                        10),                          // dequeue_batch_size
        // 10
        std::make_tuple(std::chrono::seconds(10),     // test_duration
                        std::chrono::microseconds(2), // enqueue_delay
                        std::chrono::microseconds(0), // dequeue_delay
                        3000,                         // nb_slots
                        1000,                         // enqueue_batch_size
                        10),                          // dequeue_batch_size
        // 11
        std::make_tuple(std::chrono::seconds(10),     // test_duration
                        std::chrono::microseconds(0), // enqueue_delay
                        std::chrono::microseconds(1), // dequeue_delay
                        3000,                         // nb_slots
                        1000,                         // enqueue_batch_size
                        10)));                        // dequeue_batch_size
} // namespace holoflow
