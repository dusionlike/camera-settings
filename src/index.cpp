﻿#include <napi.h>
#include "camera_settings_base.h"
#include "get_settings.hpp"
#include "set_settings.hpp"
#include "get_resolution.hpp"
#include "open_camera.hpp"
#include "close_camera.hpp"

void QueryJsParams(const Napi::CallbackInfo &info, std::wstring &wCameraName, int &camIndex)
{
  Napi::Env env = info.Env();
  if (info[0].IsNumber())
  {
    camIndex = info[0].As<Napi::Number>().Int32Value();
  }
  else if (info[0].IsString())
  {
    std::string cameraName = info[0].As<Napi::String>().Utf8Value();
    wCameraName = std::wstring(cameraName.begin(), cameraName.end());
  }
  else
  {
    Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
  }
}

Napi::Value N_GetCameraSettings(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  std::wstring wCameraName = L"";
  int camIndex = -1;
  QueryJsParams(info, wCameraName, camIndex);

  GetSettingsWorker *worker = new GetSettingsWorker(env, wCameraName, camIndex);
  worker->Queue();
  return worker->Promise();
}

Napi::Value N_SetCameraSettings(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  std::wstring wCameraName = L"";
  int camIndex = -1;
  QueryJsParams(info, wCameraName, camIndex);

  Napi::Array n_settings = info[1].As<Napi::Array>();

  std::vector<CameraSettingSetter> settings;
  for (size_t i = 0; i < n_settings.Length(); i++)
  {
    Napi::Object obj = n_settings.Get(i).As<Napi::Object>();
    CameraSettingSetter setting;
    setting.prop = obj.Get("prop").As<Napi::String>().Utf8Value();
    setting.val = obj.Get("val").As<Napi::Number>().Int32Value();
    setting.isAuto = obj.Get("isAuto").As<Napi::Boolean>().Value();
    settings.push_back(setting);
  }

  SetSettingsWorker *worker = new SetSettingsWorker(env, wCameraName, camIndex, settings);
  worker->Queue();
  return worker->Promise();
}

Napi::Value N_GetCameraResolutions(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  std::wstring wCameraName = L"";
  int camIndex = -1;
  QueryJsParams(info, wCameraName, camIndex);
  GetResolutionsWorker *worker = new GetResolutionsWorker(env, wCameraName.c_str(), camIndex);
  worker->Queue();
  return worker->Promise();
}

Napi::Value N_OpenCameraSettings(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  std::wstring wCameraName = L"";
  int camIndex = -1;
  QueryJsParams(info, wCameraName, camIndex);
  OpenSettingsWorker *worker = new OpenSettingsWorker(env, wCameraName.c_str(), camIndex);
  worker->Queue();
  return worker->Promise();
}

Napi::Value N_CloseCameraSettings(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  std::wstring wCameraName = L"";
  int camIndex = -1;
  QueryJsParams(info, wCameraName, camIndex);
  CloseSettingsWorker *worker = new CloseSettingsWorker(env, wCameraName.c_str(), camIndex);
  worker->Queue();
  return worker->Promise();
}

Napi::Value N_CloseCameraSettingsSync(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  std::wstring wCameraName = L"";
  int camIndex = -1;
  QueryJsParams(info, wCameraName, camIndex);
  CloseCameraSettings(wCameraName.c_str(), camIndex);
  return env.Undefined();
}

Napi::Value N_GetCacheCount(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  int res = GetCacheCount();
  return Napi::Number::New(env, res);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports["openCameraSettings"] = Napi::Function::New(env, N_OpenCameraSettings);
  exports["closeCameraSettings"] = Napi::Function::New(env, N_CloseCameraSettings);
  exports["closeCameraSettingsSync"] = Napi::Function::New(env, N_CloseCameraSettingsSync);
  exports["getCameraSettings"] = Napi::Function::New(env, N_GetCameraSettings);
  exports["setCameraSettings"] = Napi::Function::New(env, N_SetCameraSettings);
  exports["getCameraResolutions"] = Napi::Function::New(env, N_GetCameraResolutions);
  exports["_getCacheCount"] = Napi::Function::New(env, N_GetCacheCount);
  return exports;
}

NODE_API_MODULE(camera_settings, Init)