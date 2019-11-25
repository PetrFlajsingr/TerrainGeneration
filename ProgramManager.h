//
// Created by petr on 10/10/19.
//

#ifndef TERRAINGENERATION_PROGRAMMANAGER_H
#define TERRAINGENERATION_PROGRAMMANAGER_H

#include <algorithm>
#include <geGL/Program.h>
#include <unordered_map>

class ManagedProgram;

class ProgramManager {
public:
  using ProgramPtr = std::shared_ptr<ge::gl::Program>;

  ProgramManager(const ProgramManager &) = delete;
  ProgramManager &operator=(const ProgramManager &) = delete;

  void registerProgram(std::string_view name, const ProgramPtr &program);

  void unregisterProgram(std::string_view name);

  [[nodiscard]] ProgramPtr getProgram(std::string_view name);

  void switchProgram(std::string_view name);

  static ProgramManager &GetInstance();

private:
  ProgramManager() = default;

  std::unordered_map<std::string, ProgramPtr> programs;
};

class ManagedProgram : public ge::gl::Program {
public:
  template <typename... Args,
            typename = std::enable_if_t<
                std::is_constructible_v<ManagedProgram, Args...>>>
  static std::shared_ptr<ManagedProgram> CreateShared(Args... args);

  template <typename... ARGS>
  explicit ManagedProgram(std::string name, ARGS... shaders);

  [[nodiscard]] std::string_view getName();

private:
  std::string name;

  ManagedProgram(std::string name, const ShaderPointers &shaders);
  ManagedProgram(std::string name, const ge::gl::FunctionTablePointer &table,
                 const ShaderPointers &shaders);

  template <typename... ARGS>
  ManagedProgram(std::string name, const ge::gl::FunctionTablePointer &table,
                 ARGS... shaders);
};

template <typename... ARGS>
ManagedProgram::ManagedProgram(std::string name, ARGS... shaders)
    : Program(shaders...), name(std::move(name)) {}

template <typename... ARGS>
ManagedProgram::ManagedProgram(std::string name,
                               const ge::gl::FunctionTablePointer &table,
                               ARGS... shaders)
    : Program(table, shaders...), name(std::move(name)) {}

template <typename... Args, typename>
std::shared_ptr<ManagedProgram> ManagedProgram::CreateShared(Args... args) {
  auto result = std::make_shared<ManagedProgram>(std::forward<Args>(args)...);
  ProgramManager::GetInstance().registerProgram(result->name, result);
  return result;
}

#endif // TERRAINGENERATION_PROGRAMMANAGER_H
