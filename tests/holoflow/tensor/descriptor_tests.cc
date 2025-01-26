#include "holoflow/tensor/descriptor.hh"

#include <gtest/gtest.h>

namespace holoflow {

TEST(TensorDescriptorTest, ConstructorInitialization) {
  TensorDescriptor desc("float", sizeof(float), {4, 4}, {16, 4});
  EXPECT_EQ(desc.type_name(), "float");
  EXPECT_EQ(desc.type_size(), sizeof(float));
  EXPECT_EQ(desc.shape(), std::vector<std::size_t>({4, 4}));
  EXPECT_EQ(desc.strides(), std::vector<std::size_t>({16, 4}));
}

TEST(TensorDescriptorTest, ConstructorThrowsForMismatchedShapeAndStrides) {
  EXPECT_DEATH(TensorDescriptor("float", sizeof(float), {4, 4}, {16}), "");
}

TEST(TensorDescriptorTest, ConstructorThrowsForIncompatibleStrides) {
  EXPECT_DEATH(TensorDescriptor("float", sizeof(float), {4, 4}, {8, 4}), "");
}

TEST(TensorDescriptorTest, ConstructorAllowsEmptyShape) {
  TensorDescriptor desc("float", sizeof(float), {}, {});
  EXPECT_TRUE(desc.shape().empty());
  EXPECT_TRUE(desc.strides().empty());
}

TEST(TensorDescriptorTest, EqualityOperator) {
  TensorDescriptor desc1("float", sizeof(float), {4, 4}, {16, 4});
  TensorDescriptor desc2("float", sizeof(float), {4, 4}, {16, 4});
  EXPECT_TRUE(desc1 == desc2);
}

TEST(TensorDescriptorTest, InequalityOperator) {
  TensorDescriptor desc1("float", sizeof(float), {4, 4}, {16, 4});
  TensorDescriptor desc2("float", sizeof(float), {4, 5}, {20, 4});
  EXPECT_TRUE(desc1 != desc2);
}

TEST(TensorDescriptorTest, SizeInBytesNonEmpty) {
  TensorDescriptor desc("float", sizeof(float), {4, 4}, {16, 4});
  EXPECT_EQ(desc.size_in_bytes(), 4 * 16); // 4 rows * 16 bytes per row
}

TEST(TensorDescriptorTest, SizeInBytesEmpty) {
  TensorDescriptor desc("float", sizeof(float), {}, {});
  EXPECT_EQ(desc.size_in_bytes(), 0);
}

} // namespace holoflow
