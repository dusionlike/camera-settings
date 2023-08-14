<script lang="ts" setup>
import { ref } from "vue";
import { cameraId } from "../store";
import { onMounted } from "vue";
import CameraSettingsItem from "./CameraSettingsItem.vue";
import { ElDialog } from "element-plus";
import { type CameraSettingInfo, CameraSettings } from "camera-settings";
import { onBeforeUnmount } from "vue";

let cs: CameraSettings | null = null;

const videoSettings = ref<CameraSettingInfo[]>([]);
const cameraSettings = ref<CameraSettingInfo[]>([]);

async function init() {
  cs = new CameraSettings(cameraId.value);
  await cs.open();
  const settings = await cs.getSettings();
  videoSettings.value = settings.filter((s) => s.ctrlType === "video");
  cameraSettings.value = settings.filter((s) => s.ctrlType === "camera");
}

onMounted(() => {
  init();
});

onBeforeUnmount(() => {
  cs?.close();
  cs = null;
});

async function changeSetting(settings: CameraSettingInfo[]) {
  try {
    cs?.setSettings(
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
