import {
    Acceleration,
    SSEStatus,
    Temperature,
    YawPitchRoll,
} from "@/components";
import { MpuDataProvider } from "@/providers";
import { useState } from "react";

export function App() {
    const [dataBufSize, setDataBufSize] = useState<number>(100);

    return (
        <MpuDataProvider
            onOpen={() => console.log("SSE connected!")}
            maxElements={dataBufSize}
        >
            <h1>{import.meta.env.VITE_APP_TITLE}</h1>

            <SSEStatus addRefreshButton />
            <label>
                MPU Data buffer size:{" "}
                <input
                    type="number"
                    value={dataBufSize}
                    onChange={(e) => {
                        const val = parseInt(e.target.value, 10);
                        if (val >= 2) setDataBufSize(val);
                    }}
                />
            </label>

            <YawPitchRoll />
            <Acceleration />
            <Temperature />
        </MpuDataProvider>
    );
}
