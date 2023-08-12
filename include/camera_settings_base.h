#pragma once
#ifndef CAMERA_SETTINGS_BASE_H
#define CAMERA_SETTINGS_BASE_H

#include <iostream>
#include <map>
#include <vector>
#include <string>

struct CameraSetting
{
  std::string prop;
  long min;
  long max;
  long val;
  long step;
  long def;
  /** rangeFlags & 1 Auto; rangeFlags & 2 Auto Manual */
  long rangeFlags;
  bool isAuto;
  std::string ctrlType;
};

struct CameraSettingSetter
{
  std::string prop;
  long val;
  bool isAuto;
};

struct Resolution
{
  int width;
  int height;
  std::string type;
};

/**
 * Get camera settings by camera name
 * @param wszName camera name
 */
std::vector<CameraSetting> GetCameraSettings(const wchar_t *wszName, int index);

/**
 * Set camera settings by camera name
 * @param wszName camera name
 * @param settings camera settings
 */
void SetCameraSettings(const wchar_t *wszName, int index, const std::vector<CameraSettingSetter> &settings);

/**
 * Set camera settings by camera index
 * @param cameraIndex camera index
 */
void SetCameraSettings(int cameraIndex, const std::vector<CameraSettingSetter> &settings);

std::vector<Resolution> GetCameraResolutions(const wchar_t *wszName, int index);

#endif