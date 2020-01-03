//
// Created by petr on 1/3/20.
//

#ifndef TERRAINGENERATION_CONFIGDATA_H
#define TERRAINGENERATION_CONFIGDATA_H

struct ConfigData {
  struct {
    bool depthTest;
    bool msaa;
    bool backfaceCulling;
  } gl;

  struct {
    std::string shaderLocation;
    std::string assetsLocation;
  } paths;

  struct {
    unsigned int viewDistance;
    unsigned int levelOfDetail;
    bool viewFrustumCulling;
    bool shadows;
  } render;

  struct {
    float chunkSize;
    unsigned int surroundingSize;
    unsigned int chunkPoolSize;
    bool aggressiveChunkUnloading;
    unsigned int computeBatchSize;
  } marchingCubes;
};

#endif // TERRAINGENERATION_CONFIGDATA_H
