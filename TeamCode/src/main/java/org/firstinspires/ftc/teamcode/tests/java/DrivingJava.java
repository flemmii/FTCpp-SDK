package org.firstinspires.ftc.teamcode.tests.java;

import com.qualcomm.robotcore.eventloop.opmode.LinearOpMode;
import com.qualcomm.robotcore.eventloop.opmode.TeleOp;
import com.qualcomm.robotcore.hardware.DcMotorEx;

import java.util.ArrayList;

@TeleOp(name = "DrivingJava", group = "tests")
public class DrivingJava extends LinearOpMode {
    
    @Override
    public void runOpMode() throws InterruptedException {
        // This gets all the drive motors from the hardware map
        DcMotorEx front_right = hardwareMap.get(DcMotorEx.class, "front_right");
        DcMotorEx front_left = hardwareMap.get(DcMotorEx.class, "front_left");
        DcMotorEx rear_right = hardwareMap.get(DcMotorEx.class, "rear_right");
        DcMotorEx rear_left = hardwareMap.get(DcMotorEx.class, "rear_left");

        // Reverses two motors, so forward is all motors to positive power
        // You may need to adjust this to meet your robots configuration
        front_left.setDirection(DcMotorEx.Direction.REVERSE);
        rear_left.setDirection(DcMotorEx.Direction.REVERSE);

        long lastTimeMicros = System.nanoTime() / 1000;
        long startTime = System.currentTimeMillis();
        int loopTime;
        ArrayList<Integer> loopTimes = new ArrayList<>();
        int minLoopTime = 1000000000;
        int maxLoopTime = 0;
        double avarageLoopTime;

        telemetry.addLine("Initialized");
        telemetry.update();

        waitForStart();

        startTime = System.currentTimeMillis();
        lastTimeMicros = System.nanoTime() / 1000;

        while (opModeIsActive() && (System.currentTimeMillis() - startTime) < 60000) {
            // The y direction of the stick is reversed in this sdk
            float leftStickY = -gamepad1.left_stick_y;
            float leftStickX = gamepad1.left_stick_x;
            float rightStickX = gamepad1.right_stick_x;

            // This sets the power of the motors to the y value of the left stick
            front_right.setPower(leftStickY - leftStickX - rightStickX);
            front_left.setPower(leftStickY + leftStickX + rightStickX);
            rear_right.setPower(leftStickY + leftStickX - rightStickX);
            rear_left.setPower(leftStickY - leftStickX + rightStickX);

            // Calculating loop times
            loopTime = (int) (System.nanoTime() / 1000 - lastTimeMicros);
            lastTimeMicros = System.nanoTime() / 1000;

            loopTimes.add(loopTime);
            avarageLoopTime = loopTimes.stream().mapToInt(Integer::intValue).sum() / (double) (loopTimes.size());

            if (loopTime > maxLoopTime)
                maxLoopTime = loopTime;

            if (loopTime < minLoopTime)
                minLoopTime = loopTime;

            telemetry.addData("Loop time", loopTime);
            telemetry.addData("Loop times size", (double) (loopTimes.size()));
            telemetry.addData("Avarage loop time", avarageLoopTime);
            telemetry.addData("Max loop time", maxLoopTime);
            telemetry.addData("Min loop time", minLoopTime);
            telemetry.update();
        }

        while(!isStopRequested());

        stop();
    }
}
