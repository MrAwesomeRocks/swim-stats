interface AccelerationProps {
    /**
     * x-axis acceleration.
     */
    x: number;

    /**
     * y-axis acceleration.
     */
    y: number;

    /**
     * z-axis acceleration.
     */
    z: number;
}

export function Acceleration({ x, y, z }: AccelerationProps) {
    return (
        <p>
            Accel: {x.toFixed(2)}, {y.toFixed(2)}, {z.toFixed(2)}
        </p>
    );
}
