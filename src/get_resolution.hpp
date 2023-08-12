#include <napi.h>
#include "camera_settings_base.h"

Napi::Object GetResolution(Napi::Env env, Resolution resolution);

class GetResolutionsWorker : public Napi::AsyncWorker
{
public:
  GetResolutionsWorker(const Napi::Env &env, std::wstring wCameraName, int index)
      : Napi::AsyncWorker{env, "GetResolutionsWorker"},
        m_deferred{env},
        m_wCameraName(wCameraName),
        m_index{index} {}

  ~GetResolutionsWorker()
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
      m_resolutions = GetCameraResolutions(m_wCameraName.c_str(), m_index);
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
      Napi::Array arr = Napi::Array::New(env, m_resolutions.size());
      for (size_t i = 0; i < m_resolutions.size(); i++)
      {
        arr.Set(i, GetResolution(env, m_resolutions[i]));
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
  std::vector<Resolution> m_resolutions;
  std::string errorMsg;
};

Napi::Object GetResolution(Napi::Env env, Resolution resolution)
{
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "width"), Napi::Number::New(env, resolution.width));
  obj.Set(Napi::String::New(env, "height"), Napi::Number::New(env, resolution.height));
  obj.Set(Napi::String::New(env, "type"), Napi::String::New(env, resolution.type));
  return obj;
}
