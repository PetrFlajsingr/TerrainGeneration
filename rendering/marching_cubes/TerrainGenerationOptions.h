//
// Created by petr on 1/2/20.
//

#ifndef TERRAINGENERATION_TERRAINGENERATIONOPTIONS_H
#define TERRAINGENERATION_TERRAINGENERATIONOPTIONS_H

class TerrainGenerationOptions {
public:
  TerrainGenerationOptions() = default;
  TerrainGenerationOptions(unsigned int octaves, float gain, float lacunarity, float sharpness, float valleyScale, float heightScale);

  [[nodiscard]] unsigned int getOctaves() const;
  TerrainGenerationOptions &setOctaves(unsigned int octaves);
  [[nodiscard]] float getGain() const;
  TerrainGenerationOptions & setGain(float gain);
  [[nodiscard]] float getLacunarity() const;
  TerrainGenerationOptions & setLacunarity(float lacunarity);
  [[nodiscard]] float getSharpness() const;
  TerrainGenerationOptions & setSharpness(float sharpness);
  [[nodiscard]] float getValleyScale() const;
  TerrainGenerationOptions & setValleyScale(float valleyScale);
  [[nodiscard]] float getHeightScale() const;
  TerrainGenerationOptions & setHeightScale(float heightScale);

  void setUniforms(GLuint program);

  void randomize();

private:
  unsigned int octaves = 4;
  float gain = 15;
  float lacunarity = 6;
  float sharpness = 0.000001;
  float valleyScale = 0.1;
  float heightScale = 5;
};

#endif // TERRAINGENERATION_TERRAINGENERATIONOPTIONS_H
