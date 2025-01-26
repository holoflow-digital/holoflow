#include "holoflow/tensor/descriptor.hh"
#include "holoflow/tensor/tensor.hh"

#include <memory>

#include <gtest/gtest.h>

namespace holoflow {

TEST(TensorTest, ConstructorInitialization) {
  TensorDescriptor desc("float", sizeof(float), {4, 4}, {16, 4});
  auto buffer = std::make_unique<std::byte[]>(desc.size_in_bytes());
  Tensor tensor(desc, buffer.get());
  EXPECT_EQ(tensor.desc(), desc);
  EXPECT_EQ(tensor.data<float>(), (float *)buffer.get());
}

TEST(TensorTest, DataAccessCorrectType) {
  TensorDescriptor desc("uint16_t", sizeof(uint16_t), {4, 4}, {8, 2});
  auto buffer = std::make_unique<std::byte[]>(desc.size_in_bytes());
  Tensor tensor(desc, buffer.get());
  uint16_t *data = tensor.data<uint16_t>();
  data[0] = 42;
  EXPECT_EQ(data[0], 42);
}

TEST(TensorTest, DataAccessIncorrectTypeSize) {
  TensorDescriptor desc("float", sizeof(float), {4, 4}, {16, 4});
  auto buffer = std::make_unique<std::byte[]>(desc.size_in_bytes());
  Tensor tensor(desc, buffer.get());
  EXPECT_DEATH(tensor.data<std::byte>(), "");
}

} // namespace holoflow
