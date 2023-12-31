/**
 * pan 水平旋转（全景)
 * tilt 垂直旋转（倾斜）
 * roll 旋转（滚动）
 * zoom 缩放
 * exposure 曝光
 * iris 光圈
 * focus 焦距
 */
export type TagCameraControlProperty =
  | "Pan"
  | "Tilt"
  | "Roll"
  | "Zoom"
  | "Exposure"
  | "Iris"
  | "Focus";

/**
 * brightness 亮度
 * contrast 对比度
 * hue 色调
 * saturation 饱和度
 * sharpness 锐度（清晰度）
 * gamma 伽马
 * color_enable 颜色使能
 * white_balance 白平衡
 * backlight_compensation 背光补偿
 * gain 增益
 */
export type TagVideoProcAmpProperty =
  | "Brightness"
  | "Contrast"
  | "Hue"
  | "Saturation"
  | "Sharpness"
  | "Gamma"
  | "ColorEnable"
  | "WhiteBalance"
  | "BacklightCompensation"
  | "Gain";

export interface CameraSettingSetInfo {
  prop: TagCameraControlProperty | TagVideoProcAmpProperty;
  val: number;
  isAuto?: boolean;
}

export interface CameraSettingInfo extends CameraSettingSetInfo {
  min: number;
  max: number;
  step: number;
  /** rangeFlags & 1 Auto; rangeFlags & 2 Auto Manual */
  rangeFlags: number;
  def: number;
  ctrlType: "video" | "camera";
}

export interface Resolution {
  width: number;
  height: number;
  type: string;
}

export function openCameraSettings(cameraName: string | number): Promise<void>;
export function closeCameraSettings(cameraName: string | number): Promise<void>;
export function closeCameraSettingsSync(cameraName: string | number): void;

export function getCameraSettings(
  cameraName: string | number
): Promise<CameraSettingInfo[]>;

export function setCameraSettings(
  cameraName: string | number,
  settings: CameraSettingSetInfo[]
): Promise<void>;

export function getCameraResolutions(
  cameraName: string | number
): Promise<Resolution[]>;

export class CameraSettings {
  constructor(cameraName: string | number);
  open(): Promise<void>;
  close(): Promise<void>;
  getSettings(): Promise<CameraSettingInfo[]>;
  setSettings(settings: CameraSettingSetInfo[]): Promise<void>;
  getResolutions(): Promise<Resolution[]>;
} 

export function _getCacheCount(): number;
