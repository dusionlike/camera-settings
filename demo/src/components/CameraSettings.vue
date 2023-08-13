<script lang="ts" setup>
import { ref } from "vue";
import { cameraId } from "../store";
import { onMounted } from "vue";
import CameraSettingsItem from "./CameraSettingsItem.vue";
import { ElDialog } from "element-plus";
import {
  type CameraSetting,
  getCameraSettings,
  setCameraSettings,
} from "camera-settings";

const videoSettings = ref<CameraSetting[]>([]);
const cameraSettings = ref<CameraSetting[]>([]);

async function init() {
  const settings = await getCameraSettings(cameraId.value);
  videoSettings.value = settings.filter((s) => s.ctrlType === "video");
  cameraSettings.value = settings.filter((s) => s.ctrlType === "camera");
}

onMounted(() => {
  init();
});

async function changeSetting(settings: CameraSetting[]) {
  try {
    await setCameraSettings(
      cameraId.value,
      settings.map((setting) => ({
        prop: setting.prop,
        val: setting.val,
        isAuto: setting.isAuto,
      }))
    );
  } catch (error: any) {
    ElDialog.alert({
      title: "设置失败",
      message: error.message,
    });
  }
}
</script>

<template>
  <div class="camera-settings">
    <CameraSettingsItem
      title="视频控制"
      :settings="videoSettings"
      @change="changeSetting"
    />
    <CameraSettingsItem
      title="相机控制"
      :settings="cameraSettings"
      @change="changeSetting"
    />
  </div>
</template>

<style lang="scss" scoped>
.camera-settings {
  display: flex;
  gap: 30px;
}
</style>
