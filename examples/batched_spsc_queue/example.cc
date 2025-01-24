#include "batched_spsc_queue/batched_spsc_queue.hh"

#include <cstdint>
#include <thread>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

int main() {
  constexpr size_t nb_slots = 1000;
  constexpr size_t enqueue_batch_size = 4;
  constexpr size_t dequeue_batch_size = 8;
  constexpr size_t element_size = 512 * 512; // An image of 512x512 pixels.
  constexpr size_t buffer_size = nb_slots * element_size;

  // Note: The actual capacity of the queue is nb_slots - enqueue_batch_size.
  // In this example, the queue can hold 996 elements.
  std::vector<uint8_t> buffer(buffer_size);
  holoflow::BatchedSPSCQueue queue(nb_slots, enqueue_batch_size,
                                   dequeue_batch_size, element_size,
                                   buffer.data());

  // Enqueue thread.
  std::thread enqueue_thread([&queue]() {
    while (true) {
      // Try to get a write pointer.
      auto write_ptr = queue.write_ptr();
      if (!write_ptr)
        continue; // Queue is full.

      // Write data to the buffer.
      write_ptr[0] = 0;
      write_ptr[1] = 1;
      // ...

      // Commit the write.
      queue.commit_write();
    }
  });

  // Dequeue thread.
  std::thread dequeue_thread([&queue]() {
    while (true) {
      // Try to get a read pointer.
      auto read_ptr = queue.read_ptr();
      if (!read_ptr)
        continue; // Queue is empty.

      // Read the data from the buffer.
      uint8_t data0 = read_ptr[0];
      uint8_t data1 = read_ptr[1];
      // ...

      // Commit the read.
      queue.commit_read();
    }
  });

  enqueue_thread.join();
  dequeue_thread.join();
}

#pragma GCC diagnostic pop
