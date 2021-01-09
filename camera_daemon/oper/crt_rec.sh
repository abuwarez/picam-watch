#!/bin/bash

stat /data/recs/`ls /data/recs | grep -E "^[0-9]{6}x*" | sort | tail -n 1`
