#include <napi.h>
#include "camera_settings_base.h"
#include "settings_promise_worker.hpp"

Napi::Object GetSetting(Napi::Env env, CameraSetting setting);

class GetSettingsWorker : public SettingsPromiseWorker
{
public:
  GetSettingsWorker(const Napi::Env &env, std::wstring wCameraName, int index)
      : SettingsPromiseWorker{env, "GetSettingsWorker", wCameraName, index} {}

  void Execute2()
  {
    m_settings = GetCameraSettings(m_wCameraName.c_str(), m_index);
  }

  void OnOK2()
  {
    Napi::Env env = Env();
    Napi::Array arr = Napi::Array::New(env, m_settings.size());
    for (size_t i = 0; i < m_settings.size(); i++)
    {
      arr.Set(i, GetSetting(env, m_settings[i]));
    }
    m_deferred.Resolve(arr);
  }

private:
  std::vector<CameraSetting> m_settings;
};

Napi::Object GetSetting(Napi::Env env, CameraSetting setting)
{
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "prop"), Napi::String::New(env, setting.prop));
  obj.Set(Napi::String::New(env, "min"), Napi::Number::New(env, setting.min));
  obj.Set(Napi::String::New(env, "max"), Napi::Number::New(env, setting.max));
  obj.Set(Napi::String::New(env, "val"), Napi::Number::New(env, setting.val));
  obj.Set(Napi::String::New(env, "step"), Napi::Number::New(env, setting.step));
  obj.Set(Napi::String::New(env, "def"), Napi::Number::New(env, setting.def));
  obj.Set(Napi::String::New(env, "rangeFlags"), Napi::Number::New(env, setting.rangeFlags));
  obj.Set(Napi::String::New(env, "isAuto"), Napi::Boolean::New(env, setting.isAuto));
  obj.Set(Napi::String::New(env, "ctrlType"), Napi::String::New(env, setting.ctrlType));
  return obj;
}
