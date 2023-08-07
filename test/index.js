// @ts-check
const CameraSettings = require("../");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

(async () => {
  console.time("GetCameraSettings");
  await CameraSettings.GetCameraSettings(0);
  console.timeEnd("GetCameraSettings");

  // await sleep(0);

  // console.time("GetCameraSettings2");
  // await CameraSettings.GetCameraSettings(0);
  // console.timeEnd("GetCameraSettings2");

  console.time("SetCameraSettings");
  await CameraSettings.SetCameraSettings(0, [
    { prop: "white_balance", val: 2900, isAuto: true },
  ]);
  console.timeEnd("SetCameraSettings");
})();

// console.time("GetCameraSettingsSync");
// const ss = CameraSettings.GetCameraSettingsSync(0);
// console.timeEnd("GetCameraSettingsSync");

// // console.log(ss);
// console.log(ss.length);

// console.time("SetCameraSettingsSync");
// CameraSettings.SetCameraSettingsSync(0, [
//   { prop: "white_balance", val: 2900, isAuto: true },
// ]);
// console.timeEnd("SetCameraSettingsSync");
