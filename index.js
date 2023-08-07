"use strict";
const bindings = require("bindings")("camera-settings");

/**
 * @type {Map<number, string>}
 */
let propMap;

if (process.platform === "win32") {
  propMap = new Map([
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
} else {
  propMap = new Map([
    [9963776, "brightness"],
    [9963777, "contrast"],
    [9963779, "hue"],
    [9963778, "saturation"],
    [9963803, "sharpness"],
    [9963792, "gamma"],
    [9963806, "color_enable"],
    [9963802, "white_balance"],
    [9963804, "backlight_compensation"],
    [9963795, "gain"],
    [10094856, "pan"],
    [10094857, "tilt"],
    [0, "roll"],
    [10094861, "zoom"],
    [10094850, "exposure"],
    [10094865, "iris"],
    [10094858, "focus"],
  ])
}

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
};
