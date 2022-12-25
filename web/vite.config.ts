import preact from "@preact/preset-vite";
import { defineConfig, loadEnv, type UserConfigExport } from "vite";
import checker from "vite-plugin-checker";
import { compression } from "vite-plugin-compression2";
import { createHtmlPlugin } from "vite-plugin-html";
import viteImagemin from "vite-plugin-imagemin";
import svg from "vite-plugin-svgo";

import pkg from "./package.json";

// https://vitejs.dev/config/
export default defineConfig(({ mode }) => {
    const env = loadEnv(mode, process.cwd());

    const config: UserConfigExport = {
        plugins: [
            preact(),
            checker({
                typescript: true,
                eslint: {
                    lintCommand: pkg.scripts.lint,
                },
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
                    return html.replace(
                        /%(.*?)%/g,
                        (_match: unknown, p1: string) => {
                            if (env[p1] !== undefined) return env[p1];
                            return pkg[p1.replace("VITE_", "").toLowerCase()];
                        },
                    );
                },
            },
        ],
        resolve: {
            // https://preactjs.com/guide/v10/getting-started#aliasing-in-rollup
            alias: [
                { find: "react", replacement: "preact/compat" },
                {
                    find: "react-dom/test-utils",
                    replacement: "preact/test-utils",
                },
                { find: "react-dom", replacement: "preact/compat" },
                {
                    find: "react/jsx-runtime",
                    replacement: "preact/jsx-runtime",
                },
            ],
        },
    };

    if (mode !== "development") {
        config.plugins = [
            ...config.plugins,
            createHtmlPlugin({ minify: true }),
            compression({
                algorithm: "gzip",
                deleteOriginalAssets: true,
                threshold: 10,
            }),
        ];
    }

    return config;
});
