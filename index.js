"use strict";
const bindings = require("bindings")("camera-settings");

const propMap = new Map([
  [100, "brightness"],
  [101, "contrast"],
  [102, "hue"],
  [103, "saturation"],
  [104, "sharpness"],
  [105, "gamma"],
  [106, "color_enable"],
  [107, "white_balance"],
  [108, "backlight_compensation"],
  [109, "gain"],
  [200, "pan"],
  [201, "tilt"],
  [202, "roll"],
  [203, "zoom"],
  [204, "exposure"],
  [205, "iris"],
  [206, "focus"],
]);

const propMapReverse = new Map([...propMap].map(([k, v]) => [v, k]));

module.exports.GetCameraSettings = function (cameraName) {
  const settings = bindings.N_GetCameraSettings(cameraName);
  for (const item of settings) {
    item.prop = propMap.get(item.prop);
  }
  return settings;
};

module.exports.SetCameraSettings = function (cameraName, settings) {
  for (const item of settings) {
    item.prop = propMapReverse.get(item.prop);
  }
  bindings.N_SetCameraSettings(cameraName, settings);
}
