import { useEffect, useState } from "preact/hooks";

import {
    Acceleration,
    LineChart,
    Temperature,
    YawPitchRoll,
} from "./components";

export function App() {
    const [connected, setConnected] = useState(false);
    const [data, setData] = useState<
        {
            time: number;
            ypr: number[];
            accel: number[];
            temp: number;
        }[]
    >([]);

    const [refresh, setRefresh] = useState(false);

    useEffect(() => {
        const sse = new EventSource("/events");

        sse.addEventListener("open", () => {
            console.log("Events Connected!");
            setConnected(true);
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

            setConnected(false);
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
    }, [refresh]);

    const curData = data[data.length - 1] || {
        time: 0,
        ypr: [0, 0, 0],
        accel: [0, 0, 0],
        temp: 0,
    };

    return (
        <>
            <h1>{import.meta.env.VITE_APP_TITLE}</h1>

            <div>
                {connected ? (
                    <p style={{ color: "green" }}>Connected!</p>
                ) : (
                    <>
                        <p
                            style={{
                                color: "red",
                                display: "inline",
                                marginRight: 10,
                            }}
                        >
                            Disconnected!
                        </p>
                        <button
                            onClick={() => setRefresh((refresh) => !refresh)}
                        >
                            Reconnect
                        </button>
                    </>
                )}
            </div>

            <LineChart
                title="Test Chart"
                xVals={[
                    Date.now(),
                    Date.now() + 1,
                    Date.now() + 2,
                    Date.now() + 3,
                    Date.now() + 4,
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
            <LineChart
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

            <YawPitchRoll
                yaw={curData.ypr[0]}
                pitch={curData.ypr[1]}
                roll={curData.ypr[2]}
            />
            <Acceleration
                x={curData.accel[0]}
                y={curData.accel[1]}
                z={curData.accel[2]}
            />
            <Temperature temp={curData.temp} />
        </>
    );
}
