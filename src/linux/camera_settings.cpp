#if defined(linux) || defined(__linux) || defined(__linux__)

#include "linux/camera_settings.h"
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

std::vector<CameraSetting> GetCameraSettings(const wchar_t *wszName)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  std::string name = converter.to_bytes(wszName);
  return GetCameraSettingsByFd(queryVideoFdByName(name));
}

std::vector<CameraSetting> GetCameraSettings(int cameraIndex)
{
  return GetCameraSettingsByFd(queryVideoFdByIndex(cameraIndex));
}

std::vector<CameraSetting> GetCameraSettingsByFd(int fd)
{
  std::vector<CameraSetting> settings;
  if (fd == -1)
  {
    return settings;
  }
  close(fd);
  fd = open("/dev/video0", O_RDWR);

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
      setting.prop = queryctrl.id;
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
        setting.flags = ctrl.value == 1 ? 1 : 2;
        break;
      case V4L2_CID_EXPOSURE_ABSOLUTE:
        ctrl.id = V4L2_CID_EXPOSURE_AUTO;
        ioctl(fd, VIDIOC_G_CTRL, &ctrl);
        // 3 auto, 1 manual
        setting.flags = ctrl.value == 3 ? 1 : 2;
        break;
      case V4L2_CID_FOCUS_ABSOLUTE:
        ctrl.id = V4L2_CID_FOCUS_AUTO;
        ioctl(fd, VIDIOC_G_CTRL, &ctrl);
        setting.flags = ctrl.value == 1 ? 1 : 2;
        break;
      default:
        setting.flags = 2;
        break;
      }

      setting.type = i < 10 ? 0 : 1;
      settings.push_back(setting);
    }
  }

  close(fd);
  return settings;
}

void SetCameraSettings(const wchar_t *wszName, const std::vector<CameraSettingSetter> &settings)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  std::string name = converter.to_bytes(wszName);
  return SetCameraSettingsByFd(queryVideoFdByName(name), settings);
}

void SetCameraSettings(int cameraIndex, const std::vector<CameraSettingSetter> &settings)
{
  return SetCameraSettingsByFd(queryVideoFdByIndex(cameraIndex), settings);
}

void SetCameraSettingsByFd(int fd, const std::vector<CameraSettingSetter> &settings)
{
  if (fd == -1)
  {
    return;
  }
  close(fd);
  fd = open("/dev/video0", O_RDWR);

  struct v4l2_control ctrl;
  memset(&ctrl, 0, sizeof(ctrl));

  for (size_t i = 0; i < settings.size(); i++)
  {
    switch (settings[i].prop)
    {
    case V4L2_CID_WHITE_BALANCE_TEMPERATURE:
      ctrl.id = V4L2_CID_AUTO_WHITE_BALANCE;
      ctrl.value = settings[i].flags == 1 ? 1 : 0;
      ioctl(fd, VIDIOC_S_CTRL, &ctrl);
      break;
    case V4L2_CID_EXPOSURE_ABSOLUTE:
      ctrl.id = V4L2_CID_EXPOSURE_AUTO;
      ctrl.value = settings[i].flags == 1 ? 3 : 1;
      ioctl(fd, VIDIOC_S_CTRL, &ctrl);
      break;
    case V4L2_CID_FOCUS_ABSOLUTE:
      ctrl.id = V4L2_CID_FOCUS_AUTO;
      ctrl.value = settings[i].flags == 1 ? 1 : 0;
      ioctl(fd, VIDIOC_S_CTRL, &ctrl);
      break;
    default:
      break;
    }
    ctrl.id = settings[i].prop;
    ctrl.value = settings[i].val;

    int err = errno;
    if (-1 == ioctl(fd, VIDIOC_S_CTRL, &ctrl))
    {
      std::cout << "set " << ctrl.id << " to " << ctrl.value << " failed: " << strerror(err) << std::endl;
    }
  }

  close(fd);
}

#else
#endif