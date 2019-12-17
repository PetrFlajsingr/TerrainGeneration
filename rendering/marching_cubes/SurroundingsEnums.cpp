//
// Created by petr on 12/17/19.
//

#include "SurroundingsEnums.h"
#include <error_handling/exceptions.h>


// Please excuse this absolute insanity...

glm::vec3 SurrDirToVec(SurrMoveDir direction) {
  switch (direction) {
  case SurrMoveDir::Front:
    return glm::vec3{0, 0, 1};
  case SurrMoveDir::Back:
    return glm::vec3{0, 0, -1};
  case SurrMoveDir::Left:
    return glm::vec3{-1, 0, 0};
  case SurrMoveDir::Right:
    return glm::vec3{1, 0, 0};
  case SurrMoveDir::Up:
    return glm::vec3{0, 1, 0};
  case SurrMoveDir::Down:
    return glm::vec3{0, -1, 0};
  }
  throw exc::InternalError("Invalid enum value");
}

SurrPos PosForDir(SurrMoveDir direction, SurrPos pos) {
  switch (direction) {
  case SurrMoveDir::Front:
    switch (pos) {
    case SurrPos::BackLowLeft:
      return SurrPos::MidLowLeft;
    case SurrPos::BackLowMid:
      return SurrPos::MidLowMid;
    case SurrPos::BackLowRight:
      return SurrPos::MidLowRight;
    case SurrPos::BackMidLeft:
      return SurrPos::MidMidLeft;
    case SurrPos::BackMidMid:
      return SurrPos::MidMidMid;
    case SurrPos::BackMidRight:
      return SurrPos::MidMidRight;
    case SurrPos::BackHighLeft:
      return SurrPos::MidHighLeft;
    case SurrPos::BackHighMid:
      return SurrPos::MidHighMid;
    case SurrPos::BackHighRight:
      return SurrPos::MidHighRight;
    case SurrPos::MidLowLeft:
      return SurrPos::FrontLowLeft;
    case SurrPos::MidLowMid:
      return SurrPos::FrontLowMid;
    case SurrPos::MidLowRight:
      return SurrPos::FrontLowRight;
    case SurrPos::MidMidLeft:
      return SurrPos::FrontMidLeft;
    case SurrPos::MidMidMid:
      return SurrPos::FrontMidMid;
    case SurrPos::MidMidRight:
      return SurrPos::FrontMidRight;
    case SurrPos::MidHighLeft:
      return SurrPos::FrontHighLeft;
    case SurrPos::MidHighMid:
      return SurrPos::FrontHighMid;
    case SurrPos::MidHighRight:
      return SurrPos::FrontHighRight;
    case SurrPos::FrontLowLeft:
      return SurrPos::BackLowLeft;
    case SurrPos::FrontLowMid:
      return SurrPos::BackLowMid;
    case SurrPos::FrontLowRight:
      return SurrPos::BackLowRight;
    case SurrPos::FrontMidLeft:
      return SurrPos::BackMidLeft;
    case SurrPos::FrontMidMid:
      return SurrPos::BackMidMid;
    case SurrPos::FrontMidRight:
      return SurrPos::BackMidRight;
    case SurrPos::FrontHighLeft:
      return SurrPos::BackHighLeft;
    case SurrPos::FrontHighMid:
      return SurrPos::BackHighMid;
    case SurrPos::FrontHighRight:
      return SurrPos::BackHighRight;
    }
    break;
  case SurrMoveDir::Back:
    switch (pos) {
    case SurrPos::BackLowLeft:
      return SurrPos::FrontLowLeft;
    case SurrPos::BackLowMid:
      return SurrPos::FrontLowMid;
    case SurrPos::BackLowRight:
      return SurrPos::FrontLowRight;
    case SurrPos::BackMidLeft:
      return SurrPos::FrontMidLeft;
    case SurrPos::BackMidMid:
      return SurrPos::FrontMidMid;
    case SurrPos::BackMidRight:
      return SurrPos::FrontMidRight;
    case SurrPos::BackHighLeft:
      return SurrPos::FrontHighLeft;
    case SurrPos::BackHighMid:
      return SurrPos::FrontHighMid;
    case SurrPos::BackHighRight:
      return SurrPos::FrontHighRight;
    case SurrPos::MidLowLeft:
      return SurrPos::BackLowLeft;
    case SurrPos::MidLowMid:
      return SurrPos::BackLowMid;
    case SurrPos::MidLowRight:
      return SurrPos::BackLowRight;
    case SurrPos::MidMidLeft:
      return SurrPos::BackMidLeft;
    case SurrPos::MidMidMid:
      return SurrPos::BackMidMid;
    case SurrPos::MidMidRight:
      return SurrPos::BackMidRight;
    case SurrPos::MidHighLeft:
      return SurrPos::BackHighLeft;
    case SurrPos::MidHighMid:
      return SurrPos::BackHighMid;
    case SurrPos::MidHighRight:
      return SurrPos::BackHighRight;
    case SurrPos::FrontLowLeft:
      return SurrPos::MidLowLeft;
    case SurrPos::FrontLowMid:
      return SurrPos::MidLowMid;
    case SurrPos::FrontLowRight:
      return SurrPos::MidLowRight;
    case SurrPos::FrontMidLeft:
      return SurrPos::MidMidLeft;
    case SurrPos::FrontMidMid:
      return SurrPos::MidMidMid;
    case SurrPos::FrontMidRight:
      return SurrPos::MidMidRight;
    case SurrPos::FrontHighLeft:
      return SurrPos::MidHighLeft;
    case SurrPos::FrontHighMid:
      return SurrPos::MidHighMid;
    case SurrPos::FrontHighRight:
      return SurrPos::MidHighRight;
    }
    break;
  case SurrMoveDir::Right:
    switch (pos) {
    case SurrPos::BackHighLeft:
      return SurrPos::BackHighMid;
    case SurrPos::BackLowLeft:
      return SurrPos::BackLowMid;
    case SurrPos::BackMidLeft:
      return SurrPos::BackMidMid;
    case SurrPos::FrontHighLeft:
      return SurrPos::FrontHighMid;
    case SurrPos::FrontLowLeft:
      return SurrPos::FrontLowMid;
    case SurrPos::FrontMidLeft:
      return SurrPos::FrontMidMid;
    case SurrPos::MidHighLeft:
      return SurrPos::MidHighMid;
    case SurrPos::MidLowLeft:
      return SurrPos::MidLowMid;
    case SurrPos::MidMidLeft:
      return SurrPos::MidMidMid;
    case SurrPos::BackHighMid:
      return SurrPos::BackHighRight;
    case SurrPos::BackLowMid:
      return SurrPos::BackLowRight;
    case SurrPos::BackMidMid:
      return SurrPos::BackMidRight;
    case SurrPos::FrontHighMid:
      return SurrPos::FrontHighRight;
    case SurrPos::FrontLowMid:
      return SurrPos::FrontLowRight;
    case SurrPos::FrontMidMid:
      return SurrPos::FrontMidRight;
    case SurrPos::MidHighMid:
      return SurrPos::MidHighRight;
    case SurrPos::MidLowMid:
      return SurrPos::MidLowRight;
    case SurrPos::MidMidMid:
      return SurrPos::MidMidRight;
    case SurrPos::BackHighRight:
      return SurrPos::BackHighLeft;
    case SurrPos::BackLowRight:
      return SurrPos::BackLowLeft;
    case SurrPos::BackMidRight:
      return SurrPos::BackMidLeft;
    case SurrPos::FrontHighRight:
      return SurrPos::FrontHighLeft;
    case SurrPos::FrontLowRight:
      return SurrPos::FrontLowLeft;
    case SurrPos::FrontMidRight:
      return SurrPos::FrontMidLeft;
    case SurrPos::MidHighRight:
      return SurrPos::MidHighLeft;
    case SurrPos::MidLowRight:
      return SurrPos::MidLowLeft;
    case SurrPos::MidMidRight:
      return SurrPos::MidMidLeft;
    }
    break;
  case SurrMoveDir::Left:
    switch (pos) {
    case SurrPos::BackHighRight:
      return SurrPos::BackHighMid;
    case SurrPos::BackLowRight:
      return SurrPos::BackLowMid;
    case SurrPos::BackMidRight:
      return SurrPos::BackMidMid;
    case SurrPos::FrontHighRight:
      return SurrPos::FrontHighMid;
    case SurrPos::FrontLowRight:
      return SurrPos::FrontLowMid;
    case SurrPos::FrontMidRight:
      return SurrPos::FrontMidMid;
    case SurrPos::MidHighRight:
      return SurrPos::MidHighMid;
    case SurrPos::MidLowRight:
      return SurrPos::MidLowMid;
    case SurrPos::MidMidRight:
      return SurrPos::MidMidMid;
    case SurrPos::BackHighMid:
      return SurrPos::BackHighLeft;
    case SurrPos::BackLowMid:
      return SurrPos::BackLowLeft;
    case SurrPos::BackMidMid:
      return SurrPos::BackMidLeft;
    case SurrPos::FrontHighMid:
      return SurrPos::FrontHighLeft;
    case SurrPos::FrontLowMid:
      return SurrPos::FrontLowLeft;
    case SurrPos::FrontMidMid:
      return SurrPos::FrontMidLeft;
    case SurrPos::MidHighMid:
      return SurrPos::MidHighLeft;
    case SurrPos::MidLowMid:
      return SurrPos::MidLowLeft;
    case SurrPos::MidMidMid:
      return SurrPos::MidMidLeft;
    case SurrPos::BackHighLeft:
      return SurrPos::BackHighRight;
    case SurrPos::BackLowLeft:
      return SurrPos::BackLowRight;
    case SurrPos::BackMidLeft:
      return SurrPos::BackMidRight;
    case SurrPos::FrontHighLeft:
      return SurrPos::FrontHighRight;
    case SurrPos::FrontLowLeft:
      return SurrPos::FrontLowRight;
    case SurrPos::FrontMidLeft:
      return SurrPos::FrontMidRight;
    case SurrPos::MidHighLeft:
      return SurrPos::MidHighRight;
    case SurrPos::MidLowLeft:
      return SurrPos::MidLowRight;
    case SurrPos::MidMidLeft:
      return SurrPos::MidMidRight;
    }
    break;
  case SurrMoveDir::Down:
    switch (pos) {
    case SurrPos::MidHighLeft:
      return SurrPos::MidMidLeft;
    case SurrPos::FrontHighLeft:
      return SurrPos::FrontMidLeft;
    case SurrPos::BackHighLeft:
      return SurrPos::BackMidLeft;
    case SurrPos::BackHighMid:
      return SurrPos::BackMidMid;
    case SurrPos::BackHighRight:
      return SurrPos::BackMidRight;
    case SurrPos::FrontHighMid:
      return SurrPos::FrontMidMid;
    case SurrPos::FrontHighRight:
      return SurrPos::FrontMidRight;
    case SurrPos::MidHighMid:
      return SurrPos::MidMidMid;
    case SurrPos::MidHighRight:
      return SurrPos::MidMidRight;
    case SurrPos::MidMidLeft:
      return SurrPos::MidLowLeft;
    case SurrPos::FrontMidLeft:
      return SurrPos::FrontLowLeft;
    case SurrPos::BackMidLeft:
      return SurrPos::BackLowLeft;
    case SurrPos::BackMidMid:
      return SurrPos::BackLowMid;
    case SurrPos::BackMidRight:
      return SurrPos::BackLowRight;
    case SurrPos::FrontMidMid:
      return SurrPos::FrontLowMid;
    case SurrPos::FrontMidRight:
      return SurrPos::FrontLowRight;
    case SurrPos::MidMidMid:
      return SurrPos::MidLowMid;
    case SurrPos::MidMidRight:
      return SurrPos::MidLowRight;
    case SurrPos::MidLowLeft:
      return SurrPos::MidHighLeft;
    case SurrPos::FrontLowLeft:
      return SurrPos::FrontHighLeft;
    case SurrPos::BackLowLeft:
      return SurrPos::BackHighLeft;
    case SurrPos::BackLowMid:
      return SurrPos::BackHighMid;
    case SurrPos::BackLowRight:
      return SurrPos::BackHighRight;
    case SurrPos::FrontLowMid:
      return SurrPos::FrontHighMid;
    case SurrPos::FrontLowRight:
      return SurrPos::FrontHighRight;
    case SurrPos::MidLowMid:
      return SurrPos::MidHighMid;
    case SurrPos::MidLowRight:
      return SurrPos::MidHighRight;
    }
    break;
  case SurrMoveDir::Up:
    switch (pos) {
    case SurrPos::MidHighLeft:
      return SurrPos::MidLowLeft;
    case SurrPos::FrontHighLeft:
      return SurrPos::FrontLowLeft;
    case SurrPos::BackHighLeft:
      return SurrPos::BackLowLeft;
    case SurrPos::BackHighMid:
      return SurrPos::BackLowMid;
    case SurrPos::BackHighRight:
      return SurrPos::BackLowRight;
    case SurrPos::FrontHighMid:
      return SurrPos::FrontLowMid;
    case SurrPos::FrontHighRight:
      return SurrPos::FrontLowRight;
    case SurrPos::MidHighMid:
      return SurrPos::MidLowMid;
    case SurrPos::MidHighRight:
      return SurrPos::MidLowRight;
    case SurrPos::MidMidLeft:
      return SurrPos::MidHighLeft;
    case SurrPos::FrontMidLeft:
      return SurrPos::FrontHighLeft;
    case SurrPos::BackMidLeft:
      return SurrPos::BackHighLeft;
    case SurrPos::BackMidMid:
      return SurrPos::BackHighMid;
    case SurrPos::BackMidRight:
      return SurrPos::BackHighRight;
    case SurrPos::FrontMidMid:
      return SurrPos::FrontHighMid;
    case SurrPos::FrontMidRight:
      return SurrPos::FrontHighRight;
    case SurrPos::MidMidMid:
      return SurrPos::MidHighMid;
    case SurrPos::MidMidRight:
      return SurrPos::MidHighRight;
    case SurrPos::MidLowLeft:
      return SurrPos::MidMidLeft;
    case SurrPos::FrontLowLeft:
      return SurrPos::FrontMidLeft;
    case SurrPos::BackLowLeft:
      return SurrPos::BackMidLeft;
    case SurrPos::BackLowMid:
      return SurrPos::BackMidMid;
    case SurrPos::BackLowRight:
      return SurrPos::BackMidRight;
    case SurrPos::FrontLowMid:
      return SurrPos::FrontMidMid;
    case SurrPos::FrontLowRight:
      return SurrPos::FrontMidRight;
    case SurrPos::MidLowMid:
      return SurrPos::MidMidMid;
    case SurrPos::MidLowRight:
      return SurrPos::MidMidRight;
    }
    break;
  }
  throw exc::InternalError("Invalid input.");
}
SurrPos CoordSourceForDir(SurrMoveDir direction, SurrPos pos) {
  switch (direction) {
  case SurrMoveDir::Front:
    switch (pos) {
    case SurrPos::FrontLowLeft:
      return SurrPos::MidLowLeft;
    case SurrPos::FrontLowMid:
      return SurrPos::MidLowMid;
    case SurrPos::FrontLowRight:
      return SurrPos::MidLowRight;
    case SurrPos::FrontMidLeft:
      return SurrPos::MidMidLeft;
    case SurrPos::FrontMidMid:
      return SurrPos::MidMidMid;
    case SurrPos::FrontMidRight:
      return SurrPos::MidMidRight;
    case SurrPos::FrontHighLeft:
      return SurrPos::MidHighLeft;
    case SurrPos::FrontHighMid:
      return SurrPos::MidHighMid;
    case SurrPos::FrontHighRight:
      return SurrPos::MidHighRight;
    default: throw exc::InternalError("Invalid enum value Front");
    }
  case SurrMoveDir::Back:
    switch (pos) {
    case SurrPos::BackLowLeft:
      return SurrPos::MidLowLeft;
    case SurrPos::BackLowMid:
      return SurrPos::MidLowMid;
    case SurrPos::BackLowRight:
      return SurrPos::MidLowRight;
    case SurrPos::BackMidLeft:
      return SurrPos::MidMidLeft;
    case SurrPos::BackMidMid:
      return SurrPos::MidMidMid;
    case SurrPos::BackMidRight:
      return SurrPos::MidMidRight;
    case SurrPos::BackHighLeft:
      return SurrPos::MidHighLeft;
    case SurrPos::BackHighMid:
      return SurrPos::MidHighMid;
    case SurrPos::BackHighRight:
      return SurrPos::MidHighRight;
    default: throw exc::InternalError("Invalid enum value Back");
    }
  case SurrMoveDir::Right:
    switch (pos) {
    case SurrPos::BackHighRight:
      return SurrPos::BackHighMid;
    case SurrPos::BackLowRight:
      return SurrPos::BackLowMid;
    case SurrPos::BackMidRight:
      return SurrPos::BackMidMid;
    case SurrPos::FrontHighRight:
      return SurrPos::FrontHighMid;
    case SurrPos::FrontLowRight:
      return SurrPos::FrontLowMid;
    case SurrPos::FrontMidRight:
      return SurrPos::FrontMidMid;
    case SurrPos::MidHighRight:
      return SurrPos::MidHighMid;
    case SurrPos::MidLowRight:
      return SurrPos::MidLowMid;
    case SurrPos::MidMidRight:
      return SurrPos::MidMidMid;
    default: throw exc::InternalError("Invalid enum value Right");
    }
  case SurrMoveDir::Left:
    switch (pos) {
    case SurrPos::BackHighLeft:
      return SurrPos::BackHighMid;
    case SurrPos::BackLowLeft:
      return SurrPos::BackLowMid;
    case SurrPos::BackMidLeft:
      return SurrPos::BackMidMid;
    case SurrPos::FrontHighLeft:
      return SurrPos::FrontHighMid;
    case SurrPos::FrontLowLeft:
      return SurrPos::FrontLowMid;
    case SurrPos::FrontMidLeft:
      return SurrPos::FrontMidMid;
    case SurrPos::MidHighLeft:
      return SurrPos::MidHighMid;
    case SurrPos::MidLowLeft:
      return SurrPos::MidLowMid;
    case SurrPos::MidMidLeft:
      return SurrPos::MidMidMid;
    default: throw exc::InternalError("Invalid enum value Left");
    }
  case SurrMoveDir::Down:
    switch (pos) {
    case SurrPos::MidLowLeft:
      return SurrPos::MidMidLeft;
    case SurrPos::FrontLowLeft:
      return SurrPos::FrontMidLeft;
    case SurrPos::BackLowLeft:
      return SurrPos::BackMidLeft;
    case SurrPos::BackLowMid:
      return SurrPos::BackMidMid;
    case SurrPos::BackLowRight:
      return SurrPos::BackMidRight;
    case SurrPos::FrontLowMid:
      return SurrPos::FrontMidMid;
    case SurrPos::FrontLowRight:
      return SurrPos::FrontMidRight;
    case SurrPos::MidLowMid:
      return SurrPos::MidMidMid;
    case SurrPos::MidLowRight:
      return SurrPos::MidMidRight;
    default: throw exc::InternalError("Invalid enum value Down");
    }
  case SurrMoveDir::Up:
    switch (pos) {
    case SurrPos::MidHighLeft:
      return SurrPos::MidMidLeft;
    case SurrPos::FrontHighLeft:
      return SurrPos::FrontMidLeft;
    case SurrPos::BackHighLeft:
      return SurrPos::BackMidLeft;
    case SurrPos::BackHighMid:
      return SurrPos::BackMidMid;
    case SurrPos::BackHighRight:
      return SurrPos::BackMidRight;
    case SurrPos::FrontHighMid:
      return SurrPos::FrontMidMid;
    case SurrPos::FrontHighRight:
      return SurrPos::FrontMidRight;
    case SurrPos::MidHighMid:
      return SurrPos::MidMidMid;
    case SurrPos::MidHighRight:
      return SurrPos::MidMidRight;
    default: throw exc::InternalError("Invalid enum value Up");
    }
  }
  throw exc::InternalError("Invalid input.");
}