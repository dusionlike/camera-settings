// @ts-check
const CameraSettings = require("../");

const ss = CameraSettings.GetCameraSettings(0);

console.log(ss);
console.log(ss.length);

CameraSettings.SetCameraSettings(0, [
  { prop: "white_balance", val: 2900, flags: 1 },
]);
