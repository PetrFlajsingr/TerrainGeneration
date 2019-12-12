//
// Created by petr on 12/12/19.
//

#include "SceneLoader.h"


SceneLoader::SceneLoader(std::string assetsPath, std::string sceneName) : modelLoader(assetsPath + "/models") {
  doc.LoadFile((assetsPath + "/scenes/" + sceneName + ".xml").c_str());
  xmlIter = doc.FirstChildElement("scene")->FirstChildElement("models");
}

bool SceneLoader::isValid() const { return valid; }

SceneLoader::iterator SceneLoader::begin() { return iterator{*this}; }

SceneLoader::iterator SceneLoader::end() { return iterator{*this, true}; }

std::shared_ptr<GraphicsModelBase> SceneLoader::getNextModel() {
  if (xmlIter == nullptr) {
    valid = false;
    return nullptr;
  }
  auto model = xmlIter->FirstChild()->ToElement();
  auto result = modelLoader.loadModel(model->FindAttribute("filename")->Value(), model->FindAttribute("name")->Value());
  if (strncmp(model->Value(), "model", 5) == 0) {
    auto posEl = model->FirstChildElement("position");
    glm::vec3 position {
        posEl->FloatAttribute("x", 0),
        posEl->FloatAttribute("y", 0),
        posEl->FloatAttribute("z", 0)
    };
    auto rotEl = model->FirstChildElement("rotation");
    glm::vec3 rotation {
        rotEl->FloatAttribute("x", 0),
        rotEl->FloatAttribute("y", 0),
        rotEl->FloatAttribute("z", 0)
    };
    auto scaleEl = model->FirstChildElement("scale");
    float scale = scaleEl->DoubleText();
    result->setPosition(position);
    result->setRotation(rotation);
    result->setScale(scale);
  } else {

  }
  xmlIter = xmlIter->NextSibling();
  return result;
}

SceneLoader::iterator::iterator(SceneLoader &loader, bool isEnd) : loader(&loader) {
  if (!isEnd)
    currentModel = loader.getNextModel();
}

SceneLoader::iterator::iterator(const SceneLoader::iterator &other) {
  loader = other.loader;
}

SceneLoader::iterator &
SceneLoader::iterator::operator=(const SceneLoader::iterator &other) {
  loader = other.loader;
  currentModel = other.currentModel;
  return *this;
}

bool SceneLoader::iterator::operator==(const SceneLoader::iterator &rhs) const {
  return !loader->isValid();
}

bool SceneLoader::iterator::operator!=(const SceneLoader::iterator &rhs) const {
  return !(rhs == *this);
}

SceneLoader::iterator &SceneLoader::iterator::operator++() {
  currentModel = loader->getNextModel();
  return *this;
}

std::shared_ptr<GraphicsModelBase> SceneLoader::iterator::operator*() {
  return currentModel;
}
