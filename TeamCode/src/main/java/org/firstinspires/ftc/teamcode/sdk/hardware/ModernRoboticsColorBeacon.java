package org.firstinspires.ftc.teamcode.sdk.hardware;

import com.qualcomm.robotcore.hardware.I2cAddr;
import com.qualcomm.robotcore.hardware.I2cDevice;
import com.qualcomm.robotcore.hardware.I2cDeviceSynch;
import com.qualcomm.robotcore.hardware.I2cDeviceSynchDevice;
import com.qualcomm.robotcore.hardware.configuration.annotations.DeviceProperties;
import com.qualcomm.robotcore.hardware.configuration.annotations.I2cDeviceType;

@SuppressWarnings("unused")
@I2cDeviceType()
@DeviceProperties(name = "Modern Robotics Color Beacon", description = "Color Beacon", xmlTag = "ColorBeacon")
public class ModernRoboticsColorBeacon extends I2cDeviceSynchDevice<I2cDeviceSynch> {

    I2cDevice colorBeacon;
    I2cDeviceSynch colorBeaconSynch;

    public ModernRoboticsColorBeacon(I2cDeviceSynch colorBeaconSynch) {
        //default address: 0x4c
        super(colorBeaconSynch, true);
        this.colorBeaconSynch = colorBeaconSynch;
        this.colorBeaconSynch.setI2cAddress(I2cAddr.create8bit(0x4c));
        super.registerArmingStateCallback(false);
        this.colorBeaconSynch.engage();
        setColor(0, 0, 0);
    }

    public void turnLEDsOff() {
        colorBeaconSynch.write8(4, 0); // off
    }

    public void setRed() {
        colorBeaconSynch.write8(4, 1); // red
    }

    public void setGreen() {
        colorBeaconSynch.write8(4, 2); // green
    }

    public void setYellow() {
        colorBeaconSynch.write8(4, 3); // yellow
    }

    public void setBlue() {
        colorBeaconSynch.write8(4, 4); // blue
    }

    public void setPurple() {
        colorBeaconSynch.write8(4, 5); // purple
    }

    public void setTeal() {
        colorBeaconSynch.write8(4, 6); // teal
    }

    public void setWhite() {
        colorBeaconSynch.write8(4, 7); // white
    }

    public void setColor(int red, int green, int blue) {
        colorBeaconSynch.write8(4, 8); // custom color mode
        colorBeaconSynch.write8(5, red); // red
        colorBeaconSynch.write8(6, green); // green
        colorBeaconSynch.write8(7, blue); // blue
    }

    @Override
    protected boolean doInitialize() {
        return true;
    }

    @Override
    public Manufacturer getManufacturer() {
        return Manufacturer.ModernRobotics;
    }

    @Override
    public String getDeviceName() {
        return "ColorBeacon";
    }
}
