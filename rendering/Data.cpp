#include "Data.h"
//
// Created by petr on 11/18/19.
//

void to_json(json &j, const GLData &data) {
  j = json{{"depthTest", data.depthTest}, {"msaa", data.msaa}, {"backfaceCulling", data.backfaceCulling}};
}
void from_json(const json &j, GLData &data) {
  data.depthTest = j.at("depthTest");
  data.msaa = j.at("msaa");
  data.backfaceCulling = j.at("backfaceCulling");
}
void to_json(json &j, const DeviceData &data) {
  j = json{{"screen", {"width", data.screen.width}, {"height", data.screen.height}}};
}
void from_json(const json &j, DeviceData &data) {
  data.screen.width = j.at("screen").at("width");
  data.screen.height = j.at("screen").at("height");
}
void to_json(json &j, const RenderData &data) {
  j = json{{"viewDistance", data.viewDistance},
           {"levelOfDetail", data.levelOfDetail},
           {"viewFrustumCulling", data.viewFrustumCulling}};
}
void from_json(const json &j, RenderData &data) {
  data.viewDistance = j.at("viewDistance");
  data.levelOfDetail = j.at("levelOfDetail");
  data.viewFrustumCulling = j.at("viewFrustumCulling");
}
