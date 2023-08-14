import { ref } from "vue";
import { type Resolution } from "camera-settings";

export const cameraId = ref(0);

export const loading = ref(false);

export const resolutions = ref<Resolution[]>([]);
