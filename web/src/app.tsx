import { useState, useEffect } from "preact/hooks";
import preactLogo from "./assets/preact.svg";

export function App() {
  const [connected, setConnected] = useState(false);
  const [data, setData] = useState({
    ypr: [0, 0, 0],
    accel: [0, 0, 0],
    temp: 0,
  });

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
      setData(JSON.parse(e.data));
    });

    return () => {
      sse.close();
    };
  }, []);

  return (
    <>
      <h1>Swim Stats Tracker</h1>

      {connected ? (
        <p style={{ color: "green" }}>Connected!</p>
      ) : (
        <p style={{ color: "red" }}>Disconnected!</p>
      )}

      <p>
        YPR: {data.ypr[0]}, {data.ypr[1]}, {data.ypr[2]}
      </p>
      <p>
        Accel: {data.accel[0]}, {data.accel[1]}, {data.accel[2]}
      </p>
      <p>Temp: {data.temp}</p>
    </>
  );
}
