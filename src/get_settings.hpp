#include <napi.h>
#include "camera_settings_base.h"

Napi::Object GetSetting(Napi::Env env, CameraSetting setting);

class GetSettingsWorker : public Napi::AsyncWorker
{
public:
  GetSettingsWorker(const Napi::Env &env, std::wstring wCameraName, int index)
      : Napi::AsyncWorker{env, "GetSettingsWorker"},
        m_deferred{env},
        m_wCameraName(wCameraName),
        m_index{index} {}

  ~GetSettingsWorker()
  {
  }

  Napi::Promise Promise()
  {
    return m_deferred.Promise();
  }

  void Execute()
  {
    try
    {
      m_settings = GetCameraSettings(m_wCameraName.c_str(), m_index);
    }
    catch (const std::exception &e)
    {
      errorMsg = e.what();
    }
  }

  void OnOK()
  {
    if (!errorMsg.empty())
    {
      m_deferred.Reject(Napi::String::New(Env(), errorMsg));
      return;
    }

    try
    {
      Napi::Env env = Env();
      Napi::Array arr = Napi::Array::New(env, m_settings.size());
      for (size_t i = 0; i < m_settings.size(); i++)
      {
        arr.Set(i, GetSetting(env, m_settings[i]));
      }
      m_deferred.Resolve(arr);
    }
    catch (const std::exception &e)
    {
      m_deferred.Reject(Napi::String::New(Env(), e.what()));
    }
  }

  void OnError(const Napi::Error &err) { m_deferred.Reject(err.Value()); }

private:
  Napi::Promise::Deferred m_deferred;
  std::wstring m_wCameraName;
  int m_index;
  std::vector<CameraSetting> m_settings;
  std::string errorMsg;
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
