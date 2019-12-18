//
// Created by petr on 12/12/19.
//

#include "SceneLoader.h"
#include "utils/types/Zip.h"

SceneLoader::SceneLoader(const std::string &assetsPath, const std::string &sceneName) : modelLoader(assetsPath + "/models") {
  doc.LoadFile((assetsPath + "/scenes/" + sceneName + ".xml").c_str());
  xmlIter = doc.FirstChildElement("scene")->FirstChildElement("models");
}

bool SceneLoader::isValid() const { return valid; }

SceneLoader::iterator SceneLoader::begin() { return iterator{*this}; }

SceneLoader::iterator SceneLoader::end() { return iterator{*this, true}; }

std::vector<std::shared_ptr<GraphicsModelBase>> SceneLoader::getNextModel() {
  if (xmlIter == nullptr) {
    valid = false;
    return {};
  }
  auto model = xmlIter->FirstChild()->ToElement();
  xmlIter = xmlIter->NextSibling();
  std::shared_ptr<GraphicsModel<ge::gl::Buffer>> result = std::dynamic_pointer_cast<GraphicsModel<ge::gl::Buffer>>(
      modelLoader.loadModel(model->FindAttribute("filename")->Value(), model->FindAttribute("name")->Value()));
  if (strncmp(model->Value(), "model", 5) == 0) {
    auto posEl = model->FirstChildElement("position");
    glm::vec3 position{posEl->FloatAttribute("x", 0), posEl->FloatAttribute("y", 0), posEl->FloatAttribute("z", 0)};
    auto rotEl = model->FirstChildElement("rotation");
    glm::vec3 rotation{rotEl->FloatAttribute("x", 0), rotEl->FloatAttribute("y", 0), rotEl->FloatAttribute("z", 0)};
    auto scaleEl = model->FirstChildElement("scale");
    float scale = scaleEl->FloatText();
    result->setPosition(position);
    result->setRotation(rotation);
    result->setScale(scale);
    return {result};
  } else {
    std::vector<std::shared_ptr<GraphicsModelBase>> res;
    auto genPosStartEl = model->FirstChildElement("genPosStart");
    glm::vec3 genStart{genPosStartEl->FloatAttribute("x", 0), genPosStartEl->FloatAttribute("y", 0),
                       genPosStartEl->FloatAttribute("z", 0)};
    auto genPosStepEl = model->FirstChildElement("genPosStep");
    glm::vec3 genStep{genPosStepEl->FloatAttribute("x", 0), genPosStepEl->FloatAttribute("y", 0),
                      genPosStepEl->FloatAttribute("z", 0)};
    const auto instanceCount = model->FirstChildElement("instanceCount")->IntText(0);
    const auto scale = model->FirstChildElement("scale")->FloatText(0);
    using namespace MakeRange;
    int cnt = 0;
    int genAxisCount = 0;
    if (genStep.x > 0) {
      ++genAxisCount;
    } else {
      genStep.x += 0.000001f;
    }
    if (genStep.y > 0) {
      ++genAxisCount;
    } else {
      genStep.y += 0.000001f;
    }
    if (genStep.z > 0) {
      ++genAxisCount;
    } else {
      genStep.z += 0.000001f;
    }
    const auto genEnd = genStart + genStep * (std::pow<float>(instanceCount, 1.f / genAxisCount));
    for (auto [x, y, z] : range<float, 3>({genStart.x, genStart.y, genStart.z}, {genEnd.x, genEnd.y, genEnd.z},
                                          {genStep.x, genStep.y, genStep.z})) {
      auto mod = std::make_shared<GraphicsModel<ge::gl::Buffer>>(*result);
      mod->setId(result->getId() + std::to_string(cnt));
      mod->setPosition(x, y, z);
      mod->setScale(scale);
      res.emplace_back(mod);
      ++cnt;
      if (cnt == instanceCount) {
        break;
      }
    }
    return res;
  }
}

SceneLoader::iterator::iterator(SceneLoader &loader, bool isEnd) : loader(&loader) {
  if (!isEnd)
    currentModels = loader.getNextModel();
}

SceneLoader::iterator::iterator(const SceneLoader::iterator &other) { loader = other.loader; }

SceneLoader::iterator &SceneLoader::iterator::operator=(const SceneLoader::iterator &other) {
  loader = other.loader;
  currentModels = other.currentModels;
  return *this;
}

bool SceneLoader::iterator::operator==([[maybe_unused]] const SceneLoader::iterator &rhs) const { return !loader->isValid(); }

bool SceneLoader::iterator::operator!=(const SceneLoader::iterator &rhs) const { return !(rhs == *this); }

SceneLoader::iterator &SceneLoader::iterator::operator++() {
  currentModels = loader->getNextModel();
  return *this;
}

SceneLoader::iterator::value_type SceneLoader::iterator::operator*() { return currentModels; }
