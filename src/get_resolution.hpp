#include <napi.h>
#include "camera_settings_base.h"
#include "settings_promise_worker.hpp"

Napi::Object GetResolution(Napi::Env env, Resolution resolution);

class GetResolutionsWorker : public SettingsPromiseWorker
{
public:
  GetResolutionsWorker(const Napi::Env &env, std::wstring wCameraName, int index)
      : SettingsPromiseWorker{env, "GetSettingsWorker", wCameraName, index} {}

  void Execute2()
  {
    m_resolutions = GetCameraResolutions(m_wCameraName.c_str(), m_index);
  }

  void OnOK2()
  {
    Napi::Env env = Env();
    Napi::Array arr = Napi::Array::New(env, m_resolutions.size());
    for (size_t i = 0; i < m_resolutions.size(); i++)
    {
      arr.Set(i, GetResolution(env, m_resolutions[i]));
    }
    m_deferred.Resolve(arr);
  }

private:
  std::vector<Resolution> m_resolutions;
};

Napi::Object GetResolution(Napi::Env env, Resolution resolution)
{
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "width"), Napi::Number::New(env, resolution.width));
  obj.Set(Napi::String::New(env, "height"), Napi::Number::New(env, resolution.height));
  obj.Set(Napi::String::New(env, "type"), Napi::String::New(env, resolution.type));
  return obj;
}
