//
// Created by petr on 11/23/19.
//

#include "SparseBuffer.h"
SparseBuffer::SparseBuffer(GLsizeiptr size, const void *data)
    : Buffer(size, data, GL_SPARSE_STORAGE_BIT_ARB) {
  getContext().glGetIntegerv(GL_SPARSE_BUFFER_PAGE_SIZE_ARB, &sparsePageSize);
}

SparseBuffer::SparseBuffer(const ge::gl::FunctionTablePointer &table)
    : Buffer(table) {
  getContext().glGetIntegerv(GL_SPARSE_BUFFER_PAGE_SIZE_ARB, &sparsePageSize);
}

SparseBuffer::SparseBuffer(const ge::gl::FunctionTablePointer &table,
                           GLsizeiptr size, const void *data)
    : Buffer(table, size, data, GL_SPARSE_STORAGE_BIT_ARB) {
  getContext().glGetIntegerv(GL_SPARSE_BUFFER_PAGE_SIZE_ARB, &sparsePageSize);
}

template <typename T>
SparseBuffer::SparseBuffer(const std::vector<T> &data)
    : Buffer(data, GL_SPARSE_STORAGE_BIT_ARB) {
  getContext().glGetIntegerv(GL_SPARSE_BUFFER_PAGE_SIZE_ARB, &sparsePageSize);
}

template <typename T>
SparseBuffer::SparseBuffer(const ge::gl::FunctionTablePointer &table,
                           const std::vector<T> &data)
    : Buffer(table, data, GL_SPARSE_STORAGE_BIT_ARB) {
  getContext().glGetIntegerv(GL_SPARSE_BUFFER_PAGE_SIZE_ARB, &sparsePageSize);
}

void SparseBuffer::pageCommitment(GLbitfield offset, GLbitfield size,
                                  bool commit, bool alignOffsetUp,
                                  bool alignSizeDown) {
  auto [aOffset, aSize] =
      alignToPageSize(offset, size, alignOffsetUp, alignSizeDown);
  getContext().glNamedBufferPageCommitmentEXT(getId(), aOffset, aSize, commit);
}

std::pair<int, int> SparseBuffer::alignToPageSize(int offset, int size,
                                                  bool alignOffsetUp,
                                                  bool alignSizeDown) {
  const auto alignedOffset =
      alignOffsetUp ? offset + (sparsePageSize - offset % sparsePageSize)
                    : offset - (offset % sparsePageSize);
  if (offset + size == getSize()) {
    return {alignedOffset, size};
  }
  const auto alignedSize = size + (sparsePageSize - size % sparsePageSize);
  return {alignedOffset, alignedSize};
}
