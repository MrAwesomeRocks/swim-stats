import { useContext, useEffect, useState } from "preact/hooks";

import { SSEContext } from "../providers";

export function useSSE(
    listeners: {
        event: string;
        onMessage: (data: JSON | string, id: string) => void;
    }[],
) {
    const [error, setError] = useState<Event | null>(null);
    const sse = useContext(SSEContext);

    // Error handler
    useEffect(() => {
        sse?.addEventListener("error", setError, false);

        return () => {
            sse?.removeEventListener("error", setError, false);
        };
    }, [sse]);

    // Event handlers
    useEffect(() => {
        if (!sse) return;

        // Keep track of handlers so we can clean them up
        const handlers: { [key: string]: (e: MessageEvent) => void } = {};

        // Add our event handlers
        for (const { event, onMessage } of listeners) {
            const handler = (e: MessageEvent) => {
                let message;
                try {
                    message = JSON.parse(e.data);
                } catch {
                    message = e.data;
                }

                onMessage(message, e.lastEventId);
            };

            sse.addEventListener(event, handler, false);
            handlers[event] = handler;
        }

        // Remove our event handlers
        return () => {
            for (const { event } of listeners) {
                sse.removeEventListener(event, handlers[event], false);
            }
        };
    }, [listeners, sse]);

    return error;
}
