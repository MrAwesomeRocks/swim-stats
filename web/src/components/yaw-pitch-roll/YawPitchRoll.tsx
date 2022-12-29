import { MpuDataContext } from "@/providers";
import { useContext } from "react";

import { LineChart } from "..";

interface YawPitchRollProps {}

// eslint-disable-next-line no-empty-pattern
export function YawPitchRoll({}: YawPitchRollProps) {
    const { data } = useContext(MpuDataContext);

    const latestData = data.at(-1);
    const [yaw, pitch, roll] = (latestData && latestData.ypr) || [0, 0, 0];

    return (
        <div>
            <p>
                YPR: {yaw.toFixed(2)}, {pitch.toFixed(2)}, {roll.toFixed(2)} (°)
            </p>
            <LineChart
                title="Yaw, Pitch, and Roll"
                height={400}
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
                scales={{
                    x: {
                        time: false,
                    },
                }}
            />
        </div>
    );
}
