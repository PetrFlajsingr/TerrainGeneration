//
// Created by petr on 11/17/19.
//

#include "TransformFeedback.h"

TransformFeedback::TransformFeedback() : ge::gl::OpenGLObject(nullptr) {
  getContext().glGenTransformFeedbacks(1, &feedbackHandle);
}

void TransformFeedback::changeBuffer(uint index,
                                     TransformFeedback::Buffer buffer) {
  assert(index < buffers.size());
  getContext().glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackHandle);
  getContext().glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0,
                                buffer->getId());
  getContext().glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
  buffers[index] = std::move(buffer);
}

void TransformFeedback::begin(GLenum primitiveType) {
  getContext().glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackHandle);
  getContext().glBeginTransformFeedback(primitiveType);
}

void TransformFeedback::end() { getContext().glEndTransformFeedback(); }

ge::gl::Buffer *TransformFeedback::getBuffer(uint index) {
  assert(index < buffers.size());
  return buffers[index].get();
}

uint TransformFeedback::bufferCount() const { return buffers.size(); }
