//
// Created by fnlg on 29.04.2024.
//

#include "../../header/vision/picture_normalization.h"

using namespace std;
using namespace cv;

namespace picture_normalization {
    Mat normalize_value(const Mat &hsv) {
        vector<Mat> channels;
        split(hsv, channels);
        Mat valueChannel = channels[2];

        double averageValue = mean(valueChannel).val[0];
        valueChannel.convertTo(valueChannel, -1, 128 / averageValue, 0);
        valueChannel.copyTo(channels[2]);

        merge(channels, hsv);
        return hsv;
    }

    Mat cut_off_outer_pixel(const Mat &input) {
        return input({4, 4, input.cols - 8, input.rows - 8});
    }

    Mat filter_out_large_objects(const Mat &input) {
        Mat output;
        erode(input, output, getStructuringElement(MORPH_RECT, Size(1, 1)));
        dilate(output, output, getStructuringElement(MORPH_RECT, Size(1, 1)));
        return output;
    }
} // picture_normalization