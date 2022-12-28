import { MpuDataContext } from "@/providers";
import { useContext } from "react";

import { LineChart } from "..";

interface TemperatureProps {}

// eslint-disable-next-line no-empty-pattern
export function Temperature({}: TemperatureProps) {
    const { data } = useContext(MpuDataContext);

    const latestData = data.at(-1);
    const temp = (latestData && latestData.temp) || 0;

    return (
        <div>
            <p>Temperature: {temp.toFixed(2)}°C</p>
            <LineChart
                title="Temperature (°C)"
                height={400}
                xVals={data.map((e) => e.time)}
                yVals={[data.map((e) => e.temp)]}
                series={[
                    {
                        label: "Temperature",
                        stroke: "red",
                    },
                ]}
            />
        </div>
    );
}
