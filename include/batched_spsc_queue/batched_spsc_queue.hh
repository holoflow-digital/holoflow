#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE 64
#endif

namespace holoflow {
/**
 * @class BatchedSPSCQueue
 * @brief A high-performance, lock-free, single-producer single-consumer (SPSC)
 * queue designed for batched operations.
 *
 * This queue allows the producer to enqueue a batch of elements and the
 * consumer to dequeue a batch of elements, improving throughput by reducing the
 * frequency of synchronization operations.
 *
 * The queue guarantees that a batch of elements is always contiguous in memory,
 * which means the queue can be used to extend a batch size in a context of
 * tensor processing, for example.
 *
 * This queue implementation is based on a acquire/commit pattern. The producer
 * acquires a pointer to the next batch of elements to be written, writes the
 * data, and then commits the write operation. The consumer acquires a pointer
 * to the next batch of elements to be read, reads the data, and then commits
 * the read operation. This pattern allows the producer and consumer to work
 * directly on the buffer without having to allocate memory for each batch or to
 * copy data between buffers.
 *
 * @note The actual capacity of the queue is `nb_slots - enqueue_batch_size`.
 * This is because circular buffer needs to keep one slot empty to distinguish
 * between full and empty states. Given that `nb_slots` must be a multiple of
 * `enqueue_batch_size`, the actual capacity is not `nb_slots - 1` but
 * `nb_slots - enqueue_batch_size`.
 *
 * @warning The queue is subject to the following constraints, if not respected,
 * the behavior is undefined:
 * - The number of slots must be a multiple of the enqueue and dequeue batch
 * sizes.
 * - The buffer must be pre-allocated with a size of at least
 * `nb_slots * element_size`
 * - A single thread must be used for enqueue operations.
 * - A single thread must be used for dequeue operations.
 * - Each call to commit_write() must be preceded by a call to write_ptr().
 * - The whole batch must be written before committing the write operation.
 * - The pointer returned by write_ptr() must not be used after commit_write()
 * has been called.
 * - Each call to commit_read() must be preceded by a call to read_ptr().
 * - The pointer returned by read_ptr() must not be used after commit_read() has
 * been called.
 *
 * @warning The methods `reset()` and `fill()` are not thread-safe and should
 * not be called in production code. They are provided for testing and
 * benchmarking purposes only.
 *
 * The following example demonstrates how to use the `BatchedSPSCQueue` class:
 * @include example/example.cc
 */
class BatchedSPSCQueue {
public:
  /**
   * @brief Constructs a new `BatchedSPSCQueue` object.
   *
   * @param nb_slots The number of slots in the circular buffer. Must be a
   * multiple of `enqueue_batch_size` and `dequeue_batch_size`.
   *
   * @param enqueue_batch_size The number of elements that are enqueued in a
   * single batch.
   *
   * @param dequeue_batch_size The number of elements that are dequeued in a
   * single batch.
   *
   * @param element_size The size of each element in bytes.
   *
   * @param buffer A pre-allocated memory block for storing elements. The buffer
   * must be allocated with a size of at least `nb_slots * element_size` bytes.
   *
   * @note The actual capacity of the queue is `nb_slots - enqueue_batch_size`.
   *
   * @warning This constructor will lead to undefined behavior if the
   * following constraints are not respected:
   * - The number of slots must be a multiple of the enqueue and dequeue batch
   * sizes.
   * - The buffer must be pre-allocated with a size of at least
   * `nb_slots * element_size`
   */
  BatchedSPSCQueue(size_t nb_slots, size_t enqueue_batch_size,
                   size_t dequeue_batch_size, size_t element_size,
                   uint8_t *buffer);

  /**
   * @brief Returns a pointer to the next batch of elements to be written.
   *
   * @return A pointer to the next batch of elements to be written, if the
   * queue has enough capacity. Otherwise, returns `nullptr`.
   *
   * @note Not calling `commit_write()` after calling this method does not lead
   * to undefined behavior. This can be leveraged to cancel the enqueue
   * operation.
   *
   * @warning This method will lead to undefined behavior if the following
   * constraints are not respected:
   * - The pointer returned by this method must not be used after calling
   * `commit_write()`.
   * - The whole batch must be written before committing the write operation.
   */
  uint8_t *write_ptr();

  /**
   * @brief Commits the write operation.
   *
   * @warning This method will lead to undefined behavior if the following
   * constraints are not respected:
   * - Each call to this method must be preceded by a successful call to
   * `write_ptr()`.
   * - The pointer returned by `write_ptr()` must not be used after calling this
   * method.
   * - The whole batch must be written before committing the write operation.
   */
  void commit_write();

  /**
   * @brief Returns a pointer to the next batch of elements to be read.
   *
   * @return A pointer to the next batch of elements to be read, if the queue
   * has enough elements. Otherwise, returns `nullptr`.
   *
   * @note Not calling `commit_read()` after calling this method does not lead
   * to undefined behavior. This can be leveraged to cancel the dequeue
   * operation.
   *
   * @warning This method will lead to undefined behavior if the following
   * constraints are not respected:
   * - The pointer returned by this method must not be used after calling
   * `commit_read()`.
   */
  uint8_t *read_ptr();

  /**
   * @brief Commits the read operation.
   *
   * @warning This method will lead to undefined behavior if the following
   * constraints are not respected:
   * - Each call to this method must be preceded by a successful call to
   * `read_ptr()`.
   * - The pointer returned by `read_ptr()` must not be used after calling this
   * method.
   */
  void commit_read();

  /**
   * @brief Returns the number of elements in the queue.
   *
   * @return The number of elements in the queue.
   */
  size_t size();

  /**
   * @brief Resets the queue.
   *
   * @warning This method is not thread-safe and should not be called in
   * production code. It is provided for testing and benchmarking purposes
   * only.
   */
  void reset();

  /**
   * @brief Fills the queue.
   *
   * @warning This method is not thread-safe and should not be called in
   * production code. It is provided for testing and benchmarking purposes
   * only.
   */
  void fill();

private:
  /**
   * @brief Returns the number of elements in the queue.
   *
   * @warning This method is only thread-safe for the writer thread.
   *
   * @return The number of elements in the queue.
   */
  size_t writer_size();

  /**
   * @brief Returns the number of elements in the queue.
   *
   * @warning This method is only thread-safe for the reader thread.
   *
   * @return The number of elements in the queue.
   */
  size_t reader_size();

private:
  /// The number of slots in the circular buffer.
  size_t nb_slots_;

  /// The number of elements to be enqueued in a single batch.
  size_t enqueue_batch_size_;

  /// The number of elements to be dequeued in a single batch.
  size_t dequeue_batch_size_;

  /// The size of each element in bytes.
  size_t element_size_;

  /// A pre-allocated memory block for storing elements.
  uint8_t *buffer_;

  /// The current write index.
  alignas(CACHE_LINE_SIZE) std::atomic<size_t> write_idx_;

  /// The current read index.
  alignas(CACHE_LINE_SIZE) std::atomic<size_t> read_idx_;
};

} // namespace holoflow
