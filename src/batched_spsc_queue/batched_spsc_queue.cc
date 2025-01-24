#include "batched_spsc_queue/batched_spsc_queue.hh"

#include <atomic>
#include <cstdint>

namespace holoflow {
BatchedSPSCQueue::BatchedSPSCQueue(size_t nb_slots, size_t enqueue_batch_size,
                                   size_t dequeue_batch_size,
                                   size_t element_size, uint8_t *buffer)
    : nb_slots_(nb_slots), enqueue_batch_size_(enqueue_batch_size),
      dequeue_batch_size_(dequeue_batch_size), element_size_(element_size),
      buffer_(buffer), write_idx_(0), read_idx_(0) {}

uint8_t *BatchedSPSCQueue::write_ptr() {
  if (nb_slots_ - writer_size() < enqueue_batch_size_ + 1) {
    return nullptr;
  }

  size_t write_idx = write_idx_.load(std::memory_order_relaxed);
  return buffer_ + write_idx * element_size_;
}

void BatchedSPSCQueue::commit_write() {
  size_t write_idx = write_idx_.load(std::memory_order_relaxed);
  size_t next_write_idx = write_idx + enqueue_batch_size_;
  if (next_write_idx == nb_slots_)
    next_write_idx = 0;

  write_idx_.store(next_write_idx, std::memory_order_release);
}

uint8_t *BatchedSPSCQueue::read_ptr() {
  if (reader_size() < dequeue_batch_size_)
    return nullptr;

  size_t read_idx = read_idx_.load(std::memory_order_relaxed);
  return buffer_ + read_idx * element_size_;
}

void BatchedSPSCQueue::commit_read() {
  size_t read_idx = read_idx_.load(std::memory_order_relaxed);
  size_t next_read_idx = read_idx + dequeue_batch_size_;
  if (next_read_idx == nb_slots_)
    next_read_idx = 0;

  read_idx_.store(next_read_idx, std::memory_order_release);
}

[[maybe_unused]] size_t BatchedSPSCQueue::size() {
  size_t write_idx = write_idx_.load(std::memory_order_acquire);
  size_t read_idx = read_idx_.load(std::memory_order_acquire);

  size_t diff = write_idx - read_idx;

  if (write_idx < read_idx)
    diff += nb_slots_;

  return diff;
}

void BatchedSPSCQueue::reset() {
  write_idx_.store(0, std::memory_order_release);
  read_idx_.store(0, std::memory_order_release);
}

void BatchedSPSCQueue::fill() {
  write_idx_.store(nb_slots_, std::memory_order_release);
  read_idx_.store(0, std::memory_order_release);
}

size_t BatchedSPSCQueue::writer_size() {
  size_t write_idx = write_idx_.load(std::memory_order_relaxed);
  size_t read_idx = read_idx_.load(std::memory_order_acquire);

  size_t diff = write_idx - read_idx;

  if (write_idx < read_idx)
    diff += nb_slots_;

  return diff;
}

size_t BatchedSPSCQueue::reader_size() {
  size_t write_idx = write_idx_.load(std::memory_order_acquire);
  size_t read_idx = read_idx_.load(std::memory_order_relaxed);

  size_t diff = write_idx - read_idx;

  if (write_idx < read_idx)
    diff += nb_slots_;

  return diff;
}
} // namespace holoflow
