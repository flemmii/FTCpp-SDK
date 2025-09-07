#include "vision/picture_utils.hpp"

#include "constants.hpp"

using namespace std;
using namespace cv;

namespace picture_utils
{
    Mat normalize_value(const Mat &hsv)
    {
        vector<Mat> channels;
        split(hsv, channels);
        Mat valueChannel = channels[2];

        double averageValue = mean(valueChannel).val[0];
        valueChannel.convertTo(valueChannel, -1, 128 / averageValue, 0);
        valueChannel.copyTo(channels[2]);

        merge(channels, hsv);
        return hsv;
    }

    Mat cut_off_outer_pixel(const Mat &input)
    {
        return input({4, 4, input.cols - 8, input.rows - 8});
    }

    Mat filter_out_large_objects(const Mat &input)
    {
        Mat output;
        erode(input, output, getStructuringElement(MORPH_RECT, Size(1, 1)));
        dilate(output, output, getStructuringElement(MORPH_RECT, Size(1, 1)));
        return output;
    }

    double measure_distance(double object_mm, int object_pixels)
    {
        if (object_pixels == 0)
            return 0;

        // 480 is the height of the image if the resolution is 640x480
        return (camera::FOCAL_LENGTH * object_mm * 480) / (object_pixels * camera::SENSOR_HEIGHT);
    }
} // picture_normalization