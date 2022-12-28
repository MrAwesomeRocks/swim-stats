import { useEffect, useRef, useState } from "react";
import { createContext, type ReactNode } from "react";

export interface MpuData {
    ypr: [number, number, number];
    accel: [number, number, number];
    temp: number;
}

type MpuContextData = {
    error: Event | null;
    data: MpuData | null;
    id: number;
};

export const MpuDataContext = createContext<MpuContextData>({
    error: null,
    data: null,
    id: 0,
});

interface MpuDataProviderProps {
    onOpen?: (e: MessageEvent) => void;
    children: ReactNode;
}

export function MpuDataProvider({
    onOpen = () => {
        /* no-op */
    },
    children,
}: MpuDataProviderProps) {
    const sseRef = useRef<EventSource | null>(null);

    const [error, setError] = useState<Event | null>(null);
    const [data, setData] = useState<MpuData | null>(null);
    const [id, setId] = useState<number>(0);

    useEffect(() => {
        const sse = new EventSource("/events");
        console.log("SSE created!");

        sse.addEventListener("error", (ev) => {
            setError(ev);
        });

        sse.addEventListener("mpuData", (ev) => {
            try {
                setData(JSON.parse(ev.data));
            } catch (e) {
                setData(null);
            }
            setId(parseInt(ev.lastEventId, 10));
        });

        sseRef.current = sse;

        return () => {
            sse.close();
            sseRef.current = null;
        };
    }, []);

    useEffect(() => {
        const sse = sseRef.current;
        if (!sse) return;

        sse.addEventListener("open", onOpen);
        return () => {
            sse.removeEventListener("open", onOpen);
        };
    }, [onOpen]);

    return (
        <MpuDataContext.Provider value={{ data, error, id }}>
            {children}
        </MpuDataContext.Provider>
    );
}
