#if defined(linux) || defined(__linux) || defined(__linux__)

#include "camera_settings_base.h"
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string>
#include <locale>
#include <codecvt>
#include <cstring>

std::vector<CameraSetting> GetCameraSettingsByFd(int fd);
void SetCameraSettingsByFd(int fd, const std::vector<CameraSettingSetter> &settings);
std::vector<Resolution> GetCameraResolutionsByFd(int fd);

const uint32_t ctrl_id_list[] =
    {V4L2_CID_BRIGHTNESS,
     V4L2_CID_CONTRAST,
     V4L2_CID_HUE,
     V4L2_CID_SATURATION,
     V4L2_CID_SHARPNESS,
     V4L2_CID_GAMMA,
     V4L2_CID_COLOR_KILLER,
     V4L2_CID_WHITE_BALANCE_TEMPERATURE,
     V4L2_CID_BACKLIGHT_COMPENSATION,
     V4L2_CID_GAIN,
     V4L2_CID_PAN_ABSOLUTE,
     V4L2_CID_TILT_ABSOLUTE,
     0,
     V4L2_CID_ZOOM_ABSOLUTE,
     V4L2_CID_EXPOSURE_ABSOLUTE,
     V4L2_CID_IRIS_ABSOLUTE,
     V4L2_CID_FOCUS_ABSOLUTE};

std::map<int, std::string> propMap = {
    {V4L2_CID_BRIGHTNESS, "Brightness"},
    {V4L2_CID_CONTRAST, "Contrast"},
    {V4L2_CID_HUE, "Hue"},
    {V4L2_CID_SATURATION, "Saturation"},
    {V4L2_CID_SHARPNESS, "Sharpness"},
    {V4L2_CID_GAMMA, "Gamma"},
    {V4L2_CID_COLOR_KILLER, "ColorEnable"},
    {V4L2_CID_WHITE_BALANCE_TEMPERATURE, "WhiteBalance"},
    {V4L2_CID_BACKLIGHT_COMPENSATION, "BacklightCompensation"},
    {V4L2_CID_GAIN, "Gain"},
    {V4L2_CID_PAN_ABSOLUTE, "Pan"},
    {V4L2_CID_TILT_ABSOLUTE, "Tilt"},
    {0, "Roll"},
    {V4L2_CID_ZOOM_ABSOLUTE, "Zoom"},
    {V4L2_CID_EXPOSURE_ABSOLUTE, "Exposure"},
    {V4L2_CID_IRIS_ABSOLUTE, "Iris"},
    {V4L2_CID_FOCUS_ABSOLUTE, "Focus"},
};

std::map<std::string, int> propMapReverse = {
    {"Brightness", V4L2_CID_BRIGHTNESS},
    {"Contrast", V4L2_CID_CONTRAST},
    {"Hue", V4L2_CID_HUE},
    {"Saturation", V4L2_CID_SATURATION},
    {"Sharpness", V4L2_CID_SHARPNESS},
    {"Gamma", V4L2_CID_GAMMA},
    {"ColorEnable", V4L2_CID_COLOR_KILLER},
    {"WhiteBalance", V4L2_CID_WHITE_BALANCE_TEMPERATURE},
    {"BacklightCompensation", V4L2_CID_BACKLIGHT_COMPENSATION},
    {"Gain", V4L2_CID_GAIN},
    {"Pan", V4L2_CID_PAN_ABSOLUTE},
    {"Tilt", V4L2_CID_TILT_ABSOLUTE},
    {"Roll", 0},
    {"Zoom", V4L2_CID_ZOOM_ABSOLUTE},
    {"Exposure", V4L2_CID_EXPOSURE_ABSOLUTE},
    {"Iris", V4L2_CID_IRIS_ABSOLUTE},
    {"Focus", V4L2_CID_FOCUS_ABSOLUTE},
};

void OpenCameraSettings(const wchar_t *wszName, int index)
{
}

void CloseCameraSettings(const wchar_t *wszName, int index)
{
}

int queryVideoFdByIndex(int &index)
{
  char devName[32];
  sprintf(devName, "/dev/video%d", index);
  int fd = open(devName, O_RDWR);
  return fd;
}

std::string extractName(const std::string &fullName)
{
  size_t pos = fullName.find(":");
  if (pos != std::string::npos)
  {
    return fullName.substr(0, pos);
  }

  return fullName;
}

int queryVideoFdByName(const std::string &queryName)
{
  struct v4l2_capability cap;
  memset(&cap, 0, sizeof(cap));
  int index = 0;
  while (true)
  {
    int fd = queryVideoFdByIndex(index);
    if (fd == -1)
    {
      break;
    }
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
    {
      close(fd);
      break;
    }

    size_t length = sizeof(cap.card) / sizeof(cap.card[0]);
    std::string fullName(reinterpret_cast<const char *>(cap.card), length);
    std::string name = extractName(fullName);

    if (queryName == name)
    {
      return fd;
    }
    close(fd);
    index++;
  }
  return -1;
}

std::vector<CameraSetting> GetCameraSettings(const wchar_t *wszName, int cameraIndex)
{
  if (cameraIndex == -1)
  {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string name = converter.to_bytes(wszName);
    return GetCameraSettingsByFd(queryVideoFdByName(name));
  }
  else
  {
    return GetCameraSettingsByFd(queryVideoFdByIndex(cameraIndex));
  }
}

std::vector<CameraSetting> GetCameraSettingsByFd(int fd)
{
  std::vector<CameraSetting> settings;
  if (fd == -1)
  {
    throw std::runtime_error("Failed to query device");
  }

  struct v4l2_control ctrl;
  memset(&ctrl, 0, sizeof(ctrl));

  struct v4l2_queryctrl queryctrl;
  memset(&queryctrl, 0, sizeof(queryctrl));

  size_t len = sizeof(ctrl_id_list) / sizeof(ctrl_id_list[0]);
  for (size_t i = 0; i < len; i++)
  {
    queryctrl.id = ctrl_id_list[i];
    // std::cout << queryctrl.id << std::endl;
    if (0 != queryctrl.id && 0 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl))
    {
      if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
      {
        continue;
      }

      ctrl.id = queryctrl.id;
      ioctl(fd, VIDIOC_G_CTRL, &ctrl);

      CameraSetting setting;
      setting.prop = propMap[queryctrl.id];
      setting.min = queryctrl.minimum;
      setting.max = queryctrl.maximum;
      setting.val = ctrl.value;
      setting.step = queryctrl.step;
      setting.def = queryctrl.default_value;
      if (queryctrl.id == V4L2_CID_WHITE_BALANCE_TEMPERATURE || queryctrl.id == V4L2_CID_EXPOSURE_ABSOLUTE || queryctrl.id == V4L2_CID_FOCUS_ABSOLUTE)
      {
        setting.rangeFlags = 3;
      }
      else if (queryctrl.id == V4L2_CID_IRIS_ABSOLUTE)
      {
        setting.rangeFlags = 1;
      }
      else
      {
        setting.rangeFlags = 2;
      }

      switch (queryctrl.id)
      {
      case V4L2_CID_WHITE_BALANCE_TEMPERATURE:
        ctrl.id = V4L2_CID_AUTO_WHITE_BALANCE;
        ioctl(fd, VIDIOC_G_CTRL, &ctrl);
        setting.isAuto = ctrl.value == 1;
        break;
      case V4L2_CID_EXPOSURE_ABSOLUTE:
        ctrl.id = V4L2_CID_EXPOSURE_AUTO;
        ioctl(fd, VIDIOC_G_CTRL, &ctrl);
        // 3 auto, 1 manual
        setting.isAuto = ctrl.value == 3;
        break;
      case V4L2_CID_FOCUS_ABSOLUTE:
        ctrl.id = V4L2_CID_FOCUS_AUTO;
        ioctl(fd, VIDIOC_G_CTRL, &ctrl);
        setting.isAuto = ctrl.value == 1;
        break;
      default:
        setting.isAuto = false;
        break;
      }

      setting.ctrlType = i < 10 ? "video" : "camera";
      settings.push_back(setting);
    }
  }

  close(fd);
  return settings;
}

void SetCameraSettings(const wchar_t *wszName, int cameraIndex, const std::vector<CameraSettingSetter> &settings)
{
  if (cameraIndex == -1)
  {

    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string name = converter.to_bytes(wszName);
    return SetCameraSettingsByFd(queryVideoFdByName(name), settings);
  }
  else
  {
    return SetCameraSettingsByFd(queryVideoFdByIndex(cameraIndex), settings);
  }
}

void SetCameraSettingsByFd(int fd, const std::vector<CameraSettingSetter> &settings)
{
  if (fd == -1)
  {
    throw std::runtime_error("Failed to query device");
  }

  struct v4l2_control ctrl;
  memset(&ctrl, 0, sizeof(ctrl));

  for (auto setting : settings)
  {
    int id = propMapReverse[setting.prop];

    switch (id)
    {
    case V4L2_CID_WHITE_BALANCE_TEMPERATURE:
      ctrl.id = V4L2_CID_AUTO_WHITE_BALANCE;
      ctrl.value = setting.isAuto ? 1 : 0;
      ioctl(fd, VIDIOC_S_CTRL, &ctrl);
      break;
    case V4L2_CID_EXPOSURE_ABSOLUTE:
      ctrl.id = V4L2_CID_EXPOSURE_AUTO;
      ctrl.value = setting.isAuto ? 3 : 1;
      ioctl(fd, VIDIOC_S_CTRL, &ctrl);
      break;
    case V4L2_CID_FOCUS_ABSOLUTE:
      ctrl.id = V4L2_CID_FOCUS_AUTO;
      ctrl.value = setting.isAuto ? 1 : 0;
      ioctl(fd, VIDIOC_S_CTRL, &ctrl);
      break;
    default:
      break;
    }
    ctrl.id = id;
    ctrl.value = setting.val;

    // int err = errno;
    if (-1 == ioctl(fd, VIDIOC_S_CTRL, &ctrl))
    {
      // std::cout << "set " << ctrl.id << " to " << ctrl.value << " failed: " << strerror(err) << std::endl;-
    }
  }

  close(fd);
}

std::vector<Resolution> GetCameraResolutions(const wchar_t *wszName, int index)
{
  if (index == -1)
  {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string name = converter.to_bytes(wszName);
    return GetCameraResolutionsByFd(queryVideoFdByName(name));
  }
  else
  {
    return GetCameraResolutionsByFd(queryVideoFdByIndex(index));
  }
}

std::vector<Resolution> GetCameraResolutionsByFd(int fd)
{
  if (fd == -1)
  {
    throw std::runtime_error("Failed to query device");
  }

  std::vector<Resolution> resolutions;

  struct v4l2_frmsizeenum frmsize;

  memset(&frmsize, 0, sizeof(frmsize));
  frmsize.index = 0;
  frmsize.pixel_format = V4L2_PIX_FMT_YUYV;
  while (0 == ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize))
  {
    if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
    {
      Resolution resolution;
      resolution.width = frmsize.discrete.width;
      resolution.height = frmsize.discrete.height;
      resolution.type = "yuyv";
      resolutions.push_back(resolution);
    }
    frmsize.index++;
  }

  memset(&frmsize, 0, sizeof(frmsize));
  frmsize.index = 0;
  frmsize.pixel_format = V4L2_PIX_FMT_MJPEG;
  while (0 == ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize))
  {
    if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
    {
      Resolution resolution;
      resolution.width = frmsize.discrete.width;
      resolution.height = frmsize.discrete.height;
      resolution.type = "mjpg";
      resolutions.push_back(resolution);
    }
    frmsize.index++;
  }

  close(fd);
  return resolutions;
}

#else
#endif