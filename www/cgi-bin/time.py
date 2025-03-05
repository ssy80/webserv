#!/usr/bin/env python3
import datetime

try:
    print(f"<h2>Current Server Time</h2><p>{datetime.datetime.now()}</p>")

except:
    print("<h2>Error</h2>")