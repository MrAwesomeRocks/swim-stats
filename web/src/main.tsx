import { render } from "preact";

import { App } from "./app";
import "./index.css";

async function prepare() {
    if (import.meta.env.DEV) {
        const { worker } = await import("@mocks/browser");
        return worker.start();
    }
}

prepare().then(() => {
    render(<App />, document.getElementById("app") as HTMLElement);
});
