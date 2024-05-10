package org.firstinspires.ftc.teamcode.VisionProcessor;

import android.graphics.Canvas;

import org.firstinspires.ftc.robotcore.internal.camera.calibration.CameraCalibration;
import org.firstinspires.ftc.vision.VisionProcessor;
import org.opencv.core.Mat;

public class SecondVisionProcessor implements VisionProcessor {
    @Override
    public void init(int width, int height, CameraCalibration calibration) {
        nativeInit(width, height, calibration);
    }

    @Override
    public Object processFrame(Mat frame, long captureTimeNanos) {
        nativeProcessFrame(frame.nativeObj, captureTimeNanos);
        return null;
    }

    @Override
    public void onDrawFrame(Canvas canvas, int onscreenWidth, int onscreenHeight, float scaleBmpPxToCanvasPx, float scaleCanvasDensity, Object userContext) {
        nativeOnDrawFrame(canvas, onscreenWidth, onscreenHeight, scaleBmpPxToCanvasPx, scaleCanvasDensity, userContext);
    }

    private native void nativeInit(int width, int height, CameraCalibration calibration);

    private native void nativeProcessFrame(long frame, long captureTimeNanos);

    private native void nativeOnDrawFrame(Canvas canvas, int onscreenWidth, int onscreenHeight, float scaleBmpPxToCanvasPx, float scaleCanvasDensity, Object userContext);
}
