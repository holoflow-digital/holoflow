#include "batched_spsc_queue/batched_spsc_queue.hh"

#include <cstdint>
#include <cstdlib>
#include <memory>

#include <gtest/gtest.h>

namespace holoflow {
class BatchedSPSCQueueCapacityTest
    : public ::testing::TestWithParam<std::tuple<size_t, size_t, size_t>> {};

TEST_P(BatchedSPSCQueueCapacityTest, Capacity_Is_Respected) {
  // Test parameters.
  auto [nb_slots, enqueue_batch_size, dequeue_batch_size] = GetParam();

  // Check predicates to avoid undefined behavior as specified in the
  // BatchedSPSCQueue documentation.
  if (nb_slots % enqueue_batch_size != 0)
    FAIL() << "nb_slots % enqueue_batch_size != 0";

  if (nb_slots % dequeue_batch_size != 0)
    FAIL() << "nb_slots % dequeue_batch_size != 0";

  // Prepare the buffer.
  size_t element_size = sizeof(uint8_t);
  size_t buffer_size = nb_slots * element_size;
  std::vector<uint8_t> buffer(buffer_size);

  // We loop for capacity iteration to test all possible internal shifts of the
  // read/write indexes. We loop X10 just to be sure :)
  for (size_t i = 0; i < nb_slots * 10; i++) {
    // Create the queue.
    BatchedSPSCQueue queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                           element_size, buffer.data());
    // Enqueue-Dequeue i elements to shift internal read/write indexes by i.
    // To make sure one can dequeue everything, we have to enqueue
    // dequeue_batch_size times and dequeue enqueue_batch_size times. (n * m) =
    // (m * n). A better approach would be to look for the first common multiple
    // of enqueue_batch_size and dequeue_batch_size.
    for (size_t j = 0; j < i; j++) {
      // Enqueues.
      for (size_t k = 0; k < dequeue_batch_size; k++) {
        ASSERT_TRUE(queue.write_ptr());
        queue.commit_write();
      }

      // Dequeues.
      for (size_t k = 0; k < enqueue_batch_size; k++) {
        ASSERT_TRUE(queue.read_ptr());
        queue.commit_read();
      }
    }

    // Queue should be empty now.
    ASSERT_EQ(queue.size(), 0);

    // The queue is now empty.
    // One should be able to enqueue nb_slots - enqueue_batch_size elements.
    // That is (nb_slots - enqueue_batch_size) / enqueue_batch_size times.
    size_t nb_enqueues = (nb_slots - enqueue_batch_size) / enqueue_batch_size;
    for (size_t j = 0; j < nb_enqueues; j++) {
      ASSERT_TRUE(queue.write_ptr());
      queue.commit_write();
    }

    // The queue should be full now.
    ASSERT_FALSE(queue.write_ptr());

    // A total of nb_enqueues * enqueue_batch_size elements have been enqueued.
    // One should be able to dequeue nb_enqueues * enqueue_batch_size elements.
    // That is (nb_enqueues * enqueue_batch_size) / dequeue_batch_size times.
    size_t nb_dequeues =
        (nb_enqueues * enqueue_batch_size) / dequeue_batch_size;
    for (size_t j = 0; j < nb_dequeues; j++) {
      ASSERT_TRUE(queue.read_ptr());
      queue.commit_read();
    }

    // The queue should be empty now.
    ASSERT_FALSE(queue.read_ptr());
  }
}

INSTANTIATE_TEST_SUITE_P(BatchedSPSCQueueCapacityTestSuite,
                         BatchedSPSCQueueCapacityTest,
                         ::testing::Values(
                             // 00
                             std::make_tuple(100, // nb_slots
                                             1,   // enqueue_batch_size
                                             1),  // dequeue_batch_size
                             // 01
                             std::make_tuple(100, // nb_slots
                                             1,   // enqueue_batch_size
                                             2),  // dequeue_batch_size
                             // 02
                             std::make_tuple(100, // nb_slots
                                             2,   // enqueue_batch_size
                                             1),  // dequeue_batch_size
                             // 03
                             std::make_tuple(100, // nb_slots
                                             2,   // enqueue_batch_size
                                             2),  // dequeue_batch_size
                             // 04
                             std::make_tuple(102, // nb_slots
                                             3,   // enqueue_batch_size
                                             1),  // dequeue_batch_size
                             // 05
                             std::make_tuple(102, // nb_slots
                                             1,   // enqueue_batch_size
                                             3),  // dequeue_batch_size
                             // 06
                             std::make_tuple(102, // nb_slots
                                             3,   // enqueue_batch_size
                                             3),  // dequeue_batch_size
                             // 07
                             std::make_tuple(102, // nb_slots
                                             3,   // enqueue_batch_size
                                             2),  // dequeue_batch_size
                             // 08
                             std::make_tuple(102, // nb_slots
                                             2,   // enqueue_batch_size
                                             3),  // dequeue_batch_size
                             // 09
                             std::make_tuple(105, // nb_slots
                                             1,   // enqueue_batch_size
                                             5),  // dequeue_batch_size
                             // 10
                             std::make_tuple(105, // nb_slots
                                             5,   // enqueue_batch_size
                                             1),  // dequeue_batch_size
                             // 11
                             std::make_tuple(105, // nb_slots
                                             5,   // enqueue_batch_size
                                             5),  // dequeue_batch_size
                             // 12
                             std::make_tuple(105, // nb_slots
                                             5,   // enqueue_batch_size
                                             3),  // dequeue_batch_size
                             // 13
                             std::make_tuple(105,  // nb_slots
                                             3,    // enqueue_batch_size
                                             5))); // dequeue_batch_size
} // namespace holoflow
