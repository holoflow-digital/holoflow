#include "holoflow/tensor/tensor.hh"

#include <cstdint>
#include <iostream>

#include <glog/logging.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

int main() {
  // Define a tensor descriptor for a 4x4 matrix of uint16_t elements.
  holoflow::TensorDescriptor desc{
      "uint16_t", sizeof(uint16_t), {4, 4}, {16, 4}};

  // Allocate buffer.
  auto buffer = std::make_unique<std::byte[]>(desc.size_in_bytes());

  // Create a tensor using the descriptor and the buffer.
  holoflow::Tensor tensor(desc, buffer.get());

  // Access the raw data of the tensor as uint16_t.
  uint16_t *tensor_data = tensor.data<uint16_t>();

  // Initialize the tensor data with some values.
  for (std::size_t i = 0; i < desc.shape()[0]; ++i) {
    for (std::size_t j = 0; j < desc.shape()[1]; ++j) {
      tensor_data[i * desc.shape()[1] + j] =
          static_cast<uint16_t>(i * desc.shape()[1] + j);
    }
  }

  // Print the tensor values.
  std::cout << "Tensor values:\n";
  for (std::size_t i = 0; i < desc.shape()[0]; ++i) {
    for (std::size_t j = 0; j < desc.shape()[1]; ++j) {
      std::cout << tensor_data[i * desc.shape()[1] + j] << " ";
    }
    std::cout << "\n";
  }

  // Attempting to access data as an invalid type to demonstrate safety checks.
  int *invalid_data = tensor.data<int>();

  return 0;
}

#pragma GCC diagnostic pop
