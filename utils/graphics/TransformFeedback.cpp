//
// Created by petr on 11/17/19.
//

#include "TransformFeedback.h"

TransformFeedback::TransformFeedback() : ge::gl::OpenGLObject(nullptr) {
  getContext().glGenTransformFeedbacks(1, &feedbackHandle);
}

void TransformFeedback::changeBuffer(uint index, const TransformFeedback::Buffer &buffer) {
  getContext().glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackHandle);
  getContext().glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, buffer->getId());
  getContext().glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

void TransformFeedback::begin(GLenum primitiveType) {
  getContext().glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackHandle);
  getContext().glBeginTransformFeedback(primitiveType);
}

void TransformFeedback::end() { getContext().glEndTransformFeedback(); }
