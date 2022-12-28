import { useEffect, useRef, useState } from "react";
import { createContext, type ReactNode } from "react";

export interface MpuData {
    ypr: [number, number, number];
    accel: [number, number, number];
    temp: number;
    time: number;
}

type MpuContextData = {
    error: Event | null;
    data: MpuData[];
};

export const MpuDataContext = createContext<MpuContextData>({
    error: null,
    data: [],
});

interface MpuDataProviderProps {
    maxElements?: number;
    onOpen?: (e: MessageEvent) => void;
    children: ReactNode;
}

const noop = () => {
    /* noop */
};

export function MpuDataProvider({
    maxElements = 100,
    onOpen = noop,
    children,
}: MpuDataProviderProps) {
    const sseRef = useRef<EventSource | null>(null);

    const [error, setError] = useState<Event | null>(null);
    const [data, setData] = useState<MpuData[]>([]);

    useEffect(() => {
        const sse = new EventSource("/events");
        console.log("SSE created!");

        sseRef.current = sse;
        sse.addEventListener("error", (ev) => {
            setError(ev);
        });

        return () => {
            sse.close();
            sseRef.current = null;
        };
    }, []);

    useEffect(() => {
        // Change our data array if we have too
        if (data.length > maxElements)
            setData((prev) => prev.slice(-maxElements));

        // Update our event listener
        const sse = sseRef.current;
        if (!sse) return;

        const onData = (ev: MessageEvent) => {
            try {
                setData((prev) => [
                    ...prev.slice(-(maxElements - 1)),
                    {
                        ...JSON.parse(ev.data),
                        time: parseInt(ev.lastEventId, 10),
                    },
                ]);
            } catch (e) {
                // Do nothing, as we got invalid data
            }
        };
        sse.addEventListener("mpuData", onData);

        return () => {
            sse.removeEventListener("mpuData", onData);
        };
    }, [maxElements]);

    useEffect(() => {
        const sse = sseRef.current;
        if (!sse) return;

        sse.addEventListener("open", onOpen);
        return () => {
            sse.removeEventListener("open", onOpen);
        };
    }, [onOpen]);

    return (
        <MpuDataContext.Provider value={{ data, error }}>
            {children}
        </MpuDataContext.Provider>
    );
}
