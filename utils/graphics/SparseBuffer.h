//
// Created by petr on 11/23/19.
//

#ifndef TERRAINGENERATION_SPARSEBUFFER_H
#define TERRAINGENERATION_SPARSEBUFFER_H

#include <geGL/Buffer.h>

class SparseBuffer : public ge::gl::Buffer {
public:
  template <typename T> SparseBuffer(const ge::gl::FunctionTablePointer &table, const std::vector<T> &data);
  template <typename T> explicit SparseBuffer(const std::vector<T> &data);
  SparseBuffer(const ge::gl::FunctionTablePointer &table, GLsizeiptr size, const void *data);
  explicit SparseBuffer(const ge::gl::FunctionTablePointer &table);
  SparseBuffer(GLsizeiptr size, const void *data);

  void pageCommitment(GLbitfield offset, GLbitfield size, bool commit, bool alignOffsetUp = false, bool alignSizeDown = false);

  void pageCommitment(bool commit);

private:
  int sparsePageSize;
  std::pair<int, int> alignToPageSize(int offset, int size, bool alignOffsetUp, bool alignSizeDown);
};

#endif // TERRAINGENERATION_SPARSEBUFFER_H
