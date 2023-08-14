# camera-settings [![npm](https://img.shields.io/npm/v/camera-settings.svg)](https://npmjs.com/package/camera-settings)

[English](./README.md) | 简体中文

获取和修改 usb 摄像头的配置，支持 Windows 和 Linux。一般用于 Electron 应用。

- 在 Windows 上，使用 [DirectShow](https://docs.microsoft.com/en-us/windows/win32/directshow/directshow)。
- 在 Linux 上，使用 [v4l2](https://www.kernel.org/doc/html/v4.9/media/uapi/v4l/v4l2.html)。

![](./demo.png)

## 安装

```bash
npm install camera-settings
```

## 使用

```ts
import { CameraSettings } from "camera-settings";

(async () => {
  const cs = new CameraSettings(0);
  await cs.open();

  // 获取摄像头配置
  const settings = await cs.getSettings();

  // 设置摄像头配置，如白平衡
  await cs.setSettings([{ prop: "WhiteBalance", val: 2900, isAuto: true }]);

  // 获取所有可选分辨率
  const resolutions = await cs.getResolutions();

  await cs.close();
})();
```

## API

### `CameraSettings`

#### `constructor(cameraId: number | string)`

- `cameraId` 摄像头索引，从 0 开始，或者摄像头名称，如 `HD USB Camera`

#### `open(): Promise<void>`

做一些准备工作，比如查找设备。

#### `close(): Promise<void>`

清理缓存，释放资源。

#### `getSettings(): Promise<CameraSetting[]>`

获取摄像头配置。

```ts
interface CameraSettingInfo extends CameraSettingSetInfo {
  prop: TagCameraControlProperty | TagVideoProcAmpProperty;
  val: number;
  isAuto?: boolean;
  min: number;
  max: number;
  step: number;
  /** rangeFlags & 1 Auto; rangeFlags & 2 Auto Manual */
  rangeFlags: number;
  /** 默认配置 */
  def: number;
  ctrlType: "video" | "camera";
}
```

#### `setSettings(settings: CameraSetting[]): Promise<void>`

设置摄像头配置。

```ts
interface CameraSetting {
  prop: TagCameraControlProperty | TagVideoProcAmpProperty;
  val: number;
  isAuto?: boolean;
}
```

#### `getResolutions(): Promise<Resolution[]>`

获取所有可选分辨率。

```ts
interface Resolution {
  width: number;
  height: number;
  /** mjpg, yuy2, yuyv... */
  type: string;
}
```
