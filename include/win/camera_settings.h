#pragma once
#ifndef CAMERA_SETTINGS_H
#define CAMERA_SETTINGS_H

#include <dshow.h>
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
 * Query IAMCameraControl by camera name
 */
HRESULT QueryAllInterface(const WCHAR *wszName, IAMVideoProcAmp **ppProcAmp, IAMCameraControl **ppCameraControl);

/**
 * Get camera settings by camera name
 * @param wszName camera name
 */
std::vector<CameraSetting> GetCameraSettings(const WCHAR *wszName);

/**
 * Set camera settings by camera name
 * @param wszName camera name
 * @param settings camera settings
 */
void SetCameraSettings(const WCHAR *wszName, const std::vector<CameraSettingSetter> &settings);

#endif