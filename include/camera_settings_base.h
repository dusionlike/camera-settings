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
  /** rangeFlags & 1 Auto; rangeFlags & 2 Auto Manual */
  long rangeFlags;
  /** 1 Auto 2 Manual */
  long flags;
  long type;
};

struct CameraSettingSetter
{
  long prop;
  long val;
  /** 1 Auto 2 Manual */
  long flags;
};

/**
 * Get camera settings by camera name
 * @param wszName camera name
 */
std::vector<CameraSetting> GetCameraSettings(const wchar_t *wszName);

/**
 * Get camera settings by camera index
 * @param cameraIndex camera index
 */
std::vector<CameraSetting> GetCameraSettings(int cameraIndex);

/**
 * Set camera settings by camera name
 * @param wszName camera name
 * @param settings camera settings
 */
void SetCameraSettings(const wchar_t *wszName, const std::vector<CameraSettingSetter> &settings);

/**
 * Set camera settings by camera index
 * @param cameraIndex camera index
*/
void SetCameraSettings(int cameraIndex, const std::vector<CameraSettingSetter> &settings);

#endif