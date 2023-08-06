#include <napi.h>
#include "camera_settings_base.h"

Napi::Object GetSetting(Napi::Env env, CameraSetting setting)
{
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "prop"), Napi::Number::New(env, setting.prop));
  obj.Set(Napi::String::New(env, "min"), Napi::Number::New(env, setting.min));
  obj.Set(Napi::String::New(env, "max"), Napi::Number::New(env, setting.max));
  obj.Set(Napi::String::New(env, "val"), Napi::Number::New(env, setting.val));
  obj.Set(Napi::String::New(env, "step"), Napi::Number::New(env, setting.step));
  obj.Set(Napi::String::New(env, "def"), Napi::Number::New(env, setting.def));
  obj.Set(Napi::String::New(env, "flags"), Napi::Number::New(env, setting.flags));
  return obj;
}

/**
 * Get camera settings by camera name
 */
Napi::Value N_GetCameraSettings(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  try
  {
    std::string cameraName = info[0].As<Napi::String>().Utf8Value();
    std::wstring wCameraName = std::wstring(cameraName.begin(), cameraName.end());
    std::vector<CameraSetting> settings = GetCameraSettings(wCameraName.c_str());
    Napi::Array arr = Napi::Array::New(env, settings.size());
    for (int i = 0; static_cast<std::vector<CameraSetting>::size_type>(i) < settings.size(); i++)
    {
      arr.Set(i, GetSetting(env, settings[i]));
    }
    return arr;
  }
  catch (const std::exception &e)
  {
    Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
    return env.Null();
  }
}

Napi::Value N_SetCameraSettings(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  try
  {
    std::string cameraName = info[0].As<Napi::String>().Utf8Value();
    std::wstring wCameraName = std::wstring(cameraName.begin(), cameraName.end());
    Napi::Array n_settings = info[1].As<Napi::Array>();
    std::vector<CameraSettingSetter> settings;
    for (int i = 0; static_cast<uint32_t>(i) < n_settings.Length(); i++)
    {
      Napi::Object obj = n_settings.Get(i).As<Napi::Object>();
      CameraSettingSetter setting;
      setting.prop = obj.Get("prop").As<Napi::Number>().Int32Value();
      setting.val = obj.Get("val").As<Napi::Number>().Int32Value();
      setting.flags = obj.Get("flags").As<Napi::Number>().Int32Value();
      settings.push_back(setting);
    }
    SetCameraSettings(wCameraName.c_str(), settings);
  }
  catch (const std::exception &e)
  {
    Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
  }
  return env.Null();
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set(Napi::String::New(env, "N_GetCameraSettings"), Napi::Function::New(env, N_GetCameraSettings));
  exports.Set(Napi::String::New(env, "N_SetCameraSettings"), Napi::Function::New(env, N_SetCameraSettings));
  return exports;
}

NODE_API_MODULE(camera_settings, Init)