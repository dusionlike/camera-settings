#include <napi.h>
#include "camera_settings_base.h"
#include "settings_promise_worker.hpp"

class CloseSettingsWorker : public SettingsPromiseWorker
{
public:
  CloseSettingsWorker(const Napi::Env &env, std::wstring wCameraName, int index)
      : SettingsPromiseWorker{env, "GetSettingsWorker", wCameraName, index} {}

  void Execute2()
  {
    CloseCameraSettings(m_wCameraName.c_str(), m_index);
  }

  void OnOK2()
  {
    m_deferred.Resolve(Env().Undefined());
  }
};
