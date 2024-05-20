#include "hardware/Distance_sensor.h"

using namespace std;
namespace sdk {
    jclass DistanceSensor;
    jclass DistanceUnit;

    Distance_sensor::Distance_sensor(jobject distanceSensor) : distanceSensor(distanceSensor) {}

    Distance_sensor::~Distance_sensor() {
        if (distanceSensor) {
            attach_thread
            env->DeleteGlobalRef(distanceSensor);
            distanceSensor = nullptr;
        }
    }

    Distance_sensor &Distance_sensor::operator=(jobject distanceSensor) {
        if (this->distanceSensor) {
            attach_thread
            env->DeleteGlobalRef(this->distanceSensor);
        }
        this->distanceSensor = distanceSensor;
        return *this;
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
