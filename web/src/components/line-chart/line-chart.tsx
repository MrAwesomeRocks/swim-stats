import { useWindowSize } from "@react-hookz/web";
import { isEqual } from "lodash-es";
import { useEffect, useRef, useState } from "react";
import uPlot from "uplot";
import "uplot/dist/uPlot.min.css";

interface LineChartProps {
    /**
     * X-values.
     */
    xVals: number[];

    /**
     * Array of array of y-values.
     *
     * Each array is a line.
     */
    yVals: number[][];

    /**
     * Chart title
     */
    title: string;

    /**
     * Chart width
     */
    maxWidth?: number;

    /**
     * Chart height
     */
    height?: number;

    /**
     * Chart data series
     */
    series?: uPlot.Series[];

    /**
     * Chart data axes
     */
    axes?: uPlot.Axis[];

    /**
     * Chart data scales
     */
    scales?: uPlot.Scales;

    /**
     * Chart legend options
     */
    legend?: uPlot.Legend;

    /**
     * Chart cursor options
     */
    cursor?: uPlot.Cursor;

    /**
     * Whether the x-axis is in ms, s, or raw numbers.
     */
    xAxisType?: "ms" | "s";
}

export function LineChart({
    xVals,
    yVals,
    title = "My Chart",
    maxWidth = 800,
    height = 600,
    series = [],
    axes = [],
    scales = {},
    legend = {},
    cursor = {},
    xAxisType = "s",
}: LineChartProps) {
    const containerRef = useRef<HTMLDivElement | null>(null);
    const plotRef = useRef<uPlot | null>(null);

    const [immutableOptions, setImmutableOptions] = useState({
        title,
        series,
        axes,
        scales,
        legend,
        cursor,
        xAxisType,
    });

    /*
     * Constructor
     */
    useEffect(() => {
        // If we don't have a div we can't create a chart
        if (!containerRef.current) return;

        // If we already have a plot so we should destroy it
        if (plotRef.current) {
            plotRef.current.destroy();
            plotRef.current = null;
            console.log("Plot destroyed");
        }

        // Plot options
        const opts: uPlot.Options = {
            title: immutableOptions.title,
            // TODO(nino): will this cause a layout shift???
            width: 100,
            height: 100,
            series: [{}, ...immutableOptions.series],
            axes: [{}, ...immutableOptions.axes],
            scales: immutableOptions.scales,
            legend: immutableOptions.legend,
            cursor: immutableOptions.cursor,
            ms: immutableOptions.xAxisType == "ms" ? 1e-3 : 1,
        };

        // Create plot
        plotRef.current = new uPlot(
            opts,
            [
                /* xVals, ...yVals */
            ],
            containerRef.current,
        );

        return () => {
            plotRef.current?.destroy();
        };
    }, [immutableOptions]);

    /*
     * Mutators
     */
    // Data
    useEffect(() => {
        if (!plotRef.current) return;
        plotRef.current.setData([xVals, ...yVals]);
    }, [xVals, yVals]);

    // Size
    const { width: winWidth } = useWindowSize();
    const width = winWidth < maxWidth ? winWidth - 40 : maxWidth; // Minimum of the 2, w/ padding

    useEffect(() => {
        if (!plotRef.current) return;
        plotRef.current.setSize({ width, height });
    }, [width, height]);

    useEffect(() => {
        const newOptions = {
            title,
            series,
            axes,
            scales,
            legend,
            cursor,
            xAxisType,
        };
        if (!isEqual(immutableOptions, newOptions)) {
            setImmutableOptions(newOptions);
        }
    }, [
        title,
        series,
        axes,
        scales,
        legend,
        cursor,
        xAxisType,
        immutableOptions,
    ]);

    // The title is 27px and the legend is 31.3667
    return <div ref={containerRef} style={{ height: height + 59, width }} />;
}
