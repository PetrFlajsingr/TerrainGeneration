//
// Created by petr on 10/19/19.
//

#include "Camera.h"

PerspectiveProjection::PerspectiveProjection(float, float, float aspectRatio,
                                             float fieldOfView)
    : near(near), far(far), aspectRatio(aspectRatio), fieldOfView(fieldOfView) {
}
float PerspectiveProjection::getAspectRatio() const { return aspectRatio; }
void PerspectiveProjection::setAspectRatio(float aspectRatio) {
  shouldUpdateMatrix = true;
  PerspectiveProjection::aspectRatio = aspectRatio;
}
float PerspectiveProjection::getFieldOfView() const { return fieldOfView; }
void PerspectiveProjection::setFieldOfView(float fieldOfView) {
  shouldUpdateMatrix = true;
  PerspectiveProjection::fieldOfView = fieldOfView;
}
float PerspectiveProjection::getNear() const { return near; }
void PerspectiveProjection::setNear(float near) {
  PerspectiveProjection::near = near;
}
float PerspectiveProjection::getFar() const { return far; }
void PerspectiveProjection::setFar(float far) {
  PerspectiveProjection::far = far;
}
