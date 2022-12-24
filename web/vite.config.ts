import preact from "@preact/preset-vite";
import { createHtmlPlugin } from "vite-plugin-html";
import checker from "vite-plugin-checker";
import { compression } from "vite-plugin-compression2";
import svg from "vite-plugin-svgo";
import viteImagemin from "vite-plugin-imagemin";

import { defineConfig, loadEnv } from "vite";
import type { UserConfigExport } from "vite";

// https://vitejs.dev/config/
export default defineConfig(({ mode }) => {
  const env = loadEnv(mode, process.cwd());

  const config: UserConfigExport = {
    plugins: [
      preact(),
      checker({
        typescript: true,
        eslint: /* TODO */ false,
        overlay: true,
        enableBuild: true,
      }),
      svg({
        multipass: true,
        plugins: [
          {
            name: "preset-default",
          },
        ],
      }),
      viteImagemin({
        gifsicle: {
          optimizationLevel: 3,
          interlaced: false,
        },
        optipng: {
          optimizationLevel: 7,
        },
        mozjpeg: {
          quality: 20,
          dcScanOpt: 2,
        },
        pngquant: {
          quality: [0.8, 0.9],
          speed: 2,
          strip: true,
        },
        webp: {
          method: 6,
          quality: 70,
        },
        svgo: false, // vite-plugin-svgo
      }),
      // HTML transform plugin to replace %VAR% with the variable's value
      {
        name: "html-transform",
        transformIndexHtml(html) {
          return html.replace(/%(.*?)%/g, (_match: any, p1: string) => {
            return env[p1];
          });
        },
      },
    ],
    build: {
      rollupOptions: {
        output: {
          // Single file for every NPM package
          // TODO: make sure this doesn't take up extra space
          manualChunks(id) {
            if (id.includes("node_modules")) {
              const packageNameMatch = id.match(
                /node_modules[\\/]\.pnpm[\\/]@?([\w+-.]+)@[\d.]+/
              );
              const packageName = packageNameMatch[1]
                .replace("@", "")
                .replace("+", "-")
                .replace(".", "-");
              return `vendor-npm.${packageName}`;
            }
          },
        },
      },
    },
  };

  if (mode !== "development") {
    config.plugins = [
      ...config.plugins,
      createHtmlPlugin({ minify: true }),
      compression({ algorithm: "gzip", deleteOriginalAssets: true }),
    ];
  }

  return config;
});
