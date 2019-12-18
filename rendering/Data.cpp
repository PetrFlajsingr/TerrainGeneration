#include "Data.h"
//
// Created by petr on 11/18/19.
//

void to_json(json &j, const GLData &data) {
  j = json{{"depthTest", data.depthTest}, {"msaa", data.msaa}, {"backfaceCulling", data.backfaceCulling}};
}
void from_json(const json &j, GLData &data) {
  j.at("depthTest").get_to(data.depthTest);
  j.at("msaa").get_to(data.msaa);
  j.at("backfaceCulling").get_to(data.backfaceCulling);
}
void to_json(json &j, const DeviceData &data) {
  j = json{{"screen", {"width", data.screen.width}, {"height", data.screen.height}}};
}
void from_json(const json &j, DeviceData &data) {
  j.at("screen").at("width").get_to(data.screen.width);
  j.at("screen").at("height").get_to(data.screen.height);
}
void to_json(json &j, const RenderData &data) {
  j = json{{"viewDistance", data.viewDistance},
           {"levelOfDetail", data.levelOfDetail},
           {"viewFrustumCulling", data.viewFrustumCulling}};
}
void from_json(const json &j, RenderData &data) {
  j.at("viewDistance").get_to(data.viewDistance);
  j.at("levelOfDetail").get_to(data.levelOfDetail);
  j.at("viewFrustumCulling").get_to(data.viewFrustumCulling);
}
