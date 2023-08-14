#pragma once
#ifndef SETTINGS_PROMISE_WORKER_H
#define SETTINGS_PROMISE_WORKER_H

#include <napi.h>
#include "camera_settings_base.h"

class SettingsPromiseWorker : public Napi::AsyncWorker
{
public:
  SettingsPromiseWorker(const Napi::Env &env, const char *resource_name, std::wstring wCameraName, int index)
      : Napi::AsyncWorker{env, resource_name},
        m_deferred{env},
        m_wCameraName(wCameraName),
        m_index{index} {}

  ~SettingsPromiseWorker()
  {
  }

  Napi::Promise Promise()
  {
    return m_deferred.Promise();
  }

  virtual void Execute2() {}

  void Execute()
  {
    try
    {
      Execute2();
    }
    catch (const std::exception &e)
    {
      errorMsg = e.what();
    }
  }
  virtual void OnOK2() {}

  void OnOK()
  {
    if (!errorMsg.empty())
    {
      m_deferred.Reject(Napi::String::New(Env(), errorMsg));
      return;
    }
    try
    {
      OnOK2();
    }
    catch (const std::exception &e)
    {
      m_deferred.Reject(Napi::String::New(Env(), e.what()));
    }
  }

  void OnError(const Napi::Error &err) { m_deferred.Reject(err.Value()); }

protected:
  Napi::Promise::Deferred m_deferred;
  std::wstring m_wCameraName;
  int m_index;
  std::string errorMsg;
};

#endif // SETTINGS_PROMISE_WORKER_H
