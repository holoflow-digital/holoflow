#include "holoflow/tensor/tensor.hh"

namespace holoflow {

Tensor::Tensor(const TensorDescriptor &desc, std::byte *data)
    : desc_(desc), data_(data) {}

const TensorDescriptor &Tensor::desc() const { return desc_; }

} // namespace holoflow
