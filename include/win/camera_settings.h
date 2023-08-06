#pragma once
#ifndef CAMERA_SETTINGS_H
#define CAMERA_SETTINGS_H

#include <dshow.h>
#include <iostream>
#include "camera_settings_base.h"

/**
 * Query IAMCameraControl by camera name
 */
HRESULT QueryAllInterface(const wchar_t *wszName, IAMVideoProcAmp **ppProcAmp, IAMCameraControl **ppCameraControl);

#endif