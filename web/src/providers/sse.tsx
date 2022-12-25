/**
 * Adapted from
 * https://github.com/mfrachet/server-push-hooks/blob/master/packages/use-server-sent-events/
 *
 * ZLib License
 */
import { type ComponentChildren, createContext } from "preact";
import { useEffect, useRef } from "preact/hooks";

export const SSEContext = createContext<EventSource | null>(null);

interface SSEProviderProps {
    url: string | URL;
    opts?: EventSourceInit;
    onOpen?: (ev: Event) => void;
    children: ComponentChildren;
}

export function SSEProvider({
    url,
    opts = {},
    onOpen = () => {
        /* noop */
    },
    children,
}: SSEProviderProps) {
    const eventSourceRef = useRef<EventSource | null>(null);

    useEffect(() => {
        const sse = new EventSource(url, opts);
        eventSourceRef.current = sse;

        return () => {
            sse.close();
        };
    }, [url, opts]);

    useEffect(() => {
        if (!eventSourceRef.current) return;
        eventSourceRef.current.onopen = onOpen;
    }, [onOpen]);

    if (!window) return <>{children}</>;

    return (
        <SSEContext.Provider value={eventSourceRef.current}>
            {children}
        </SSEContext.Provider>
    );
}
