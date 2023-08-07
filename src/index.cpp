#include <napi.h>
#include <thread>
#include "camera_settings_base.h"
#include <mutex>

constexpr size_t ARRAY_LENGTH = 10;

// Data structure representing our thread-safe function context.
struct TsfnContext
{
  // Native thread
  std::thread nativeThread;

  Napi::ThreadSafeFunction tsfn;
};

void FinalizerCallback(Napi::Env env, void *finalizeData, TsfnContext *context)
{
  // Join the thread
  context->nativeThread.join();
  delete context;
}

Napi::Object GetSetting(Napi::Env env, CameraSetting setting)
{
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "prop"), Napi::Number::New(env, setting.prop));
  obj.Set(Napi::String::New(env, "min"), Napi::Number::New(env, setting.min));
  obj.Set(Napi::String::New(env, "max"), Napi::Number::New(env, setting.max));
  obj.Set(Napi::String::New(env, "val"), Napi::Number::New(env, setting.val));
  obj.Set(Napi::String::New(env, "step"), Napi::Number::New(env, setting.step));
  obj.Set(Napi::String::New(env, "def"), Napi::Number::New(env, setting.def));
  obj.Set(Napi::String::New(env, "rangeFlags"), Napi::Number::New(env, setting.rangeFlags));
  obj.Set(Napi::String::New(env, "flags"), Napi::Number::New(env, setting.flags));
  obj.Set(Napi::String::New(env, "type"), Napi::Number::New(env, setting.type));
  return obj;
}

/**
 * Get camera settings by camera name
 */
Napi::Value N_GetCameraSettingsSync(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  try
  {
    std::vector<CameraSetting> settings;
    if (info[0].IsString())
    {
      std::string cameraName = info[0].As<Napi::String>().Utf8Value();
      std::wstring wCameraName = std::wstring(cameraName.begin(), cameraName.end());
      settings = GetCameraSettings(wCameraName.c_str());
    }
    else
    {
      Napi::Number index = info[0].As<Napi::Number>();
      int cameraIndex = index.Int32Value();
      settings = GetCameraSettings(cameraIndex);
    }

    Napi::Array arr = Napi::Array::New(env, settings.size());
    for (size_t i = 0; i < settings.size(); i++)
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

void Callback_GetCameraSettings(TsfnContext *context, std::wstring wCameraName, int cameraIndex)
{
  auto callback = [wCameraName, cameraIndex](Napi::Env env, Napi::Function jsCallback)
  {
    std::vector<CameraSetting> settings;

    try
    {
      if (cameraIndex != -1)
      {
        settings = GetCameraSettings(cameraIndex);
      }
      else
      {
        settings = GetCameraSettings(wCameraName.c_str());
      }
      Napi::Array arr = Napi::Array::New(env, settings.size());
      for (size_t i = 0; i < settings.size(); i++)
      {
        arr.Set(i, GetSetting(env, settings[i]));
      }
      jsCallback.Call({env.Null(), arr});
    }
    catch (const std::exception &e)
    {
      jsCallback.Call({Napi::String::New(env, e.what()), env.Null()});
    }
  };

  auto status = context->tsfn.BlockingCall(callback);
  if (status != napi_ok)
  {
    std::cerr << "Callback_GetCameraSettings failed" << std::endl;
  }
  context->tsfn.Release();
}

Napi::Value N_GetCameraSettings(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  auto testData = new TsfnContext();

  testData->tsfn = Napi::ThreadSafeFunction::New(
      env,
      info[1].As<Napi::Function>(),
      "N_GetCameraSettings",
      0,
      1,
      testData,
      FinalizerCallback,
      (void *)nullptr // Finalizer data
  );

  if (info[0].IsString())
  {
    std::string cameraName = info[0].As<Napi::String>().Utf8Value();
    std::wstring wCameraName = std::wstring(cameraName.begin(), cameraName.end());
    testData->nativeThread = std::thread(Callback_GetCameraSettings, testData, wCameraName, -1);
  }
  else
  {
    std::vector<CameraSetting> settings;
    Napi::Number index = info[0].As<Napi::Number>();
    int cameraIndex = index.Int32Value();
    testData->nativeThread = std::thread(Callback_GetCameraSettings, testData, L"", cameraIndex);
  }
  return env.Undefined();
}

void Callback_SetCameraSettings(TsfnContext *context, std::vector<CameraSettingSetter> *psettings, std::wstring wCameraName, int cameraIndex)
{
  auto callback = [wCameraName, cameraIndex, psettings](Napi::Env env, Napi::Function jsCallback)
  {
    try
    {
      if (cameraIndex != -1)
      {
        SetCameraSettings((int)cameraIndex, *psettings);
      }
      else
      {
        SetCameraSettings(wCameraName.c_str(), *psettings);
      }
      jsCallback.Call({env.Null()});
    }
    catch (const std::exception &e)
    {
      jsCallback.Call({Napi::String::New(env, e.what())});
    }
    delete psettings;
  };

  auto status = context->tsfn.BlockingCall(callback);
  if (status != napi_ok)
  {
    std::cerr << "Callback_SetCameraSettings failed" << std::endl;
  }
  context->tsfn.Release();
}

Napi::Value N_SetCameraSettings(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  auto testData = new TsfnContext();

  testData->tsfn = Napi::ThreadSafeFunction::New(
      env,
      info[2].As<Napi::Function>(),
      "N_SetCameraSettings",
      0,
      1,
      testData,
      FinalizerCallback,
      (void *)nullptr // Finalizer data
  );

  Napi::Array n_settings = info[1].As<Napi::Array>();

  auto *psettings = new std::vector<CameraSettingSetter>();
  for (size_t i = 0; i < n_settings.Length(); i++)
  {
    Napi::Object obj = n_settings.Get(i).As<Napi::Object>();
    CameraSettingSetter setting;
    setting.prop = obj.Get("prop").As<Napi::Number>().Int32Value();
    setting.val = obj.Get("val").As<Napi::Number>().Int32Value();
    setting.flags = obj.Get("flags").As<Napi::Number>().Int32Value();
    psettings->push_back(setting);
  }

  if (info[0].IsString())
  {
    std::string cameraName = info[0].As<Napi::String>().Utf8Value();
    std::wstring wCameraName = std::wstring(cameraName.begin(), cameraName.end());
    testData->nativeThread = std::thread(Callback_SetCameraSettings, testData, psettings, wCameraName, -1);
  }
  else
  {
    Napi::Number index = info[0].As<Napi::Number>();
    int cameraIndex = index.Int32Value();
    testData->nativeThread = std::thread(Callback_SetCameraSettings, testData, psettings, L"", index);
  }

  return env.Undefined();
}

Napi::Value N_SetCameraSettingsSync(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  try
  {
    Napi::Array n_settings = info[1].As<Napi::Array>();

    std::vector<CameraSettingSetter> settings;
    for (size_t i = 0; i < n_settings.Length(); i++)
    {
      Napi::Object obj = n_settings.Get(i).As<Napi::Object>();
      CameraSettingSetter setting;
      setting.prop = obj.Get("prop").As<Napi::Number>().Int32Value();
      setting.val = obj.Get("val").As<Napi::Number>().Int32Value();
      setting.flags = obj.Get("flags").As<Napi::Number>().Int32Value();
      settings.push_back(setting);
    }

    if (info[0].IsString())
    {
      std::string cameraName = info[0].As<Napi::String>().Utf8Value();
      std::wstring wCameraName = std::wstring(cameraName.begin(), cameraName.end());
      SetCameraSettings(wCameraName.c_str(), settings);
    }
    else
    {
      Napi::Number index = info[0].As<Napi::Number>();
      int cameraIndex = index.Int32Value();
      SetCameraSettings(cameraIndex, settings);
    }
  }
  catch (const std::exception &e)
  {
    Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
  }
  return env.Null();
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports["N_GetCameraSettings"] = Napi::Function::New(env, N_GetCameraSettings);
  exports["N_SetCameraSettings"] = Napi::Function::New(env, N_SetCameraSettings);
  exports["N_GetCameraSettingsSync"] = Napi::Function::New(env, N_GetCameraSettingsSync);
  exports["N_SetCameraSettingsSync"] = Napi::Function::New(env, N_SetCameraSettingsSync);
  return exports;
}

NODE_API_MODULE(camera_settings, Init)