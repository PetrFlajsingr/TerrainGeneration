//
// Created by petr on 12/5/19.
//

#include "ui_main.h"
void ui_main(int argc, char *argv[]) {
  using namespace sdl2cpp::ui;
  loc_assert(argc != 1, "Provide path for config");
  Conf config{argv[1]};

  auto mainLoop = std::make_shared<sdl2cpp::MainLoop>();

  const auto deviceData = config.get<DeviceData>("device").value();
  auto window = std::make_shared<sdl2cpp::Window>(deviceData.screen.width,
                                                  deviceData.screen.height);
  window->createContext("rendering", 430);
  mainLoop->addWindow("mainWindow", window);

  // init OpenGL
  ge::gl::init(SDL_GL_GetProcAddress);
  ge::gl::setHighDebugMessage();

  ge::gl::glClearColor(0, 0, 0, 1);

  setShaderLocation(config.get<std::string>("paths", "shaderLocation").value());

  const auto assetPath =
      config.get<std::string>("paths", "assetsLocation").value();


  sdl2cpp::ui::UIManager uiManager{window, String{assetPath + "/gui/fonts"}};

  auto btn1 = uiManager.createGUIObject<sdl2cpp::ui::Button>(
      glm::vec3{0, 0, 1}, glm::vec3{500, 500, 0});

  btn1->setMouseDblClicked([] {print("dbl clicked");}).setMouseClicked([] {print("clicked");});

  mainLoop->setIdleCallback([&]() {
    ge::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    uiManager.render(glm::ortho<float>(0, 1000, 0, 562.5, -1, 1));

    window->swap();
  });

  (*mainLoop)();
}
