interface TemperatureProps {
    /**
     * The temperature data
     */
    temp: number;
}

export function Temperature({ temp }: TemperatureProps) {
    return <p>Temp: {temp.toFixed(1)}</p>;
}
