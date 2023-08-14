<script lang="ts" setup>
import type { CameraSettingInfo } from "camera-settings";
import { settingNameMap } from "../utils";

const props = defineProps<{
  title: string;
  settings: CameraSettingInfo[];
}>();

const emit = defineEmits<{
  (event: "change", val: CameraSettingInfo[]): void;
}>();

function changeToDef() {
  const shuldChangeList: CameraSettingInfo[] = [];
  for (const setting of props.settings) {
    if (setting.val !== setting.def) {
      setting.val = setting.def;
      shuldChangeList.push(setting);
    }
  }
  emit("change", shuldChangeList);
}
</script>

<template>
  <div class="camera-settings-item">
    <div class="settings-ctrl">
      <h2>
        {{ title }} &nbsp;&nbsp;&nbsp;
        <ElButton type="warning" @click="changeToDef()">恢复默认</ElButton>
      </h2>
      <ElForm>
        <ElFormItem
          v-for="setting in settings"
          :key="setting.prop"
          :label="settingNameMap[setting.prop]"
          label-width="70px"
        >
          <ElSlider
            v-model="setting.val"
            :min="setting.min"
            :max="setting.max"
            :step="setting.step"
            :show-input="true"
            :disabled="setting.isAuto"
            @change="$emit('change', [setting])"
          />
        </ElFormItem>
      </ElForm>
    </div>
    <div>
      <h2>自动</h2>
      <ElForm>
        <ElFormItem
          v-for="setting in settings"
          :key="setting.prop"
          label="&nbsp;"
        >
          <ElSwitch
            v-model="setting.isAuto"
            v-if="setting.rangeFlags === 3"
            @change="$emit('change', [setting])"
          />
        </ElFormItem>
      </ElForm>
    </div>
  </div>
</template>

<style lang="scss" scoped>
.camera-settings-item {
  display: flex;
  gap: 10px;
  .settings-ctrl {
    width: 500px;
  }
}
</style>
