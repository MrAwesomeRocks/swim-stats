import react from "@vitejs/plugin-react-swc";
import { defineConfig, loadEnv, type UserConfigExport } from "vite";
import { ViteAliases } from "vite-aliases";
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
            react({}),
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
            // eslint-disable-next-line new-cap
            ViteAliases({
                dir: "src",
                prefix: "@",
                deep: true,
                depth: 1,
                createGlobalAlias: true,
                adjustDuplicates: false,
                useAbsolute: false,
                useIndexes: false,
                useConfig: true,
                useTypescript: true,
                createLog: true,
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
