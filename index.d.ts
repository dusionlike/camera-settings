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
  | "pan"
  | "tilt"
  | "roll"
  | "zoom"
  | "exposure"
  | "iris"
  | "focus";

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
  | "brightness"
  | "contrast"
  | "hue"
  | "saturation"
  | "sharpness"
  | "gamma"
  | "color_enable"
  | "white_balance"
  | "backlight_compensation"
  | "gain";

export interface CameraSettingSetter {
  prop: TagCameraControlProperty | TagVideoProcAmpProperty;
  val: number;
  flags: number;
}

export interface CameraSetting extends CameraSettingSetter {
  min: number;
  max: number;
  step: number;
  range_flags: number;
  def: number;
  type: number;
}

export function GetCameraSettings(cameraName: string | number): CameraSetting[];

export function SetCameraSettings(
  cameraName: string | number,
  settings: CameraSettingSetter[],
): void;
