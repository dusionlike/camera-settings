"use strict";
const bindings = require("bindings")("camera-settings");

class CameraSettings {
  constructor(cameraId) {
    this.cameraId = cameraId;
  }

  async open() {
    await bindings.openCameraSettings(this.cameraId);
    if (global.window) {
      const cameraId = this.cameraId;
      this.bindClose = () => {
        bindings.closeCameraSettingsSync(cameraId)
      };
      window.addEventListener("beforeunload", this.bindClose);
    }
  }
  close() {
    if (global.window && this.bindClose) {
      window.removeEventListener("beforeunload", this.bindClose);
    }
    return bindings.closeCameraSettings(this.cameraId);
  }

  getSettings() {
    return bindings.getCameraSettings(this.cameraId);
  }

  setSettings(settings) {
    return bindings.setCameraSettings(this.cameraId, settings);
  }

  getResolutions() {
    return bindings.getCameraResolutions(this.cameraId);
  }
}

module.exports = { CameraSettings, ...bindings };
