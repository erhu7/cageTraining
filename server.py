"""
Tiny lab server: serves task.html to the tablet, saves every trial to a CSV,
and gives you a place to trigger the reward hardware.

Run:   python server.py
Then on the tablet's browser go to:   http://<this-computer's-IP>:8000/task.html
(Standard library only - nothing to install.)
"""
import csv, json, os
from http.server import SimpleHTTPRequestHandler, HTTPServer

DATA_FILE = "trial_log.csv"

def fire_reward():
    # >>> Put your hardware trigger here <<<
    # Examples: send a serial command to an Arduino, toggle a Raspberry Pi GPIO pin,
    # call a pump controller's HTTP API, etc.
    print("REWARD!")

class Handler(SimpleHTTPRequestHandler):
    def do_POST(self):
        length = int(self.headers.get("Content-Length", 0))
        body = self.rfile.read(length)

        if self.path == "/log":
            rec = json.loads(body)
            new_file = not os.path.exists(DATA_FILE)
            with open(DATA_FILE, "a", newline="") as f:
                w = csv.DictWriter(f, fieldnames=rec.keys())
                if new_file:
                    w.writeheader()
                w.writerow(rec)
        elif self.path == "/reward":
            fire_reward()

        self.send_response(200)
        self.end_headers()

if __name__ == "__main__":
    print("Serving on port 8000")
    HTTPServer(("0.0.0.0", 8000), Handler).serve_forever()
