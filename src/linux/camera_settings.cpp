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

wchar_t *convert(unsigned char *str)
{
  std::string s(reinterpret_cast<char *>(str));

  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  std::wstring ws = converter.from_bytes(s);

  return const_cast<wchar_t *>(ws.c_str());
}

int queryVideoFdByIndex(int &index)
{
  char devName[32];
  sprintf(devName, "/dev/video%d", index);
  int fd = open(devName, O_RDWR);
  return fd;
}

int queryVideoFdByName(const wchar_t *wszName)
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
    wchar_t *capCard = convert(cap.card);
    if (wcscmp(wszName, capCard) == 0)
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
  return GetCameraSettingsByV4l2(queryVideoFdByName(wszName));
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
  struct v4l2_queryctrl queryctrl;
  memset(&queryctrl, 0, sizeof(queryctrl));
  if (0 == ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) && !(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED))
  {
    CameraSetting setting;
    setting.prop = queryctrl.id;
    setting.min = queryctrl.minimum;
    setting.max = queryctrl.maximum;
    setting.val = queryctrl.default_value;
    setting.step = queryctrl.step;
    setting.def = queryctrl.default_value;
    setting.range_flags = queryctrl.flags;
    setting.flags = queryctrl.flags;
    setting.type = queryctrl.type;
    settings.push_back(setting);
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