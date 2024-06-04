# Results of the tests
This test files are designed to test the performance of C++ versus Java in various realistic applications.
They are also designed to be nearly the exact same in C++ and Java so the loop times can get compared

## Location of the test files
[Java Test files](https://github.com/flemmii/FTCpp-SDK/new/Tests/TeamCode/src/main/java/org/firstinspires/ftc/teamcode/tests/java)
[Cpp Test files](https://github.com/flemmii/FTCpp-SDK/new/Tests/TeamCode/src/main/cpp/teamcode/sources/tests)

## How the tests were done
At first the C++ program got executed and afterwards with (no) time between the Java program got executed.
Both programs ran 1 minute long and mesaured the time one loop took in Microseconds.
The robot was not moved or was influenced from outside in the entire test (except for the last driving test).

## Results

### TrackPosition
#### C++
Overall loops: 4919 loops
Avarage loop time: 12.1981 ms
Loops per second: 81.9800 loops/s
Max loop time: 24.625 ms
Min loop time: 8.766 ms

#### Java
Overall loops: 4834 loops
Avarage loop time: 12.4047 ms
Loops per second: 80.6146 loops/s
Max loop time: 28.681 ms
Min loop time: 8.579 ms

### TeamPropDetection
#### C++
Overall loops: 883 loops
Avarage loop time: 67.8918 ms
Loops per second: 14.7293 loops/s
Max loop time: 1138.217 ms (This may be the time it took to create the vision portal)
Min loop time: 49.037 ms

#### Java
Overall loops: 883 loops
Avarage loop time: 67.9092 ms
Loops per second: 14.7076 loops/s
Max loop time: 1162.971 ms (This may be the time it took to create the vision portal)
Min loop time: 36.211 ms

### Driving (No movement)
#### C++
Overall loops: 31559 loops
Avarage loop time: 1.8993 ms
Loops per second: 526.5100 loops/s
Max loop time: 34.700 ms
Min loop time: 0.597 ms

#### Java
Overall loops: 22333 loops
Avarage loop time: 2.6848 ms
Loops per second: 372.4672 loops/s
Max loop time: 21.677 ms
Min loop time: 0.235 ms

### Driving (Driving around the field)
#### C++
Overall loops: 30561 loops
Avarage loop time: 1.9613 ms
Loops per second: 509.8659 loops/s
Max loop time: 20.929 ms
Min loop time: 0.593 ms

#### Java
Overall loops: 22004 loops
Avarage loop time: 2.7254 ms
Loops per second: 366.9186 loops/s
Max loop time: 17.143 ms
Min loop time: 0.145 ms
