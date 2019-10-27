//
// Created by petr on 10/27/19.
//

#include "FPSCounter.h"
float FPSCounter::average() {
  return static_cast<float>(totalFrameCount) / totalTimeSum.count() * std::chrono::duration_cast<std::chrono::nanoseconds>(1s).count();
}

float FPSCounter::current() {
  return static_cast<float>(1) / lastFrameDuration.count() * std::chrono::duration_cast<std::chrono::nanoseconds>(1s).count();
}


void FPSCounter::step() {
  ++totalFrameCount;
  auto timeStamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
  lastFrameDuration = timeStamp - lastTimeStamp;
  totalTimeSum += lastFrameDuration;
  lastTimeStamp = timeStamp;
}
void FPSCounter::restart() {
  totalFrameCount = 0;
  totalTimeSum = 0ns;
  lastFrameDuration = 0ns;
  lastTimeStamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
}

std::ostream &operator<<(std::ostream &stream, FPSCounter &fpsCounter) {
  stream << "[FPS]: average: " << fpsCounter.average() << " immediate: " << fpsCounter.current();
  return stream;
}