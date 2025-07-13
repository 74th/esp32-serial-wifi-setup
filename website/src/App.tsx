import { useState, useRef, useEffect } from "react";

/**
 * Simple Web Serial terminal built with React + Vite.
 * - Connect / disconnect to any serial device the user selects
 * - Stream incoming bytes into a scroll‑locking monitor pane
 * - Send a single line when ⏎ or Send is pressed
 *
 * Uses plain HTML elements + Tailwind classes (no external UI library required).
 */
export default function App() {
  // --- state --------------------------------------------------------------
  const [port, setPort] = useState<any>(null);
  const [writer, setWriter] = useState<WritableStreamDefaultWriter | null>(
    null
  );
  const [connected, setConnected] = useState(false);

  const [input, setInput] = useState("");
  const [log, setLog] = useState<string[]>([]);
  const [ip, setIp] = useState<string | null>(null); // IPアドレス用state追加
  const [macAddress, setMacAddress] = useState<string | null>(null); // MACアドレス用state追加
  const [ssid, setSsid] = useState(""); // SSID入力用state
  const [wifiPass, setWifiPass] = useState(""); // パスワード入力用state
  const logRef = useRef<HTMLDivElement>(null);

  // --- helpers ------------------------------------------------------------
  const appendLog = (line: string) =>
    setLog((prev) => [...prev, line].slice(-500)); // keep last 500 lines

  const scrollToBottom = () => {
    const node = logRef.current;
    if (node) node.scrollTop = node.scrollHeight;
  };
  useEffect(scrollToBottom, [log]);

  // --- connect / disconnect ----------------------------------------------
  const connect = async () => {
    try {
      // Ask the user to choose a serial port.
      const selectedPort = await (navigator as any).serial.requestPort();
      await selectedPort.open({ baudRate: 115200 });
      setPort(selectedPort);
      setConnected(true);
      appendLog("[system] connected…");

      // start read loop
      const reader = selectedPort.readable?.getReader();
      const decoder = new TextDecoder();
      (async () => {
        try {
          let buffer = "";
          while (true) {
            const { value, done } = await reader!.read();
            if (done) break;
            if (value) {
              const text = decoder.decode(value);
              buffer += text;
              let idx;
              while ((idx = buffer.indexOf("\n")) !== -1) {
                const line = buffer.slice(0, idx).trim();
                buffer = buffer.slice(idx + 1);
                if (line) {
                  // JSONレスポンスのパース
                  try {
                    const obj = JSON.parse(line);
                    if (
                      obj &&
                      obj.jsonrpc === "2.0" &&
                      obj.id === 1 &&
                      obj.result
                    ) {
                      if (obj.result.ip !== undefined) {
                        setIp(obj.result.ip);
                        appendLog(`[ip] ${obj.result.ip}`);
                      } else if (obj.result.mac_address !== undefined) {
                        setMacAddress(obj.result.mac_address);
                        appendLog(`[mac] ${obj.result.mac_address}`);
                      } else {
                        appendLog(line);
                      }
                    } else {
                      appendLog(line);
                    }
                  } catch {
                    appendLog(line);
                  }
                }
              }
            }
          }
        } catch (err) {
          appendLog(`[error] ${err}`);
        }
      })();

      // set writer for send()
      const w = selectedPort.writable?.getWriter();
      setWriter(w ?? null);
    } catch (err) {
      appendLog(`[error] ${err}`);
    }
  };

  const disconnect = async () => {
    try {
      await writer?.close();
      await port?.close();
    } catch {
      /* ignore */
    } finally {
      setWriter(null);
      setPort(null);
      setConnected(false);
      appendLog("[system] disconnected");
    }
  };

  // --- send a line --------------------------------------------------------
  const send = async () => {
    if (!writer) return;
    try {
      const data = new TextEncoder().encode(input + "\r\n");
      await writer.write(data);
      appendLog(`> ${input}`);
      setInput("");
    } catch (err) {
      appendLog(`[error] ${err}`);
    }
  };

  // IPアドレス取得リクエスト
  const getIp = async () => {
    if (!writer) return;
    try {
      const req = { jsonrpc: "2.0", method: "get_ip", id: 1 };
      const data = new TextEncoder().encode(JSON.stringify(req) + "\r\n");
      await writer.write(data);
      appendLog("> get_ip request sent");
    } catch (err) {
      appendLog(`[error] ${err}`);
    }
  };

  // MACアドレス取得リクエスト
  const getMacAddress = async () => {
    if (!writer) return;
    try {
      const req = { jsonrpc: "2.0", method: "get_mac_address", id: 1 };
      const data = new TextEncoder().encode(JSON.stringify(req) + "\r\n");
      await writer.write(data);
      appendLog("> get_mac_address request sent");
    } catch (err) {
      appendLog(`[error] ${err}`);
    }
  };

  // WiFi設定リクエスト
  const setWifiCreds = async () => {
    if (!writer) return;
    try {
      const req = {
        jsonrpc: "2.0",
        method: "set_wifi_creds",
        params: { ssid, pass: wifiPass },
        id: 1,
      };
      const data = new TextEncoder().encode(JSON.stringify(req) + "\r\n");
      await writer.write(data);
      appendLog("> set_wifi_creds request sent");
    } catch (err) {
      appendLog(`[error] ${err}`);
    }
  };

  // --- ui -----------------------------------------------------------------
  return (
    <div className="min-h-screen bg-gray-50 dark:bg-gray-900">
      <div className="container mx-auto px-6 py-8 max-w-4xl">
        <div className="flex flex-col items-center space-y-6">
          <h1 className="text-3xl font-bold text-gray-900 dark:text-white">Web Serial WiFi Setup</h1>

          <div className="space-x-2">
            <button
              onClick={connected ? disconnect : connect}
              className="px-4 py-2 rounded-lg shadow text-white bg-blue-600 disabled:opacity-50"
            >
              {connected ? "Disconnect" : "Connect"}
            </button>
            <button
              onClick={getIp}
              disabled={!connected}
              className="px-4 py-2 rounded-lg shadow text-white bg-indigo-600 disabled:opacity-50"
            >
              Get IP
            </button>
            <button
              onClick={getMacAddress}
              disabled={!connected}
              className="px-4 py-2 rounded-lg shadow text-white bg-purple-600 disabled:opacity-50"
            >
              Get MAC Address
            </button>
          </div>
          <div className="w-full max-w-2xl text-left text-base text-gray-900 dark:text-gray-100 bg-gray-100 dark:bg-gray-800 rounded p-2">
            <span className="font-bold">IP Address: </span>
            <span>{ip ?? "-"}</span>
          </div>
          <div className="w-full max-w-2xl text-left text-base text-gray-900 dark:text-gray-100 bg-gray-100 dark:bg-gray-800 rounded p-2">
            <span className="font-bold">MAC Address: </span>
            <span>{macAddress ?? "-"}</span>
          </div>

          <div className="w-full max-w-2xl flex space-x-2 items-center">
            <input
              className="flex-grow border rounded-lg p-2 bg-gray-800 text-white placeholder-gray-500 focus:outline-none"
              type="text"
              placeholder="SSID"
              value={ssid}
              onChange={(e) => setSsid(e.target.value)}
              disabled={!connected}
            />
            <input
              className="flex-grow border rounded-lg p-2 bg-gray-800 text-white placeholder-gray-500 focus:outline-none"
              type="password"
              placeholder="Password"
              value={wifiPass}
              onChange={(e) => setWifiPass(e.target.value)}
              disabled={!connected}
            />
            <button
              onClick={setWifiCreds}
              disabled={!connected || !ssid}
              className="px-4 py-2 rounded-lg shadow text-white bg-orange-600 disabled:opacity-50"
            >
              Set WiFi
            </button>
          </div>
          <div
            ref={logRef}
            className="w-full max-w-2xl h-64 overflow-y-auto rounded-lg bg-gray-900 text-green-400 font-mono text-sm p-3 shadow-inner"
          >
            {log.map((l, i) => (
              <div key={i} className="whitespace-pre-wrap break-words">
                {l}
              </div>
            ))}
          </div>

          <div className="w-full max-w-2xl flex space-x-2">
            <input
              className="flex-grow border rounded-lg p-2 bg-gray-800 text-white placeholder-gray-500 focus:outline-none"
              type="text"
              placeholder="Type a command…"
              value={input}
              onChange={(e) => setInput(e.target.value)}
              onKeyDown={(e) => {
                if (e.key === "Enter") {
                  e.preventDefault();
                  send();
                }
              }}
              disabled={!connected}
            />
            <button
              onClick={send}
              disabled={!connected || !input.trim()}
              className="px-4 py-2 rounded-lg shadow text-white bg-green-600 disabled:opacity-50"
            >
              Send
            </button>
          </div>
        </div>
      </div>
    </div>
  );
}
