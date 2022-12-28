import { useContext } from "preact/hooks";

import { MpuDataContext } from "@/providers";

interface SSEStatusProps {
    addRefreshButton?: boolean;
}

export function SSEStatus({ addRefreshButton = false }: SSEStatusProps) {
    const { error } = useContext(MpuDataContext);
    return (
        <div style={{ display: "block" }}>
            {error ? (
                <>
                    <p
                        style={{
                            color: "red",
                            display: "inline",
                            marginRight: 10,
                        }}
                    >
                        Disconnected!
                    </p>
                    {addRefreshButton && (
                        <button onClick={() => location.reload()}>
                            Reconnect
                        </button>
                    )}
                </>
            ) : (
                <p style={{ color: "green" }}>Connected!</p>
            )}
        </div>
    );
}
