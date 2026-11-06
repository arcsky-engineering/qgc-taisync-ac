#!/usr/bin/env python3
"""Test NTRIP caster for exercising QGC's NTRIP client.

Replays RTCM3 frames from a .gpsbase file (raw RTCM3) at ~1 epoch per second,
using one of several caster behaviours. Accepts clients in a loop so QGC's
reconnect/backoff can be observed. Standard library only.

Modes:
  icy          ICY 200 OK + raw RTCM (NTRIP v1, like the RoboDot caster)
  http         HTTP/1.1 200 OK + raw RTCM
  http10       HTTP/1.0 200 OK + raw RTCM
  chunked      HTTP/1.1 200 OK, Transfer-Encoding: chunked RTCM
  sourcetable  SOURCETABLE 200 OK + table, then close
  badpass      ERROR - Bad Password, then close
  http401      HTTP/1.1 401 Unauthorized, then close
  http404      HTTP/1.1 404 Not Found, then close
  silent       accept and read the request, never answer (handshake timeout)

Options:
  --stop-after N   stop sending data after N s but keep the socket open (heartbeat test)
  --close-after N  close the connection after N s
  --separate       send the response header in its own TCP segment
                   (default: header and first RTCM epoch in one segment)

Example:
  python tools/ntrip_test_caster.py icy --file "...\\2026-10-20 12-48-08.gpsbase"
Then point QGC at 127.0.0.1:2101, any mount point/user/password.
"""
import argparse
import socket
import time
from collections import Counter


def load_frames(path):
    data = open(path, "rb").read()
    frames, i = [], 0
    while i + 3 <= len(data):
        if data[i] != 0xD3:
            i += 1
            continue
        n = ((data[i + 1] & 3) << 8) | data[i + 2]
        frames.append(data[i:i + n + 6])
        i += n + 6
    return frames


def msg_id(frame):
    return (frame[3] << 4) | (frame[4] >> 4)


def epochs(frames):
    """Group frames into ~1 s epochs: a new epoch starts at each 1074 (GPS MSM4)."""
    out, cur = [], []
    for f in frames:
        if msg_id(f) == 1074 and cur:
            out.append(cur)
            cur = []
        cur.append(f)
    if cur:
        out.append(cur)
    return out


def chunk(data):
    return b"%x\r\n" % len(data) + data + b"\r\n"


def log(msg):
    print(time.strftime("%H:%M:%S ") + msg, flush=True)


def read_request(c):
    c.settimeout(5)
    buf = b""
    while b"\r\n\r\n" not in buf:
        part = c.recv(4096)
        if not part:
            break
        buf += part
    for line in buf.decode(errors="replace").split("\r\n"):
        if line:
            log("  RX " + ("Authorization: <redacted>" if line.lower().startswith("authorization") else line))
    c.settimeout(None)


def serve(c, args, eps):
    read_request(c)
    m = args.mode
    rejects = {
        "sourcetable": b"SOURCETABLE 200 OK\r\nServer: test\r\nContent-Type: text/plain\r\n\r\n"
                       b"STR;RTCM3_MSM4;test;RTCM 3.2;;2;GPS;SNIP;USA;30.25;-98.01;0;0;sNTRIP;none;N;N;0;\r\nENDSOURCETABLE\r\n",
        "badpass": b"ERROR - Bad Password\r\n",
        "http401": b"HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"/RTCM3_MSM4\"\r\nContent-Length: 0\r\n\r\n",
        "http404": b"HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n",
    }
    if m in rejects:
        c.sendall(rejects[m])
        log("  sent reject, closing")
        time.sleep(0.5)
        return
    if m == "silent":
        log("  not answering")
        time.sleep(60)
        return

    header = {
        "icy": b"ICY 200 OK\r\n",
        "http": b"HTTP/1.1 200 OK\r\nServer: test\r\nContent-Type: gnss/data\r\nCache-Control: no-store\r\n\r\n",
        "http10": b"HTTP/1.0 200 OK\r\nServer: test\r\nContent-Type: gnss/data\r\n\r\n",
        "chunked": b"HTTP/1.1 200 OK\r\nServer: test\r\nContent-Type: gnss/data\r\nTransfer-Encoding: chunked\r\n\r\n",
    }[m]
    if args.separate:
        c.sendall(header)
        header = b""
        time.sleep(0.2)

    sent = Counter()
    t0 = time.time()
    k = 0
    stalled = False
    while True:
        el = time.time() - t0
        if args.close_after and el >= args.close_after:
            log("  close-after reached, closing")
            break
        if args.stop_after and el >= args.stop_after:
            if not stalled:
                log("  stop-after reached, socket stays open")
                stalled = True
            time.sleep(0.5)
            continue
        ep = eps[k % len(eps)]
        k += 1
        body = b"".join(ep)
        if m == "chunked":
            # One chunk per frame, with the epoch split across chunk boundaries mid-frame too
            mid = len(body) // 2
            body = chunk(body[:mid]) + chunk(body[mid:])
        c.sendall(header + body)
        header = b""
        sent.update(msg_id(f) for f in ep)
        if k % 10 == 0:
            log("  sent so far: " + ", ".join(f"{i}x{n}" for i, n in sorted(sent.items())))
        time.sleep(1)
    log("  total sent: " + ", ".join(f"{i}x{n}" for i, n in sorted(sent.items())))


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("mode", choices=["icy", "http", "http10", "chunked", "sourcetable", "badpass",
                                    "http401", "http404", "silent"])
    p.add_argument("--file", required=True, help=".gpsbase / raw RTCM3 file to replay")
    p.add_argument("--host", default="0.0.0.0")
    p.add_argument("--port", type=int, default=2101)
    p.add_argument("--stop-after", type=float, default=0)
    p.add_argument("--close-after", type=float, default=0)
    p.add_argument("--separate", action="store_true")
    args = p.parse_args()

    eps = epochs(load_frames(args.file))
    log(f"{len(eps)} epochs loaded, mode {args.mode}, listening on {args.host}:{args.port}")

    srv = socket.socket()
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind((args.host, args.port))
    srv.listen(1)
    while True:
        c, addr = srv.accept()
        log(f"client {addr[0]}:{addr[1]} connected")
        try:
            serve(c, args, eps)
        except OSError as e:
            log(f"  client gone: {e}")
        finally:
            c.close()
        log("client closed")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
