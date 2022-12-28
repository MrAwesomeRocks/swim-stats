// import { useEffect, useState } from "preact/hooks";
import {
    Acceleration,
    LineChart,
    SSEStatus,
    Temperature,
    YawPitchRoll,
} from "@/components";
import { MpuDataProvider } from "@/providers";

export function App() {
    /*     useEffect(() => {
        const sse = new EventSource("/events");

        sse.addEventListener("open", () => {
            console.log("Events Connected!");
        });

        sse.addEventListener("error", (e) => {
            if (!(e.target instanceof EventSource)) {
                return;
            }

            if (e.target.readyState != EventSource.OPEN) {
                console.log("Events disconnected!");
            } else {
                console.log("Other error!", e);
            }

            sse.close();
        });

        sse.addEventListener("mpuData", (e) => {
            setData((data) => [
                ...data.slice(-100, data.length),
                { time: parseInt(e.lastEventId, 10), ...JSON.parse(e.data) },
            ]);
        });

        return () => {
            sse.close();
        };
    }, []); */

    return (
        <MpuDataProvider onOpen={() => console.log("SSE connected!")}>
            <h1>{import.meta.env.VITE_APP_TITLE}</h1>

            <SSEStatus addRefreshButton />
            <LineChart
                title="Test Chart"
                xVals={[
                    Date.now(),
                    Date.now() + 1000,
                    Date.now() + 2000,
                    Date.now() + 3000,
                    Date.now() + 4000,
                ]}
                yVals={[[1, 2, 3, 4, 5]]}
                series={[
                    {
                        label: "Test",
                        stroke: "red",
                    },
                ]}
                xAxisType="ms"
            />
            {/*             <LineChart
                title="YPR"
                xVals={data.map((e) => e.time)}
                yVals={data.reduce<number[][]>(
                    (acc, cur) => {
                        acc[0].push(cur.ypr[0]);
                        acc[1].push(cur.ypr[1]);
                        acc[2].push(cur.ypr[2]);
                        return acc;
                    },
                    [[], [], []],
                )}
                series={[
                    {
                        label: "Yaw",
                        stroke: "red",
                    },
                    {
                        label: "Pitch",
                        stroke: "blue",
                    },
                    {
                        label: "Roll",
                        stroke: "green",
                    },
                ]}
            />
 */}
            <YawPitchRoll />
            {/* <Acceleration
                x={curData.accel[0]}
                y={curData.accel[1]}
                z={curData.accel[2]}
            />
            <Temperature temp={curData.temp} /> */}
        </MpuDataProvider>
    );
}
