#include <napi.h>
#include "camera_settings_base.h"
#include "settings_promise_worker.hpp"

class SetSettingsWorker : public SettingsPromiseWorker
{
public:
  SetSettingsWorker(const Napi::Env &env, std::wstring wCameraName, int index, const std::vector<CameraSettingSetter> &settings)
      : SettingsPromiseWorker{env, "GetSettingsWorker", wCameraName, index},
      m_settings{settings} {}

  void Execute2()
  {
    SetCameraSettings(m_wCameraName.c_str(), m_index, m_settings);
  }

  void OnOK2()
  {
    m_deferred.Resolve(Env().Undefined());
  }

private:
  std::vector<CameraSettingSetter> m_settings;
};
