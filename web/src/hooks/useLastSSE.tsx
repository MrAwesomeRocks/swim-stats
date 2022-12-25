import { useContext, useEffect, useState } from "preact/hooks";

import { SSEContext } from "../providers";
import { useSSE } from "./useSSE";

export function useLastSSE(event = "message") {
    const [data, setData] = useState<JSON | string | null>(null);
    const [id, setId] = useState<string | null>(null);
    const [error, setError] = useState<Event | null>(null);

    const sse = useContext(SSEContext);

    useEffect(() => {
        const handler = (e: MessageEvent) => {
            let message;
            try {
                message = JSON.parse(e.data);
            } catch {
                message = e.data;
            }

            setData(e.data);
            setId(e.lastEventId);
        };

        sse?.addEventListener(event, handler, false);

        return () => {
            sse?.removeEventListener(event, handler, false);
        };
    }, [event, sse]);

    // Error handler
    useEffect(() => {
        sse?.addEventListener("error", setError, false);

        return () => {
            sse?.removeEventListener("error", setError, false);
        };
    }, [sse]);

    return { data, id, error };
}
