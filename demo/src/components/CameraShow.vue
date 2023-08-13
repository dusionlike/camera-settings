<script lang="ts" setup>
import { onMounted } from "vue";
import { ref } from "vue";
import { cameraId } from "../store";

const videoRef = ref<HTMLVideoElement>();

const cameraList = ref<MediaDeviceInfo[]>([]);

async function initCamera(re = false) {
  try {
    const devices = await navigator.mediaDevices.enumerateDevices();
    const cameras = devices.filter((device) => device.kind === "videoinput");
    cameraList.value = cameras;
  } catch (error) {
    if (!re) {
      // 如果没有权限，第一次会报错，先请求权限，再重试
      const stream = await navigator.mediaDevices.getUserMedia({ video: true });
      stream.getTracks().forEach((track) => track.stop());
      initCamera();
      return;
    }
    console.error(error);
  }
}

onMounted(() => {
  initCamera();
});

let stream: MediaStream | null = null;
const isOpen = ref(false);

async function openCapture() {
  if (!videoRef.value) {
    return;
  }
  if (stream) {
    closeCapture();
  }
  const camera = cameraList.value[cameraId.value];
  stream = await navigator.mediaDevices.getUserMedia({
    video: { deviceId: camera.deviceId },
  });
  isOpen.value = true;
  videoRef.value.srcObject = stream;
}

function closeCapture() {
  stream?.getTracks().forEach((track) => track.stop());
  stream = null;
  isOpen.value = false;
}
</script>

<template>
  <div class="camera-show">
    <h2>摄像头</h2>
    <video ref="videoRef" width="640" height="480" autoplay></video>
    <div class="ctrl-row">
      <div class="ctrl-left">
        <ElSelect
          v-model="cameraId"
          placeholder="请选择摄像头"
          :disabled="isOpen"
        >
          <ElOption
            v-for="(camera, index) in cameraList"
            :key="camera.deviceId"
            :label="camera.label"
            :value="index"
          ></ElOption>
        </ElSelect>
      </div>
      <div class="ctrl-right">
        <ElButton type="primary" :disabled="isOpen" @click="openCapture"
          >打开</ElButton
        >
        <ElButton type="danger" :disabled="!isOpen" @click="closeCapture"
          >关闭</ElButton
        >
      </div>
    </div>
  </div>
</template>

<style lang="scss" scoped>
.camera-show {
  width: 640px;
  video {
    border: 1px solid #ccc;
  }
  .ctrl-row {
    display: flex;
    justify-content: space-between;
  }
}
</style>
