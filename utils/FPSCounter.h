//
// Created by petr on 10/27/19.
//

#ifndef TERRAINGENERATION_FPSCOUNTER_H
#define TERRAINGENERATION_FPSCOUNTER_H

#include <chrono>
#include <ostream>

using namespace std::chrono_literals;
class FPSCounter {
public:
  float average() const;

  float current() const;

  void step();

  void restart();

  friend std::ostream&operator<<(std::ostream& stream, const FPSCounter &fpsCounter);

private:
  uint64_t totalFrameCount = 0;
  std::chrono::nanoseconds totalTimeSum = 0ns;
  std::chrono::nanoseconds lastTimeStamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
  std::chrono::nanoseconds lastFrameDuration = 0ns;

};

#endif // TERRAINGENERATION_FPSCOUNTER_H
