import { defineConfig } from "vite";
import vue from "@vitejs/plugin-vue";
import tailwindcss from "@tailwindcss/vite";
import path from "path";

export default defineConfig(async () => {
  const plugins = [vue(), tailwindcss()];

  if (process.env.BUNDLE_ANALYZE === "1") {
    const { visualizer } = await import("rollup-plugin-visualizer");
    plugins.push(
      visualizer({
        filename: "dist/bundle-report.html",
        template: "treemap",
        gzipSize: true,
        brotliSize: true,
      }),
    );
  }

  return {
    base: "./",
    plugins,
    resolve: {
      alias: {
        "@": path.resolve(__dirname, "src"),
        moment: "moment/src/moment",
      },
    },
    build: {
      outDir: "dist",
      assetsDir: "assets",
      cssCodeSplit: false,
      sourcemap: false,
      rollupOptions: {
        output: {
          entryFileNames: "assets/app.js",
          chunkFileNames: "assets/[name].js",
          assetFileNames: (assetInfo) => {
            const parsedName = path.parse(assetInfo.name || "asset");
            return `assets/${parsedName.name}[extname]`;
          },
          manualChunks: undefined,
        },
      },
    },
  };
});
