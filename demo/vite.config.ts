import { defineConfig } from "vite";
import vue from "@vitejs/plugin-vue";
import Components from "unplugin-vue-components/vite";
import { ElementPlusResolver } from "unplugin-vue-components/resolvers";

import commonjsExternals from "vite-plugin-commonjs-externals";
import builtinModules from "builtin-modules";

const externals = [
  ...builtinModules,
  ...builtinModules.map((m) => `node:${m}`),
  "electron",
  "camera-settings",
];

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [
    vue(),
    Components({
      resolvers: [ElementPlusResolver()],
      dts: "src/components.d.ts",
    }),
    commonjsExternals({
      externals,
      exts: ["ts", "tsx", "js", "jsx", "mjs", "cjs", "vue"],
    }),
  ],
});
