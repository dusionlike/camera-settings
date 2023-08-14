<script lang="ts" setup>
import { ref } from "vue";
import { cameraId, loading, resolutions } from "../store";
import { onMounted } from "vue";
import CameraSettingsItem from "./CameraSettingsItem.vue";
import { ElDialog } from "element-plus";
import { type CameraSettingInfo, CameraSettings, _getCacheCount } from "camera-settings";
import { onBeforeUnmount } from "vue";

let cs: CameraSettings | null = null;

const videoSettings = ref<CameraSettingInfo[]>([]);
const cameraSettings = ref<CameraSettingInfo[]>([]);

async function init() {
  loading.value = true;
  console.log("Cache count:", _getCacheCount());
  cs = new CameraSettings(cameraId.value);
  await cs.open();
  const settings = await cs.getSettings();
  videoSettings.value = settings.filter((s) => s.ctrlType === "video");
  cameraSettings.value = settings.filter((s) => s.ctrlType === "camera");
  resolutions.value = await cs.getResolutions();
  loading.value = false;
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
      v-loading="loading"
      title="视频控制"
      :settings="videoSettings"
      @change="changeSetting"
    />
    <CameraSettingsItem
      v-loading="loading"
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
