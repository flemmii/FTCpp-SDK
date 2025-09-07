package org.firstinspires.ftc.teamcode.tests.java;

import android.graphics.Bitmap;
import android.graphics.Canvas;

import com.qualcomm.robotcore.eventloop.opmode.LinearOpMode;
import com.qualcomm.robotcore.eventloop.opmode.TeleOp;

import org.firstinspires.ftc.robotcore.external.Telemetry;
import org.firstinspires.ftc.robotcore.external.hardware.camera.CameraName;
import org.firstinspires.ftc.robotcore.external.hardware.camera.WebcamName;
import org.firstinspires.ftc.robotcore.internal.camera.calibration.CameraCalibration;
import org.firstinspires.ftc.vision.VisionPortal;
import org.firstinspires.ftc.vision.VisionProcessor;
import org.opencv.android.Utils;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;
import org.opencv.imgproc.Moments;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

@TeleOp(name = "TeamPropDetectionJava", group = "tests")
public class TeamPropDetectionJava extends LinearOpMode {
    long lastTimeMicros = System.nanoTime() / 1000;
    long startTime = System.currentTimeMillis();
    int loopTime;
    int count = 0;
    int minLoopTime = 1000000000;
    int maxLoopTime = 0;
    double averageLoopTime = 0;

    @Override
    public void runOpMode() throws InterruptedException {
        telemetry.addLine("Initialized");
        telemetry.update();

        waitForStart();

        startTime = System.currentTimeMillis();
        lastTimeMicros = System.nanoTime() / 1000;

        TeamPropDetection detection = new TeamPropDetection(lastTimeMicros, startTime, loopTime, count, minLoopTime, maxLoopTime, averageLoopTime, telemetry);
        VisionPortal portal = VisionPortal.easyCreateWithDefaults(hardwareMap.get(WebcamName.class, "Webcam"), detection);

        while (!isStopRequested() && System.currentTimeMillis() - startTime < 60000) ;
        portal.setProcessorEnabled(detection, false);
        while (!isStopRequested()) ;

        stop();
    }
}

class TeamPropDetection implements VisionProcessor {
    Mat frame = new Mat();

    Scalar largestContourColor = new Scalar(255, 255, 255);
    Scalar secondLargestContourColor = new Scalar(255, 255, 255);
    Scalar arrowColor = new Scalar(255, 255, 255);

    final Scalar allianceColor = new Scalar(0, 0, 255);
    final Scalar lowHSV1 = new Scalar(90, 80, 80);
    final Scalar highHSV1 = new Scalar(120, 255, 255);

    MatOfPoint largestContour = new MatOfPoint();
    MatOfPoint secondLargestContour = new MatOfPoint();

    int positionTeamProp = 0;
    boolean noTeamPropDetected = false;

    long lastTimeMicros;
    long startTime;
    int loopTime;
    int count;
    int minLoopTime;
    int maxLoopTime;
    double averageLoopTime;

    Telemetry telemetry;

    TeamPropDetection(long lastTimeMicros, long startTime, int loopTime, int count, int minLoopTime, int maxLoopTime, double averageLoopTime, Telemetry telemetry) {
        this.lastTimeMicros = lastTimeMicros;
        this.startTime = startTime;
        this.loopTime = loopTime;
        this.count = count;
        this.minLoopTime = minLoopTime;
        this.maxLoopTime = maxLoopTime;
        this.averageLoopTime = averageLoopTime;
        this.telemetry = telemetry;
    }

    @Override
    public void init(int width, int height, CameraCalibration calibration) {

    }

    @Override
    public Object processFrame(Mat frame, long captureTimeNanos) {
        this.frame = frame;

        Mat hsv = new Mat();
        Imgproc.cvtColor(frame, hsv, Imgproc.COLOR_RGB2HSV);

        Mat finalThresh = new Mat();
        Core.inRange(hsv, lowHSV1, highHSV1, finalThresh);

        List<MatOfPoint> contours = new ArrayList<>();
        Imgproc.findContours(finalThresh, contours, new Mat(), Imgproc.RETR_EXTERNAL, Imgproc.CHAIN_APPROX_SIMPLE);

        double largestArea = -1;
        double secondLargestArea = -1;

        for (MatOfPoint contour : contours) {
            double area = Imgproc.contourArea(contour);
            if (area > secondLargestArea) {
                if (area > largestArea) {
                    secondLargestArea = largestArea;
                    secondLargestContour = largestContour;

                    largestArea = area;
                    largestContour = contour;

                } else {
                    secondLargestArea = area;
                    secondLargestContour = contour;
                }
            }
        }

        Point midLargestContour;
        Point midSecondLargestContour = new Point();
        MatOfPoint2f approxCurveLargestContour = new MatOfPoint2f();
        MatOfPoint2f approxCurveSecondLargestContour = new MatOfPoint2f();

        if (!largestContour.empty()) {
            noTeamPropDetected = false;

            MatOfPoint2f contour2f = new MatOfPoint2f(largestContour.toArray());
            double epsilon = 0.02 * Imgproc.arcLength(contour2f, true);
            Imgproc.approxPolyDP(contour2f, approxCurveLargestContour, epsilon, true);

            Moments moments = Imgproc.moments(largestContour);
            midLargestContour = new Point(moments.get_m10() / moments.get_m00(), moments.get_m01() / moments.get_m00());

            if (!secondLargestContour.empty()) {
                contour2f = new MatOfPoint2f(secondLargestContour.toArray());
                epsilon = 0.02 * Imgproc.arcLength(contour2f, true);
                Imgproc.approxPolyDP(contour2f, approxCurveSecondLargestContour, epsilon, true);

                moments = Imgproc.moments(secondLargestContour);
                midSecondLargestContour = new Point(moments.get_m10() / moments.get_m00(), moments.get_m01() / moments.get_m00());
            }

            if (approxCurveLargestContour.total() > 5 && !largestContour.empty()) {
                largestContourColor = allianceColor;

                if (midLargestContour.x < midSecondLargestContour.x) {
                    positionTeamProp = 1;
                } else {
                    positionTeamProp = 2;
                }
            } else if (approxCurveSecondLargestContour.total() > 5 && !secondLargestContour.empty()) {
                secondLargestContourColor = allianceColor;

                if (midSecondLargestContour.x < midLargestContour.x) {
                    positionTeamProp = 1;
                } else {
                    positionTeamProp = 2;
                }
            } else {
                arrowColor = allianceColor;
                positionTeamProp = 0;
            }
        } else {
            noTeamPropDetected = true;
            // It will use the last detected position
        }

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

        return null;
    }

    @Override
    public void onDrawFrame(Canvas canvas, int onscreenWidth, int onscreenHeight, float scaleBmpPxToCanvasPx, float scaleCanvasDensity, Object userContext) {
        if (!largestContour.empty()) {
            Imgproc.drawContours(frame, Collections.singletonList(largestContour), -1, largestContourColor, 5);
            if (!secondLargestContour.empty()) {
                Imgproc.drawContours(frame, Collections.singletonList(secondLargestContour), -1, secondLargestContourColor, 5);
            }
        }
        Imgproc.arrowedLine(frame, new Point(100, 120), new Point(0, 120), arrowColor, 5);

        Bitmap bitmap = Bitmap.createBitmap(640, 480, Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(frame, bitmap);
        Bitmap scaledBitmap = Bitmap.createScaledBitmap(bitmap, (int) (640 * scaleBmpPxToCanvasPx), (int) (480 * scaleBmpPxToCanvasPx), true);
        canvas.drawBitmap(scaledBitmap, 0, 0, null);

    }
}