#pragma once
#ifndef CAMERA_SETTINGS_BASE_H
#define CAMERA_SETTINGS_BASE_H

#include <iostream>
#include <vector>

struct CameraSetting
{
  long prop;
  long min;
  long max;
  long val;
  long step;
  long def;
  long range_flags;
  long flags;
  long type;
};

struct CameraSettingSetter
{
  long prop;
  long val;
  long flags;
};

/**
 * Get camera settings by camera name
 * @param wszName camera name
 */
std::vector<CameraSetting> GetCameraSettings(const wchar_t *wszName);

/**
 * Set camera settings by camera name
 * @param wszName camera name
 * @param settings camera settings
 */
void SetCameraSettings(const wchar_t *wszName, const std::vector<CameraSettingSetter> &settings);

#endif