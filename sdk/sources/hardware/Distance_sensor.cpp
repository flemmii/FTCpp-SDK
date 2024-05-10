#include "hardware/Distance_sensor.h"

using namespace std;
namespace sdk {
    jclass DistanceSensor;
    jclass DistanceUnit;

    Distance_sensor::Distance_sensor(jobject distanceSensor) {
        attach_thread
        this->distanceSensor = env->NewGlobalRef(distanceSensor);
    }

    Distance_sensor::~Distance_sensor() {
        if (distanceSensor) {
            attach_thread
            env->DeleteGlobalRef(distanceSensor);
        }
    }

    double Distance_sensor::get_distance() {
        attach_thread
        jobject mmObject = env->GetStaticObjectField(DistanceUnit,
                                                     env->GetStaticFieldID(DistanceUnit, "MM",
                                                                           "Lorg/firstinspires/ftc/robotcore/external/navigation/DistanceUnit;"));

        auto result = static_cast<double>(env->CallDoubleMethod(distanceSensor,
                                                                env->GetMethodID(DistanceSensor,
                                                                                 "getDistance",
                                                                                 "(Lorg/firstinspires/ftc/robotcore/external/navigation/DistanceUnit;)D"),
                                                                mmObject));
        env->DeleteLocalRef(mmObject);
        return result;
    }
}
