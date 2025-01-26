#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace holoflow {

/**
 * @brief Describes the metadata of a tensor, including type, shape, and
 * strides.
 */
class TensorDescriptor {
public:
  /**
   * @brief Constructs a TensorDescriptor object with the specified type, shape,
   * and strides.
   *
   * @param type_name A string representing the name of the tensor's data type
   * (e.g., "float", "int").
   * @param type_size The size of the tensor's data type in bytes (e.g., 4 for a
   * 32-bit float).
   * @param shape A vector specifying the dimensions of the tensor. Each element
   * represents the size of the tensor in the corresponding dimension.
   * @param strides A vector specifying the strides of the tensor in bytes. Each
   * element represents the step size in memory to move to the next element
   * along the corresponding dimension.
   *
   * @warning Exits the program if the provided strides are incompatible with
   * the shape and type size. This ensures that the tensor layout is valid and
   * avoids undefined behavior during access.
   *
   * The constructor initializes the tensor descriptor with the given parameters
   * and validates that the provided strides are compatible with the specified
   * shape and type size. If the strides are invalid, the program will terminate
   * to prevent misuse of an improperly defined tensor descriptor.
   */
  TensorDescriptor(const std::string &type_name, std::size_t type_size,
                   const std::vector<std::size_t> &shape,
                   const std::vector<std::size_t> &strides);

  /**
   * @brief Calculates the total size in bytes of the tensor.
   *
   * Takes into account the shape, strides, and type size to compute the memory
   * required to store the tensor, including any padding bytes.
   *
   * @return The total size in bytes of the tensor.
   */
  std::size_t size_in_bytes() const;

  /**
   * @brief Gets the name of the tensor's data type.
   * @return The type name as a string (e.g., "float", "int").
   */
  const std::string &type_name() const;

  /**
   * @brief Gets the size of the tensor's data type in bytes.
   * @return The size of the data type in bytes (e.g., 4 for a 32-bit float).
   */
  std::size_t type_size() const;

  /**
   * @brief Gets the dimensions of the tensor.
   * @return A vector representing the size of the tensor in each dimension.
   */
  const std::vector<std::size_t> &shape() const;

  /**
   * @brief Gets the strides of the tensor in bytes.
   * @return A vector representing the step size in memory for each dimension.
   */
  const std::vector<std::size_t> &strides() const;

  /**
   * @brief Checks for equality between two tensor descriptors.
   *
   * @param other The other tensor descriptor to compare with.
   * @return True if the two descriptors are equal, otherwise false.
   * @note Equality does not take into account the strides.
   */
  bool operator==(const TensorDescriptor &other) const;

  /**
   * @brief Checks for inequality between two tensor descriptors.
   *
   * @param other The other tensor descriptor to compare with.
   * @return True if the two descriptors are not equal, otherwise false.
   * @note Inequality does not take into account the strides.
   */
  bool operator!=(const TensorDescriptor &other) const;

private:
  std::string type_name_;
  std::size_t type_size_;
  std::vector<std::size_t> shape_;
  std::vector<std::size_t> strides_;
};
} // namespace holoflow
