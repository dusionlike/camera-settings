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

std::vector<CameraSetting> GetCameraSettingsByV4l2(int fd);

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
  return GetCameraSettingsByV4l2(queryVideoFdByName(name));
}

std::vector<CameraSetting> GetCameraSettings(int &cameraIndex)
{
  return GetCameraSettingsByV4l2(queryVideoFdByIndex(cameraIndex));
}

std::vector<CameraSetting> GetCameraSettingsByV4l2(int fd)
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
    std::cout << queryctrl.id << std::endl;
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
        break;
      case V4L2_CID_EXPOSURE_ABSOLUTE:
        ctrl.id = V4L2_CID_EXPOSURE_AUTO_PRIORITY;
        ioctl(fd, VIDIOC_G_CTRL, &ctrl);
        break;
      case V4L2_CID_FOCUS_ABSOLUTE:
        ctrl.id = V4L2_CID_FOCUS_AUTO;
        ioctl(fd, VIDIOC_G_CTRL, &ctrl);
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
  // TODO
}

#else
#endif