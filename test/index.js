// @ts-check
const CameraSettings = require('../');

const ss = CameraSettings.GetCameraSettings('HP HD Camera');

console.log(ss);
console.log(ss.length);

// CameraSettings.SetCameraSettings('HP HD Camera', [
//   { prop: 'exposure', val: 0, flags: 1 },
// ]);