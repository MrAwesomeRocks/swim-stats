interface YawPitchRollProps {
    /**
     * Yaw angle.
     */
    yaw: number;

    /**
     * Pitch angle.
     */
    pitch: number;

    /**
     * Roll angle.
     */
    roll: number;
}

export function YawPitchRoll({ yaw, pitch, roll }: YawPitchRollProps) {
    return (
        <p>
            YPR: {yaw.toFixed(2)}, {pitch.toFixed(2)}, {roll.toFixed(2)}
        </p>
    );
}
