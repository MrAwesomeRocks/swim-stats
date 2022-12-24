import { useState, useEffect } from "preact/hooks";
import preactLogo from "./assets/preact.svg";
import { Temperature, Acceleration, YawPitchRoll } from "./components";

export function App() {
  const [connected, setConnected] = useState(false);
  const [data, setData] = useState({
    ypr: [0, 0, 0],
    accel: [0, 0, 0],
    temp: 0,
  });

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
      setData(JSON.parse(e.data));
    });

    return () => {
      sse.close();
    };
  }, [refresh]);

  return (
    <>
      <h1>{import.meta.env.VITE_APP_TITLE}</h1>

      <div>
        {connected ? (
          <p style={{ color: "green" }}>Connected!</p>
        ) : (
          <>
            <p style={{ color: "red", display: "inline", marginRight: 10 }}>
              Disconnected!
            </p>
            <button onClick={() => setRefresh((refresh) => !refresh)}>
              Reconnect
            </button>
          </>
        )}
      </div>

      <YawPitchRoll yaw={data.ypr[0]} pitch={data.ypr[1]} roll={data.ypr[2]} />
      <Acceleration x={data.accel[0]} y={data.accel[1]} z={data.accel[2]} />
      <Temperature temp={data.temp} />
    </>
  );
}
