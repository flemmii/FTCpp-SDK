## apriltag

- Version: 3.4.2
- [Repository](https://github.com/AprilRobotics/apriltag/releases)

### update instructions

- Download the android sdk with the version written down
  in [BuildConfig.java](org.openftc.apriltag.BuildConfig) and
  the find out the corresponding version in
  the [EOCV-AprilTag-Plugin](https://github.com/OpenFTC/EOCV-AprilTag-Plugin/releases)
- Delete all files except the .hpp and .h files from the downloaded version
- Rename the folder to `apriltag`
- Replace the old folder with the new folder

## OpenCV

- Version: 4.10.0
- [Repository](https://github.com/opencv/opencv/releases)

### update instructions

- Download the android sdk with the version written down
  in [BuildConfig](org.openftc.easyopencv.BuildConfig) and
  the find out the corresponding version in
  the [EasyOpenCV repository](https://github.com/OpenFTC/EasyOpenCV/releases)
- Replace the opencv2 folder with the new opencv2 folder found in sdk/native/jni/include