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
                                  bool commit) {
  auto [aOffset, aSize] = alignToPageSize(offset, size);
  getContext().glNamedBufferPageCommitmentEXT(getId(), aOffset, aSize, commit);
}

std::pair<int, int> SparseBuffer::alignToPageSize(int offset, int size) {
  const auto offsetDiff = sparsePageSize - (offset % sparsePageSize);
  const auto alignedOffset = offset + offsetDiff;
  if (offset + size == getSize()) {
    return {alignedOffset, size - offsetDiff};
  }
  size -= offsetDiff;
  const auto alignedSize = size - (alignedOffset + size) % sparsePageSize;
  return {alignedOffset, alignedSize};
}
