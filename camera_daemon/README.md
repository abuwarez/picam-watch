# camera_daemon

[picamera](https://github.com/waveform80/picamera) based python application running on a raspberry pi for continuous recording w/ basic support for live streaming and remote configuration

## Contents

**oper** operator scripts (current recording file, clean old recordings etc.)
**deploy** configuration and scripts to deploy camera_daemon as systemd service

## Basic Operation

once started, camera_daemon records h264 encoded video to */data/recs* in chunks of 100 MB, listens to remote commands on UDP port 11111 and listens for video stream clients on port 12345. All these value are currently configurable by changing camera_daemon.py.

### Streaming

The live video stream can be played using (VLC Media Player)[https://www.videolan.org] by opening a network stream using the following url: 
> tpc/h264://<rapsberry_pi_ip_addr>:12345

In addition to that, camera_daemon can be instructed to connect to a remote video streaming server using the UDP command interface. I added this funtionality before the streaming server and thus I consider it obsolete and will remove it.

Nevertheless, to use it:

- start streaming server on your PC using netcat and VLC:
```
c:\Windows\System32\Nc64 -lp 5555 | "c:\Program Files (x86)\VideoLAN\VLC\vlc.exe" --demux h264 -
```

- send start video stream commnad to camera_daemon over UDP port 11111 using netcat
```
c:\Windows\System32\Nc64 -u <raspberry_pi_ip_address>:11111
start_video_stream <your PC IP address>:5555
```

## Major TODOs

- configuration params such as recordings folder and size, ports, camera capture settings etc. from config file
- camera capture settings configurable over the UDP command server as well
- infrastructure for sending out various alerts (e.g disk space)
