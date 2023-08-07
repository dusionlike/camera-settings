#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include "win/camera_settings.h"

void GetCameraSettingsByCtrl(std::vector<CameraSetting> &settings, IAMVideoProcAmp *pProcAmp, IAMCameraControl *pCameraControl);
void SetCameraSettingsByCtrl(const std::vector<CameraSettingSetter> &settings, IAMVideoProcAmp *pProcAmp, IAMCameraControl *pCameraControl);

/**
 * Query IAMCameraControl by camera name
 */
HRESULT QueryAllInterface(const WCHAR *wszName, int index, IAMVideoProcAmp **ppProcAmp, IAMCameraControl **ppCameraControl)
{
  // create system device enumerator
  ICreateDevEnum *pCreateDevEnum = NULL;
  HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pCreateDevEnum);
  if (FAILED(hr))
  {
    return hr;
  }

  // create video input device enumerator
  IEnumMoniker *pEnumMoniker = NULL;
  hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
  if (FAILED(hr))
  {
    pCreateDevEnum->Release();
    return hr;
  }

  // enumerate video input devices
  IMoniker *pMoniker = NULL;
  int count = -1;
  while (pEnumMoniker->Next(1, &pMoniker, NULL) == S_OK)
  {
    count++;
    // get device friendly name
    IPropertyBag *pPropertyBag = NULL;
    hr = pMoniker->BindToStorage(NULL, NULL, IID_IPropertyBag, (void **)&pPropertyBag);
    if (FAILED(hr))
    {
      pMoniker->Release();
      continue;
    }

    VARIANT varName;
    VariantInit(&varName);
    hr = pPropertyBag->Read(L"FriendlyName", &varName, NULL);
    if (FAILED(hr))
    {
      pPropertyBag->Release();
      pMoniker->Release();
      continue;
    }

    // compare device friendly name with specified name
    if (count == index || wcscmp(varName.bstrVal, wszName) == 0)
    {
      // create video capture filter
      IBaseFilter *pVideoCaptureFilter = NULL;
      hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void **)&pVideoCaptureFilter);
      if (FAILED(hr))
      {
        VariantClear(&varName);
        pPropertyBag->Release();
        pMoniker->Release();
        continue;
      }

      // get IAMCameraControl interface
      hr = pVideoCaptureFilter->QueryInterface(IID_IAMCameraControl, (void **)ppCameraControl);
      if (FAILED(hr))
      {
        VariantClear(&varName);
        pVideoCaptureFilter->Release();
        pPropertyBag->Release();
        pMoniker->Release();
        continue;
      }

      // get IAMVideoProcAmp interface
      hr = pVideoCaptureFilter->QueryInterface(IID_IAMVideoProcAmp, (void **)ppProcAmp);
      if (FAILED(hr))
      {
        VariantClear(&varName);
        pVideoCaptureFilter->Release();
        pPropertyBag->Release();
        pMoniker->Release();
        continue;
      }

      // release resources
      VariantClear(&varName);
      pVideoCaptureFilter->Release();
      pPropertyBag->Release();
      pMoniker->Release();

      // found the device and got the IAMCameraControl interface
      pEnumMoniker->Release();
      pCreateDevEnum->Release();
      return S_OK;
    }

    // release resources
    VariantClear(&varName);
    pPropertyBag->Release();
    pMoniker->Release();
  }

  // device not found
  pEnumMoniker->Release();
  pCreateDevEnum->Release();
  return E_FAIL;
}

std::vector<CameraSetting> GetCameraSettings(int &cameraIndex)
{
  CoInitialize(NULL);

  IAMVideoProcAmp *pProcAmp = NULL;
  IAMCameraControl *pCameraControl = NULL;

  std::vector<CameraSetting> settings;

  HRESULT hr = QueryAllInterface(L"", cameraIndex, &pProcAmp, &pCameraControl);
  if (FAILED(hr))
  {
    std::cerr << "Failed to get IAMCameraControl interface" << std::endl;
  }
  else
  {
    GetCameraSettingsByCtrl(settings, pProcAmp, pCameraControl);
  }

  CoUninitialize();

  return settings;
}

std::vector<CameraSetting> GetCameraSettings(const WCHAR *wszName)
{
  CoInitialize(NULL);

  IAMVideoProcAmp *pProcAmp = NULL;
  IAMCameraControl *pCameraControl = NULL;

  std::vector<CameraSetting> settings;

  HRESULT hr = QueryAllInterface(wszName, -1, &pProcAmp, &pCameraControl);
  if (FAILED(hr))
  {
    std::cerr << "Failed to query device" << std::endl;
  }
  else
  {
    GetCameraSettingsByCtrl(settings, pProcAmp, pCameraControl);
  }

  CoUninitialize();

  return settings;
}

void GetCameraSettingsByCtrl(std::vector<CameraSetting> &settings, IAMVideoProcAmp *pProcAmp, IAMCameraControl *pCameraControl)
{
  HRESULT hr;
  for (int i = 0; i <= VideoProcAmp_Gain; i++)
  {
    long min, max, val, step, def, range_flags, flags;
    hr = pProcAmp->GetRange(i, &min, &max, &step, &def, &range_flags);
    if (FAILED(hr))
    {
      std::cerr << "Failed to get range" << std::endl;
      continue;
    }

    hr = pProcAmp->Get(i, &val, &flags);
    if (FAILED(hr))
    {
      std::cerr << "Failed to get value" << std::endl;
      continue;
    }

    CameraSetting setting;
    setting.prop = i + 100;
    setting.min = min;
    setting.max = max;
    setting.val = val;
    setting.step = step;
    setting.def = def;
    setting.rangeFlags = range_flags;
    setting.flags = flags;
    setting.type = 0;
    settings.push_back(setting);
  }

  for (int i = 0; i <= CameraControl_Focus; i++)
  {
    long min, max, val, step, def, range_flags, flags;
    hr = pCameraControl->GetRange(i, &min, &max, &step, &def, &range_flags);
    if (FAILED(hr))
    {
      std::cerr << "Failed to get range" << std::endl;
      continue;
    }

    hr = pCameraControl->Get(i, &val, &flags);
    if (FAILED(hr))
    {
      std::cerr << "Failed to get value" << std::endl;
      continue;
    }

    CameraSetting setting;
    setting.prop = i + 200;
    setting.min = min;
    setting.max = max;
    setting.val = val;
    setting.step = step;
    setting.def = def;
    setting.rangeFlags = range_flags;
    setting.flags = flags;
    setting.type = 1;
    settings.push_back(setting);
  }
}

void SetCameraSettings(const WCHAR *wszName, const std::vector<CameraSettingSetter> &settings)
{
  CoInitialize(NULL);

  IAMVideoProcAmp *pProcAmp = NULL;
  IAMCameraControl *pCameraControl = NULL;
  HRESULT hr = QueryAllInterface(wszName, -1, &pProcAmp, &pCameraControl);
  if (FAILED(hr))
  {
    std::cerr << "Failed to query device" << std::endl;
  }
  else
  {
    SetCameraSettingsByCtrl(settings, pProcAmp, pCameraControl);
  }

  CoUninitialize();
}

void SetCameraSettings(int &index, const std::vector<CameraSettingSetter> &settings)
{
  CoInitialize(NULL);

  IAMVideoProcAmp *pProcAmp = NULL;
  IAMCameraControl *pCameraControl = NULL;
  HRESULT hr = QueryAllInterface(L"", index, &pProcAmp, &pCameraControl);
  if (FAILED(hr))
  {
    std::cerr << "Failed to query device" << std::endl;
  }
  else
  {
    SetCameraSettingsByCtrl(settings, pProcAmp, pCameraControl);
  }

  CoUninitialize();
}

void SetCameraSettingsByCtrl(const std::vector<CameraSettingSetter> &settings, IAMVideoProcAmp *pProcAmp, IAMCameraControl *pCameraControl)
{
  HRESULT hr;
  for (auto setting : settings)
  {
    if (setting.prop >= 100 && setting.prop < 200)
    {
      hr = pProcAmp->Set(setting.prop - 100, setting.val, setting.flags);
      if (FAILED(hr))
      {
        std::cerr << "Failed to set value" << std::endl;
        continue;
      }
    }
    else if (setting.prop >= 200 && setting.prop < 300)
    {
      hr = pCameraControl->Set(setting.prop - 200, setting.val, setting.flags);
      if (FAILED(hr))
      {
        std::cerr << "Failed to set value" << std::endl;
        continue;
      }
    }
  }
}
#else
#endif