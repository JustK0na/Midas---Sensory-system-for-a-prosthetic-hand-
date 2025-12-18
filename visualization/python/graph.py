#!/usr/bin/env python3

import socketserver
import threading
from collections import deque
import matplotlib.pyplot as plt
import matplotlib.animation as animation

HOST = '0.0.0.0'
PORT = 5000
Time = 10000
POINTS = 1000      
UPDATE_MS = 10    

class SimpleBuffers:
    def __init__(self, maxlen):
        from threading import Lock
        self.lock = Lock()
        self.times = deque(maxlen=maxlen)
        self.d1 = deque(maxlen=maxlen)
        self.d2 = deque(maxlen=maxlen)

    def append(self, t, v1, v2):
        with self.lock:
            self.times.append(t)
            self.d1.append(v1)
            self.d2.append(v2)

    def snapshot(self):
        with self.lock:
            return list(self.times), list(self.d1), list(self.d2)

BUFF = SimpleBuffers(POINTS)

# ---------- Minimal TCP handler ----------
class Handler(socketserver.BaseRequestHandler):
    def handle(self):
        f = self.request.makefile('r')
        try:
            for raw in f:
                line = raw.strip()
                if not line:
                    continue
                parts = line.split(';')
                if len(parts) < 3:
                    continue
                try:
                    t = float(parts[0])
                    v1 = float(parts[1])
                    v2 = float(parts[2])
                except Exception:
                    continue
                BUFF.append(t, v1, v2)
        finally:
            try:
                f.close()
            except Exception:
                pass

class ThreadedServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    allow_reuse_address = True
    daemon_threads = True

# ---------- Plotting (main thread) ----------

def run_plot():
    fig, ax = plt.subplots()
    l1, = ax.plot([], [], label='data1')
    #l2, = ax.plot([], [], label='data2')
    ax.set_xlabel('time')
    ax.set_ylabel('value')
    ax.legend()
    ax.grid(True)

    fig2, ax2 = plt.subplots()
    #l3, = ax.plot([], [], label='data1')
    l2, = ax2.plot([], [], label='data2')
    ax2.set_xlabel('time')
    ax2.set_ylabel('value')
    ax2.legend()
    ax2.grid(True)

    def init():
        ax.set_xlim(0, 1)
        ax.set_ylim(0, 1)

        ax2.set_xlim(0, 1)
        ax2.set_ylim(0, 1)

        return l1, l2

    def update(frame):
        x, y1, y2 = BUFF.snapshot()
        if not x:
            return l1, l2
        l1.set_data(x, y1)
        #l2.set_data(x, y2)
        xmin, xmax = max(x) - (Time - 2000), max(x)
        if xmin < 0:
            xmin = 0
        if xmin == xmax:
            xmin -= 0.5
            xmax += 0.5
        
        ax.set_xlim(xmin, xmax)
        
        ymax = max(y1[-Time:]) + 75
        ymin = min(y1[-Time:]) - 75
        if ymin == ymax:
            ymin -= 0.5
            ymax += 0.5

        ax.set_ylim(ymin, ymax)
        return l1
    
    def update2(frame):
        x, y1, y2 = BUFF.snapshot()
        if not x:
            return l1, l2
        #l1.set_data(x, y1)
        l2.set_data(x, y2)
        xmin, xmax = max(x) -(Time - 2000), max(x)
        if xmin < 0:
            xmin = 0
        if xmin == xmax:
            xmin -= 0.5
            xmax += 0.5
        ax2.set_xlim(xmin, xmax)
        
        ymax = max(y2[-Time:]) + 75
        ymin = min(y2[-Time:]) - 75
        if ymin == ymax:
            ymin -= 0.5
            ymax += 0.5

        ax2.set_ylim(ymin, ymax)

        print(ymax, ymin)
        return l2

    ani = animation.FuncAnimation(fig, update, init_func=init, interval=UPDATE_MS, blit=False)
    ani2 = animation.FuncAnimation(fig2, update2, init_func=init, interval = UPDATE_MS, blit=False)
    plt.show()

# ---------- Start server and plotting ----------

def main():
    srv = ThreadedServer((HOST, PORT), Handler)
    t = threading.Thread(target=srv.serve_forever, daemon=True)
    t.start()
    print(f"Listening on {HOST}:{PORT} — showing last {POINTS} points")
    try:
        run_plot()
    except KeyboardInterrupt:
        pass
    finally:
        srv.shutdown()
        srv.server_close()

if __name__ == '__main__':
    main()
