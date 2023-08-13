<script lang="ts" setup>
import { useDark } from "@vueuse/core";
import CameraShow from "./components/CameraShow.vue";
import CameraSettings from "./components/CameraSettings.vue";

useDark();
</script>

<template>
  <div class="app">
    <CameraShow />
    <CameraSettings />
  </div>
</template>

<style lang="scss" scoped>
.app {
  display: flex;
  gap: 50px;
}
</style>
