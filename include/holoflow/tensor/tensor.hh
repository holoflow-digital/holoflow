#include "holoflow/tensor/descriptor.hh"

#include <cstddef>

#include <glog/logging.h>

namespace holoflow {
/**
 * @brief Represents a tensor object containing metadata and raw data.
 *
 * Provides access to tensor metadata via its descriptor and raw data via
 * type-safe accessors.
 *
 * The following example demonstrates how to use the `Tensor` class:
 * @include examples/holoflow/tensor/example.cc
 */
class Tensor {
public:
  /**
   * @brief Constructs a tensor object with a descriptor and raw data.
   *
   * @param desc The tensor descriptor describing the metadata.
   * @param data A pointer to the raw data of the tensor.
   */
  Tensor(const TensorDescriptor &desc, std::byte *data);

  /**
   * @brief Gets the tensor descriptor.
   *
   * @return A constant reference to the tensor descriptor.
   */
  const TensorDescriptor &desc() const;

  /**
   * @brief Accesses the tensor data as a specific type.
   *
   * @tparam T The type to cast the tensor data to. This type must match the
   *           type described by the tensor descriptor.
   *
   * @return A constant pointer to the tensor data casted to the specified type.
   *
   * @warning Exits the program if the size of T does not match the
   * type_size in the tensor descriptor.
   */
  template <typename T> T *data();
  template <typename T> const T *data() const;

private:
  /// The tensor descriptor describing metadata such as type, shape, and
  /// strides.
  TensorDescriptor desc_;

  /// A pointer to the raw data of the tensor.
  std::byte *data_;
};

template <typename T> const T *Tensor::data() const {
  CHECK_EQ(sizeof(T), desc_.type_size())
      << ": The type provided did not match the expected type size!";

  return reinterpret_cast<const T *>(data_);
}

template <typename T> T *Tensor::data() {
  CHECK_EQ(sizeof(T), desc_.type_size())
      << ": The type provided did not match the expected type size!";

  return reinterpret_cast<T *>(data_);
}
} // namespace holoflow
