package org.firstinspires.ftc.teamcode.tests.java;

import android.util.Log;

import com.qualcomm.hardware.bosch.BNO055IMU;
import com.qualcomm.hardware.lynx.LynxModule;
import com.qualcomm.robotcore.eventloop.opmode.LinearOpMode;
import com.qualcomm.robotcore.eventloop.opmode.TeleOp;
import com.qualcomm.robotcore.hardware.DcMotor;
import com.qualcomm.robotcore.hardware.DcMotorEx;
import com.sun.tools.javac.util.FatalError;

import org.opencv.core.Mat;

import java.util.ArrayList;
import java.util.Collections;

@TeleOp(name = "TrackPositionJava", group = "tests")
public class TrackPositionJava extends LinearOpMode {
    final int ENCODER_TICKS_PER_ROTATION = 8192;
    final int WHEEL_DIAMETER_MM = 60;

    @Override
    public void runOpMode() throws InterruptedException {
        DcMotorEx deadWheelForwards = hardwareMap.get(DcMotorEx.class, "rear_left");
        DcMotorEx deadWheelSidewards = hardwareMap.get(DcMotorEx.class, "front_right");

        deadWheelSidewards.setMode(DcMotorEx.RunMode.STOP_AND_RESET_ENCODER);

        deadWheelForwards.setMode(DcMotorEx.RunMode.RUN_WITHOUT_ENCODER);
        deadWheelSidewards.setMode(DcMotorEx.RunMode.RUN_WITHOUT_ENCODER);

        LynxModule controlHub = hardwareMap.get(LynxModule.class, "Control Hub");
        LynxModule expansionHub = hardwareMap.get(LynxModule.class, "Expansion Hub 2");
        controlHub.setBulkCachingMode(LynxModule.BulkCachingMode.MANUAL);
        expansionHub.setBulkCachingMode(LynxModule.BulkCachingMode.MANUAL);

        BNO055IMU imu = hardwareMap.get(BNO055IMU.class, "imu1");
        BNO055IMU.Parameters parameters = new BNO055IMU.Parameters();
        parameters.angleUnit = BNO055IMU.AngleUnit.DEGREES;
        parameters.accelUnit = BNO055IMU.AccelUnit.METERS_PERSEC_PERSEC;
        imu.initialize(parameters);
        imu.startAccelerationIntegration(null, null, 100);

        int encoderForwardPos = 0;
        int encoderSidewardsPos = 0;

        double yPosition = 0;
        double xPosition = 0;
        double rotationRad = 0;
        double rotationDeg = 0;
        double rotationDegContinuous = 0;
        double extraRad = 0;
        int extraDegrees = 0;

        long lastTimeMicros = System.nanoTime() / 1000;
        long startTime = System.currentTimeMillis();
        int loopTime;
        int count = 0;
        int minLoopTime = 1000000000;
        int maxLoopTime = 0;
        double averageLoopTime = 0;

        ArrayList<Double> ringBufferXSpeed = new ArrayList<>(Collections.nCopies(5, 0.0));
        ArrayList<Double> ringBufferYSpeed = new ArrayList<>(Collections.nCopies(5, 0.0));

        double ySpeed;
        double xSpeed;

        telemetry.addLine("Initialized");
        telemetry.update();

        waitForStart();

        startTime = System.currentTimeMillis();
        lastTimeMicros = System.nanoTime() / 1000;

        while (!isStopRequested() && (System.currentTimeMillis() - startTime) < 60000) {
            // Getting all the data of the hubs
            controlHub.getBulkData();
            if (controlHub.isNotResponding())
                Log.e("trackPosition", "control hub not responding");

            expansionHub.getBulkData();
            if (expansionHub.isNotResponding())
                Log.e("trackPosition", "expansion hub not responding");

            // Writing all the necessary logged data from the loop before into variables
            double previousRotationDeg = rotationDeg;
            double previousRotationDegContinuous = rotationDegContinuous;
            int previousEncoderForwardPos = encoderForwardPos;
            int previousEncoderSidewardsPos = encoderSidewardsPos;

            // Getting all the new data for tracking
            rotationRad = -imu.getAngularOrientation().thirdAngle + extraRad;
            encoderForwardPos = -deadWheelForwards.getCurrentPosition();
            encoderSidewardsPos = deadWheelSidewards.getCurrentPosition();
            double delta_t = (double) (System.nanoTime() / 1000 - lastTimeMicros) / 1000;

            // Converting radians to degrees and continuous degree system
            rotationDeg = (rotationRad * 180) / Math.PI;
            if (previousRotationDeg - rotationDeg > 180)
                extraDegrees += 360;
            else if (previousRotationDeg - rotationDeg < -180)
                extraDegrees -= 360;
            rotationDegContinuous = rotationDeg + extraDegrees;

            // Calculating the delta values relative to the robots facing
            double deltaForward =
                    ((double) (previousEncoderForwardPos - encoderForwardPos) /
                            ENCODER_TICKS_PER_ROTATION) * WHEEL_DIAMETER_MM * Math.PI;
            double deltaSidewards =
                    ((double) (previousEncoderSidewardsPos - encoderSidewardsPos) /
                            ENCODER_TICKS_PER_ROTATION) * WHEEL_DIAMETER_MM * Math.PI;
            double deltaRotation =
                    (previousRotationDegContinuous - rotationDegContinuous) * (Math.PI / 180);

            // This calculates how the deltas are if we got high rotation speed
            // Because if we got high rotation speed we'll rather drive a curve than a line
            if (Math.abs(deltaRotation) > 3) {
                double radius_forward = deltaForward / deltaRotation;
                double radius_sidewards = deltaSidewards / deltaRotation;

                double sinus_delta_rot = Math.sin(deltaRotation);
                double cosinus_delta_rot = Math.cos(deltaRotation);

                deltaForward = radius_forward * sinus_delta_rot +
                        radius_sidewards * (1 - cosinus_delta_rot);
                deltaSidewards = radius_forward * (1 - cosinus_delta_rot) -
                        radius_sidewards * sinus_delta_rot;
            }

            // Calculating the way we drove in x and y direction since the last loop
            double sinus = Math.sin(rotationRad);
            double cosinus = Math.cos(rotationRad);
            double deltaY = deltaForward * cosinus + deltaSidewards * sinus;
            double deltaX = deltaForward * sinus - deltaSidewards * cosinus;

            // Adding this way to the current position -> new position
            yPosition += deltaY;
            xPosition += deltaX;

            // Converting the deltas into the speeds
            deltaY /= delta_t;
            deltaX /= delta_t;

            // Stores the last n speeds in a ring buffer (n is defined at the declaration of the ring buffers)
            ringBufferXSpeed.add(0, deltaX);
            ringBufferYSpeed.add(0, deltaY);

            // Deletes the last element
            ringBufferXSpeed.remove(ringBufferXSpeed.size() - 1);
            ringBufferYSpeed.remove(ringBufferYSpeed.size() - 1);

            // Calculates the speeds through adding up all the last n calculated speeds
            // and then dividing them through n. This way the speeds get normalized and
            // we don't get completely wrong values
            ySpeed = ringBufferXSpeed.stream().mapToDouble(Double::doubleValue).sum() /
                    ringBufferXSpeed.size();
            xSpeed = ringBufferYSpeed.stream().mapToDouble(Double::doubleValue).sum() /
                    ringBufferYSpeed.size();

            // Calculating loop times
            loopTime = (int) (System.nanoTime() / 1000 - lastTimeMicros);

            averageLoopTime = (averageLoopTime * count + loopTime) / (count + 1);
            count++;

            if (loopTime > maxLoopTime)
                maxLoopTime = loopTime;

            if (loopTime < minLoopTime)
                minLoopTime = loopTime;

            telemetry.addData("Loop time", loopTime);
            telemetry.addData("Loop count", count);
            telemetry.addData("Average loop time", averageLoopTime);
            telemetry.addData("Max loop time", maxLoopTime);
            telemetry.addData("Min loop time", minLoopTime);
            telemetry.update();

            lastTimeMicros = System.nanoTime() / 1000;
        }
        while (!isStopRequested()) ;

        stop();
    }
}
