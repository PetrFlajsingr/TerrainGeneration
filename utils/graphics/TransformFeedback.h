//
// Created by petr on 11/17/19.
//

#ifndef TERRAINGENERATION_TRANSFORMFEEDBACK_H
#define TERRAINGENERATION_TRANSFORMFEEDBACK_H

#include <geGL/Buffer.h>
#include <geGL/OpenGLObject.h>
#include <meta/geGL.h>
#include <utility>
#include <vector>

class TransformFeedback : public ge::gl::OpenGLObject {
  using Buffer = std::shared_ptr<ge::gl::Buffer>;

public:
  TransformFeedback();

  void changeBuffer(uint index, const Buffer &buffer);

  // zabije gcc:
  // template <geGLBufferShared... Buffers> void setBuffers(Buffers &... buffers) {
  template <typename... Buffers> void setBuffers(Buffers &... buffers) {
    getContext().glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackHandle);
    uint idx = 0;
    for (auto &buffer : {buffers...}) {
      getContext().glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, idx, buffer->getId());
      ++idx;
    }
    getContext().glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
  }

  void begin(GLenum primitiveType);
  void end();

private:
  GLuint feedbackHandle;
};

#endif // TERRAINGENERATION_TRANSFORMFEEDBACK_H
