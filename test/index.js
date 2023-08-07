// @ts-check
const CameraSettings = require("../");

console.time("GetCameraSettings");
const ss = CameraSettings.GetCameraSettings(0);
console.timeEnd("GetCameraSettings");

console.log(ss);
console.log(ss.length);

console.time("SetCameraSettings");
CameraSettings.SetCameraSettings(0, [
  { prop: "white_balance", val: 2900, isAuto: true },
]);
console.timeEnd("SetCameraSettings");
