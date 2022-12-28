import { useContext } from "preact/hooks";

import { MpuDataContext } from "../../providers";

interface YawPitchRollProps {}

export function YawPitchRoll({}: YawPitchRollProps) {
    const { data } = useContext(MpuDataContext);
    const [yaw, pitch, roll] = (data && data.ypr) || [0, 0, 0];

    return (
        <p>
            YPR: {yaw.toFixed(2)}, {pitch.toFixed(2)}, {roll.toFixed(2)}
        </p>
    );
}
