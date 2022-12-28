import { MpuDataContext } from "@/providers";
import { useContext } from "react";

interface YawPitchRollProps {}

// eslint-disable-next-line no-empty-pattern
export function YawPitchRoll({}: YawPitchRollProps) {
    const { data } = useContext(MpuDataContext);
    const [yaw, pitch, roll] = (data && data.ypr) || [0, 0, 0];

    return (
        <p>
            YPR: {yaw.toFixed(2)}, {pitch.toFixed(2)}, {roll.toFixed(2)}
        </p>
    );
}
