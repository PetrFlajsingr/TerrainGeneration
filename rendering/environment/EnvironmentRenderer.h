//
// Created by petr on 12/27/19.
//

#ifndef TERRAINGENERATION_ENVIRONMENTRENDERER_H
#define TERRAINGENERATION_ENVIRONMENTRENDERER_H

#include <rendering/models/ModelRenderer.h>
#include <ui/elements/CameraController.h>
class EnvironmentRenderer {
public:
  explicit EnvironmentRenderer(ObjModelLoader &loader);

  void render(sdl2cpp::ui::CameraController &cameraController, float time);

  void setCloudRelativePosition(float cloudRelativePosition);
  void setWaterLevel(float waterLevel);

private:
  std::shared_ptr<GraphicsModelBase> cloudModel;
  std::shared_ptr<GraphicsModelBase> waterModel;

  std::shared_ptr<ge::gl::Program> cloudProgram;
  std::shared_ptr<ge::gl::Program> waterProgram;

  float cloudRelativePosition = 1000.0f;
  float waterLevel = 0.0f;
};

#endif // TERRAINGENERATION_ENVIRONMENTRENDERER_H
