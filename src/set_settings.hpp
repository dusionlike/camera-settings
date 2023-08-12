#include <napi.h>
#include "camera_settings_base.h"

class SetSettingsWorker : public Napi::AsyncWorker
{
public:
  SetSettingsWorker(const Napi::Env &env, std::wstring wCameraName, int index, const std::vector<CameraSettingSetter> &settings)
      : Napi::AsyncWorker{env, "SetSettingsWorker"},
        m_deferred{env},
        m_wCameraName(wCameraName),
        m_index{index},
        m_settings{settings} {}

  ~SetSettingsWorker()
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
      SetCameraSettings(m_wCameraName.c_str(), m_index, m_settings);
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
    m_deferred.Resolve(Env().Undefined());
  }

  void OnError(const Napi::Error &err) { m_deferred.Reject(err.Value()); }

private:
  Napi::Promise::Deferred m_deferred;
  std::wstring m_wCameraName;
  int m_index;
  std::vector<CameraSettingSetter> m_settings;
  std::string errorMsg;
};
