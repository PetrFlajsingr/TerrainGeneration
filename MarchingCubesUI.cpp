//
// Created by petr on 1/1/20.
//

#include "MarchingCubesUI.h"

UI::UI(UIManager &uiManager) {
  printT(LogLevel::Info, "Initialising UI");
  auto perspective = PerspectiveProjection(0.1f, 500000.f, 1920.f / 1080, glm::degrees(60.f));
  cameraController =
      uiManager.createGUIObject<CameraController>(std::move(perspective), glm::vec3{0, 0, 0}, glm::vec3{1920, 1080, 0});

  lineFillBtn = uiManager.createGUIObject<sdl2cpp::ui::Button>(glm::vec3{0, 0, 1}, glm::vec3{160, 60, 0});
  lineFillBtn->text.setFont("arialbd", 40).setText(L"Line"_sw);

  shadowMapsBtn = uiManager.createGUIObject<sdl2cpp::ui::Button>(glm::vec3{0, 60, 1}, glm::vec3{160, 60, 0});
  shadowMapsBtn->text.setFont("arialbd", 20).setText(L"shad. maps"_sw);

  fpsLbl = uiManager.createGUIObject<Label>(glm::vec3{1300, 0, 1}, glm::vec3{220, 20, 0});
  fpsLbl->text.setFont("arialbd", 10);

  pauseMCBtn = uiManager.createGUIObject<Button>(glm::vec3{1700, 20, 1}, glm::vec3{150, 20, 0});
  pauseMCBtn->text.setFont("arialbd", 10).setText(L"MC on/off"_sw);

  speedLbl = uiManager.createGUIObject<Label>(glm::vec3{0, 620, 1}, glm::vec3{220, 50, 0});
  speedLbl->text.setFont("arialbd", 10).setColor(Color::white);
  movementSpeedSlider = uiManager.createGUIObject<Slider<float>>(glm::vec3{0, 700, 1}, glm::vec3{200, 50, 0});
  movementSpeedSlider->setColor(Color::transparent(Color::red, 0.5f));

  chunkInfoLbl = uiManager.createGUIObject<Label>(glm::vec3{0, 1000, 1}, glm::vec3{500, 20, 0});
  chunkInfoLbl->text.setFont("arialbd", 10).setColor(Color::white);

  terrain.octavesLbl = uiManager.createGUIObject<Label>(glm::vec3{1400, 350 + 500, 1}, glm::vec3{70, 30, 0});
  terrain.octavesLbl->text.setFont("arialbd", 10).setColor(Color::white).setText(L"Octaves:"_sw);
  terrain.gainLbl = uiManager.createGUIObject<Label>(glm::vec3{1400, 350 + 530, 1}, glm::vec3{70, 30, 0});
  terrain.gainLbl->text.setFont("arialbd", 10).setColor(Color::white).setText(L"Gain:"_sw);
  terrain.lacunarityLbl = uiManager.createGUIObject<Label>(glm::vec3{1400, 350 + 560, 1}, glm::vec3{70, 30, 0});
  terrain.lacunarityLbl->text.setFont("arialbd", 10).setColor(Color::white).setText(L"Lacunarity:"_sw);
  terrain.sharpnessLbl = uiManager.createGUIObject<Label>(glm::vec3{1400, 350 + 590, 1}, glm::vec3{70, 30, 0});
  terrain.sharpnessLbl->text.setFont("arialbd", 10).setColor(Color::white).setText(L"Sharpness:"_sw);
  terrain.valleyScaleLbl = uiManager.createGUIObject<Label>(glm::vec3{1400, 350 + 620, 1}, glm::vec3{70, 30, 0});
  terrain.valleyScaleLbl->text.setFont("arialbd", 10).setColor(Color::white).setText(L"Valley scale:"_sw);
  terrain.heightScaleLbl = uiManager.createGUIObject<Label>(glm::vec3{1400, 350 + 650, 1}, glm::vec3{70, 30, 0});
  terrain.heightScaleLbl->text.setFont("arialbd", 10).setColor(Color::white).setText(L"Height scale:"_sw);

  terrain.octavesValLbl = uiManager.createGUIObject<Label>(glm::vec3{1840, 350 + 500, 1}, glm::vec3{100, 30, 0});
  terrain.octavesValLbl->text.setFont("arialbd", 10).setColor(Color::white);
  terrain.gainValLbl = uiManager.createGUIObject<Label>(glm::vec3{1840, 350 + 530, 1}, glm::vec3{100, 30, 0});
  terrain.gainValLbl->text.setFont("arialbd", 10).setColor(Color::white);
  terrain.lacunarityValLbl = uiManager.createGUIObject<Label>(glm::vec3{1840, 350 + 560, 1}, glm::vec3{100, 30, 0});
  terrain.lacunarityValLbl->text.setFont("arialbd", 10).setColor(Color::white);
  terrain.sharpnessValLbl = uiManager.createGUIObject<Label>(glm::vec3{1840, 350 + 590, 1}, glm::vec3{100, 30, 0});
  terrain.sharpnessValLbl->text.setFont("arialbd", 10).setColor(Color::white);
  terrain.valleyScaleValLbl = uiManager.createGUIObject<Label>(glm::vec3{1840, 350 + 620, 1}, glm::vec3{100, 30, 0});
  terrain.valleyScaleValLbl->text.setFont("arialbd", 10).setColor(Color::white);
  terrain.heightScaleValLbl = uiManager.createGUIObject<Label>(glm::vec3{1840, 350 + 650, 1}, glm::vec3{100, 30, 0});
  terrain.heightScaleValLbl->text.setFont("arialbd", 10).setColor(Color::white);

  terrain.octavesSlider = uiManager.createGUIObject<Slider<unsigned int>>(glm::vec3{1520, 365 + 500, 1}, glm::vec3{315, 20, 0});
  terrain.octavesSlider->value.subscribe(
      [this](const auto &value) { terrain.octavesValLbl->text.setText(WString{std::to_wstring(value)}); });
  terrain.octavesSlider->setMin(1).setMax(16).setSliderValue(8);
  terrain.gainSlider = uiManager.createGUIObject<Slider<float>>(glm::vec3{1520, 365 + 530, 1}, glm::vec3{315, 20, 0});
  terrain.gainSlider->value.subscribe([this](const auto &value) { terrain.gainValLbl->text.setText(WString{std::to_wstring(value)}); });
  terrain.gainSlider->setMin(0.00001).setMax(100).setSliderValue(50);
  terrain.lacunaritySlider = uiManager.createGUIObject<Slider<float>>(glm::vec3{1520, 365 + 560, 1}, glm::vec3{315, 20, 0});
  terrain.lacunaritySlider->value.subscribe(
      [this](const auto &value) { terrain.lacunarityValLbl->text.setText(WString{std::to_wstring(value)}); });
  terrain.lacunaritySlider->setMin(0.00001).setMax(100).setSliderValue(50);
  terrain.sharpnessSlider = uiManager.createGUIObject<Slider<float>>(glm::vec3{1520, 365 + 590, 1}, glm::vec3{315, 20, 0});
  terrain.sharpnessSlider->value.subscribe(
      [this](const auto &value) { terrain.sharpnessValLbl->text.setText(WString{std::to_wstring(value)}); });
  terrain.sharpnessSlider->setMin(-1).setMax(1).setSliderValue(0.00001);
  terrain.valleyScaleSlider = uiManager.createGUIObject<Slider<float>>(glm::vec3{1520, 365 + 620, 1}, glm::vec3{315, 20, 0});
  terrain.valleyScaleSlider->value.subscribe(
      [this](const auto &value) { terrain.valleyScaleValLbl->text.setText(WString{std::to_wstring(value)}); });
  terrain.valleyScaleSlider->setMin(-1).setMax(1).setSliderValue(0.000000001);
  terrain.heightScaleSlider = uiManager.createGUIObject<Slider<float>>(glm::vec3{1520, 365 + 650, 1}, glm::vec3{315, 20, 0});
  terrain.heightScaleSlider->value.subscribe(
      [this](const auto &value) { terrain.heightScaleValLbl->text.setText(WString{std::to_wstring(value)}); });
  terrain.heightScaleSlider->setMin(0).setMax(50).setSliderValue(25);

  uiLbl = uiManager.createGUIObject<Label>(glm::vec3{0, 300, 1}, glm::vec3{140, 20, 0});
  uiLbl->text.setFont("arialbd", 10).setColor(Color::white).setText(L"Show/hide UI: "_sw);
  uiSwitch = uiManager.createGUIObject<Switch>(glm::vec3{140, 300, 1}, glm::vec3{30, 30, 0}, true);

  terrain.applyChangesBtn = uiManager.createGUIObject<sdl2cpp::ui::Button>(glm::vec3{1520, 1040, 1}, glm::vec3{150, 30, 0});
  terrain.applyChangesBtn->text.setFont("arialbd", 10).setText(L"Apply changes"_sw);
}

void UI::setVisible(bool visible) {
  auto visibility = visible ? Visibility::Visible : Visibility::Invisible;
  lineFillBtn->setVisibility(visibility);
  shadowMapsBtn->setVisibility(visibility);
  pauseMCBtn->setVisibility(visibility);
  fpsLbl->setVisibility(visibility);
  chunkInfoLbl->setVisibility(visibility);
  speedLbl->setVisibility(visibility);
  cameraController->setVisibility(visibility);
  movementSpeedSlider->setVisibility(visibility);

  terrain.octavesLbl->setVisibility(visibility);
  terrain.gainLbl->setVisibility(visibility);
  terrain.lacunarityLbl->setVisibility(visibility);
  terrain.sharpnessLbl->setVisibility(visibility);
  terrain.valleyScaleLbl->setVisibility(visibility);
  terrain.heightScaleLbl->setVisibility(visibility);
  terrain.octavesValLbl->setVisibility(visibility);
  terrain.gainValLbl->setVisibility(visibility);
  terrain.lacunarityValLbl->setVisibility(visibility);
  terrain.sharpnessValLbl->setVisibility(visibility);
  terrain.valleyScaleValLbl->setVisibility(visibility);
  terrain.heightScaleValLbl->setVisibility(visibility);
  terrain.octavesSlider->setVisibility(visibility);
  terrain.gainSlider->setVisibility(visibility);
  terrain.lacunaritySlider->setVisibility(visibility);
  terrain.sharpnessSlider->setVisibility(visibility);
  terrain.valleyScaleSlider->setVisibility(visibility);
  terrain.heightScaleSlider->setVisibility(visibility);
  terrain.applyChangesBtn->setVisibility(visibility);
}
