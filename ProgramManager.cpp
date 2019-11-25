//
// Created by petr on 10/10/19.
//

#include "ProgramManager.h"
#include <utility>

using namespace std::string_literals;

ManagedProgram::ManagedProgram(std::string name,
                               const ge::gl::Program::ShaderPointers &shaders)
    : Program(shaders), name(std::move(name)) {}

ManagedProgram::ManagedProgram(std::string name,
                               const ge::gl::FunctionTablePointer &table,
                               const ge::gl::Program::ShaderPointers &shaders)
    : Program(table, shaders), name(std::move(name)) {}

std::string_view ManagedProgram::getName() { return name; }

ProgramManager &ProgramManager::GetInstance() {
  static ProgramManager instance;
  return instance;
}
void ProgramManager::registerProgram(
    std::string_view name, const ProgramManager::ProgramPtr &program) {
  if (auto iter = std::find_if(
          programs.begin(), programs.end(),
          [name](const auto &record) { return record.first == name; });
      iter != programs.end()) {
    throw std::runtime_error("Program with name '"s + name.data() +
                             "' already registered");
  }
  programs.insert({name.data(), program});
}

ProgramManager::ProgramPtr ProgramManager::getProgram(std::string_view name) {
  if (auto iter = std::find_if(
          programs.begin(), programs.end(),
          [name](const auto &record) { return record.first == name; });
      iter != programs.end()) {
    return iter->second;
  }
  throw std::runtime_error("Program with name '"s + name.data() +
                           "' not found");
}

void ProgramManager::unregisterProgram(std::string_view name) {
  if (auto iter = std::find_if(
          programs.begin(), programs.end(),
          [name](const auto &record) { return record.first == name; });
      iter != programs.end()) {
    programs.erase(iter);
  }
}
void ProgramManager::switchProgram(std::string_view name) {
  if (auto iter = std::find_if(
          programs.begin(), programs.end(),
          [name](const auto &record) { return record.first == name; });
      iter != programs.end()) {
    iter->second->use();
  } else {
    throw std::runtime_error("Program with name '"s + name.data() +
                             "' not found");
  }
}
