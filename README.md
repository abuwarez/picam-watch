# picam-watch
Surveillance application suite based on picamera and ffmpeg

## Composition

### camera_daemon
[picamera](https://github.com/waveform80/picamera) based python application running on a raspberry pi for continuous recording w/ basic support for live streaming and remote configuration
### motion
[ffmpeg](https://www.ffmpeg.org/) based C++ application running on PC for offline motion detection inside the **camera_daemon** recordings, initially working on h264 only and w/ modding support. Can be build for raspberry pi as well.
### car_plugins
a long shot but why not... various plugins for running the **picam-watch** inside a car: car voltage monitor, car engine start for battery charging, trigger windshiled wipers. headlights control etc. w/ schematics, arduino and raspberry pi based.  

## Destination
- low power, high availability, cheap, continuous video recording
- online/offline video processing like motion detection
- given the recreational commitment, no plans for a fancy user interface. app from the **picam-watch** suite should be easy to use by people familiary with commandline tools and basic networking operations

## Commitment
Recreational and open at least to discussions regarding the development plans
