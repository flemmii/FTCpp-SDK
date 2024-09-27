# Results of the tests

This test files are designed to test the performance of C++ versus Java in various realistic
applications.
They are also designed to be nearly the exact same in C++ and Java so the loop times can get
compared

## Location of the test files

[Java Test files](https://github.com/flemmii/FTCpp-SDK/new/Tests/TeamCode/src/main/java/org/firstinspires/ftc/teamcode/tests/java)  
[Cpp Test files](https://github.com/flemmii/FTCpp-SDK/new/Tests/TeamCode/src/main/cpp/teamcode/sources/tests)

## How the tests were done

At first the Java program got executed and afterwards with (no) time between the C++ program got
executed.
Both programs ran 1 minute long and mesaured the time one loop took in Microseconds.
The robot was not moved or was influenced from outside in the entire test.

## Results

### TrackPosition

#### <span style="color:DodgerBlue"> C++ </span>

Execution Rate: 144.1649 Hz  
Avarage loop time: 6.9365 ms  
Max loop time: 18.779 ms  
Min loop time: 4.587 ms

#### <span style="color:Orange"> Java </span>

Execution rate: 141.0019 Hz  
Avarage loop time: 7.0921 ms  
Max loop time: 16.707 ms  
Min loop time: 4.538 ms

### TeamPropDetection

#### <span style="color:DodgerBlue"> C++ </span>

Execution rate: 28.9064 Hz  
Avarage loop time: 34.5944 ms  
Max loop time: 2500.715 ms (This may be the time it took to create the vision portal)  
Min loop time: 11.317 ms

#### <span style="color:Orange"> Java </span>

Execution rate: 28.5802 Hz  
Avarage loop time: 34.9893 ms  
Max loop time: 2551.550 ms (This may be the time it took to create the vision portal)  
Min loop time: 10.975 ms

### Driving

#### <span style="color:DodgerBlue"> C++ </span>

Execution rate: 2,260.3978 Hz  
Avarage loop time: 0.4424 ms  
Max loop time: 33.898 ms  
Min loop time: 0.368 ms

#### <span style="color:Orange"> Java </span>

Execution rate: 18,867.9245 Hz  
Avarage loop time: 0.0530 ms  
Max loop time: 20.934 ms  
Min loop time: 0.0410 ms
