#include <napi.h>
#include "camera_settings_base.h"
#include "settings_promise_worker.hpp"

class OpenSettingsWorker : public SettingsPromiseWorker
{
public:
  OpenSettingsWorker(const Napi::Env &env, std::wstring wCameraName, int index)
      : SettingsPromiseWorker{env, "OpenSettingsWorker", wCameraName, index} {}

  void Execute2()
  {
    OpenCameraSettings(m_wCameraName.c_str(), m_index);
  }

  void OnOK2()
  {
    m_deferred.Resolve(Env().Undefined());
  }
};
