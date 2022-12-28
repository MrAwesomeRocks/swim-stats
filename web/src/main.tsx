import React from "react";
import ReactDOM from "react-dom/client";

import { App } from "./app";
import "./index.css";

async function prepare() {
    if (import.meta.env.DEV) {
        const { worker } = await import("@mocks/browser");
        return worker.start();
    }
}

prepare().then(() => {
    ReactDOM.createRoot(document.getElementById("app") as HTMLElement).render(
        <React.StrictMode>
            <App />
        </React.StrictMode>,
    );
});
