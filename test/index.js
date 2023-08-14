// @ts-check
const { CameraSettings } = require("../");

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

(async () => {
  try {
    const cs = new CameraSettings(0);

    await cs.open();

    console.time("getCameraSettings");
    const res1 = await cs.getSettings();
    console.timeEnd("getCameraSettings");

    console.time("getCameraSettings2");
    const res3 = await cs.getSettings();
    console.timeEnd("getCameraSettings2");

    // console.log(res3);

    console.time("setCameraSettings");
    await cs.setSettings([
      { prop: "WhiteBalance", val: 2900, isAuto: true },
    ]);
    console.timeEnd("setCameraSettings");

    // console.log(res1);

    console.time("getCameraResolutions");
    const res2 = await cs.getResolutions();
    console.timeEnd("getCameraResolutions");

    // console.log(res2);

    await cs.close();
  } catch (error) {
    console.error(error);
  }
})();
