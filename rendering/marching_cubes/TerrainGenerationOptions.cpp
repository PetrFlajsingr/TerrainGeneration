//
// Created by petr on 1/2/20.
//

#include <random>

#include <geGL/StaticCalls.h>
#include "TerrainGenerationOptions.h"

TerrainGenerationOptions::TerrainGenerationOptions(unsigned int octaves, float gain, float lacunarity, float sharpness,
                                                   float valleyScale, float heightScale)
    : octaves(octaves), gain(gain), lacunarity(lacunarity), sharpness(sharpness), valleyScale(valleyScale), heightScale(heightScale){}

unsigned int TerrainGenerationOptions::getOctaves() const { return octaves; }

TerrainGenerationOptions &TerrainGenerationOptions::setOctaves(unsigned int octaves) {
  TerrainGenerationOptions::octaves = octaves;
  return *this;
}

float TerrainGenerationOptions::getGain() const { return gain; }

TerrainGenerationOptions &TerrainGenerationOptions::setGain(float gain) {
  TerrainGenerationOptions::gain = gain;
  return *this;
}

float TerrainGenerationOptions::getLacunarity() const { return lacunarity; }

TerrainGenerationOptions &TerrainGenerationOptions::setLacunarity(float lacunarity) {
  TerrainGenerationOptions::lacunarity = lacunarity;
  return *this;
}

float TerrainGenerationOptions::getSharpness() const { return sharpness; }

TerrainGenerationOptions &TerrainGenerationOptions::setSharpness(float sharpness) {
  TerrainGenerationOptions::sharpness = sharpness;
  return *this;
}

float TerrainGenerationOptions::getValleyScale() const { return valleyScale; }

TerrainGenerationOptions &TerrainGenerationOptions::setValleyScale(float valleyScale) {
  TerrainGenerationOptions::valleyScale = valleyScale;
  return *this;
}

float TerrainGenerationOptions::getHeightScale() const { return heightScale; }

TerrainGenerationOptions &TerrainGenerationOptions::setHeightScale(float heightScale) {
  TerrainGenerationOptions::heightScale = heightScale;
  return *this;
}

void TerrainGenerationOptions::randomize() {
  std::random_device dev;
  std::mt19937 rng(dev());
  {
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 8);
    octaves = dist(rng);
  }
  {
    std::uniform_real_distribution<> dist(0.0001, 100);
    gain = dist(rng);
    lacunarity = dist(rng);
    valleyScale = dist(rng);
    heightScale = dist(rng);
  }
  {
    std::uniform_real_distribution<> dist(-1, 1);
    sharpness = dist(rng);
  }
}

void TerrainGenerationOptions::setUniforms(GLuint program) {
  ge::gl::glUniform1ui(ge::gl::glGetUniformLocation(program, "octaves"), octaves);
  ge::gl::glUniform1f(ge::gl::glGetUniformLocation(program, "gain"), gain);
  ge::gl::glUniform1f(ge::gl::glGetUniformLocation(program, "lacunarity"), lacunarity);
  ge::gl::glUniform1f(ge::gl::glGetUniformLocation(program, "sharpness"), sharpness);
  ge::gl::glUniform1f(ge::gl::glGetUniformLocation(program, "valleyScale"), valleyScale);
  ge::gl::glUniform1f(ge::gl::glGetUniformLocation(program, "heightScale"), heightScale);
}
