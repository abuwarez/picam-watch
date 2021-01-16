# motion

[ffmpeg](https://www.ffmpeg.org/) based C++ application suit running on PC for offline motion detection inside the **camera_daemon** recordings, initially working on h264 only and w/ modding support. Can be build for raspberry pi as well.


## Building

Make based and Linux only for the moment.

```
# make motion
```

#### Dependencies

ffmpeg-dev
libavutil-dev
libavformat-dev
libavdevice-dev
libao-dev


## Running

```
# ./motion <h264 video file>


```

## Output

- A list with all frames where motion is detected, containing motion metrics
- A list of stream positions with duration where motion is detected

TODO: add sample output

## Configuration

- the motion algorithm used with its specific configuration


## Motion detection algos

- count pixel differenced between two consecutive frames on the Y (b&w) plane (*in progress*)
- find a way to localize areas inside the frame where motion occurs. not sure at the moment how to achive that, possibly by reducing the resolution. (*concept*)
- AI model to infer, based on the motion stats from each frame, the moving object class (a person, a car, a cyclist etc.) (*concept*)

## MISC

### Playing raw streams using ffplay

```
ffplay -f rawvideo -pix_fmt yuv420p -video_size 720x480 my_video
```
