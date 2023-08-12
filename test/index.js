// @ts-check
const CameraSettings = require("../");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

(async () => {
  try {
    console.time("getCameraSettings");
    const res1 = await CameraSettings.getCameraSettings(0);
    console.timeEnd("getCameraSettings");

    console.time("getCameraSettings1");
    await CameraSettings.getCameraSettings(0);
    console.timeEnd("getCameraSettings1");

    console.time("setCameraSettings");
    await CameraSettings.setCameraSettings(0, [
      { prop: "WhiteBalance", val: 2900, isAuto: true },
    ]);
    console.timeEnd("setCameraSettings");

    // console.log(res1);

    console.time("getCameraResolutions");
    const res2 = await CameraSettings.getCameraResolutions(0);
    console.timeEnd("getCameraResolutions");

    console.log(res2);
  } catch (error) {
    console.error(error);
  }
})();
