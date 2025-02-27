#!/usr/bin/env python3
import datetime

print("Content-type: text/html\n")
print("<h3>Current Server Time</h3>")
print(f"<p>{datetime.datetime.now()}</p>")