#include "holoflow/tensor/descriptor.hh"

#include <numeric>

#include <glog/logging.h>

namespace holoflow {

TensorDescriptor::TensorDescriptor(const std::string &type_name,
                                   std::size_t type_size,
                                   const std::vector<std::size_t> &shape,
                                   const std::vector<std::size_t> &strides)
    : type_name_(type_name), type_size_(type_size), shape_(shape),
      strides_(strides) {
  CHECK_EQ(shape.size(), strides.size())
      << ": Shape and strides must have the same number of dimensions.";

  if (shape.empty())
    return;

  size_t last = shape.size() - 1;
  CHECK_GE(strides[last], type_size_)
      << ": Stride is not big enough to hold elements!";

  for (size_t i = 1; i < shape.size(); ++i) {
    size_t next = shape.size() - i;
    size_t curr = shape.size() - i - 1;
    CHECK_GE(strides[curr], shape[next] * strides[next])
        << ": Stride at dimension " << curr
        << " is not big enough to hold elements!";
  }
}

const std::string &TensorDescriptor::type_name() const { return type_name_; }

std::size_t TensorDescriptor::type_size() const { return type_size_; }

const std::vector<std::size_t> &TensorDescriptor::shape() const {
  return shape_;
}

const std::vector<std::size_t> &TensorDescriptor::strides() const {
  return strides_;
}

bool TensorDescriptor::operator==(const TensorDescriptor &other) const {
  return type_name_ == other.type_name_ && type_size_ == other.type_size_ &&
         shape_ == other.shape_;
}

bool TensorDescriptor::operator!=(const TensorDescriptor &other) const {
  return !(*this == other);
}

std::size_t TensorDescriptor::size_in_bytes() const {
  if (shape_.empty() || strides_.empty()) {
    return 0;
  }
  return shape_.front() * strides_.front();
}

} // namespace holoflow
