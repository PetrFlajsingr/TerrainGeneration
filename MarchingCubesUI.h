//
// Created by petr on 1/1/20.
//

#ifndef TERRAINGENERATION_MARCHINGCUBESUI_H
#define TERRAINGENERATION_MARCHINGCUBESUI_H
#include <ui/elements.h>
#include <ui/managers.h>

using namespace sdl2cpp::ui;

struct UI {

  explicit UI(UIManager &uiManager);
  void setVisible(bool visible);
  std::shared_ptr<Button> lineFillBtn;
  std::shared_ptr<Button> shadowMapsBtn;
  std::shared_ptr<Button> pauseMCBtn;
  std::shared_ptr<Label> fpsLbl;
  std::shared_ptr<Label> chunkInfoLbl;
  std::shared_ptr<Label> speedLbl;
  std::shared_ptr<CameraController> cameraController;
  std::shared_ptr<Slider<float>> movementSpeedSlider;
  std::shared_ptr<Label> uiLbl;
  std::shared_ptr<Switch> uiSwitch;

  struct {
    std::shared_ptr<Label> octavesLbl;
    std::shared_ptr<Label> gainLbl;
    std::shared_ptr<Label> lacunarityLbl;
    std::shared_ptr<Label> sharpnessLbl;
    std::shared_ptr<Label> valleyScaleLbl;
    std::shared_ptr<Label> heightScaleLbl;

    std::shared_ptr<Label> octavesValLbl;
    std::shared_ptr<Label> gainValLbl;
    std::shared_ptr<Label> lacunarityValLbl;
    std::shared_ptr<Label> sharpnessValLbl;
    std::shared_ptr<Label> valleyScaleValLbl;
    std::shared_ptr<Label> heightScaleValLbl;

    std::shared_ptr<Slider<unsigned int>> octavesSlider;
    std::shared_ptr<Slider<float>> gainSlider;
    std::shared_ptr<Slider<float>> lacunaritySlider;
    std::shared_ptr<Slider<float>> sharpnessSlider;
    std::shared_ptr<Slider<float>> valleyScaleSlider;
    std::shared_ptr<Slider<float>> heightScaleSlider;

    std::shared_ptr<Button> applyChangesBtn;
  } terrain;
};


#endif // TERRAINGENERATION_MARCHINGCUBESUI_H
