"use strict";
const bindings = require("bindings")("camera-settings");

class CameraSettings {
  constructor(cameraId) {
    this.cameraId = cameraId;
  }

  open() {
    return bindings.openCameraSettings(this.cameraId);
  }
  close() {
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
