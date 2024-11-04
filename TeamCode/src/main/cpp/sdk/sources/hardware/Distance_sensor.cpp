#include "hardware/Distance_sensor.h"

using namespace std;
namespace sdk {
    jclass Distance_sensor::jclazz;
    jclass Distance_sensor::DistanceUnit;

    Distance_sensor::Distance_sensor(const jobject &distanceSensor) : distanceSensor(
            distanceSensor) {}

    Distance_sensor::~Distance_sensor() {
        if (distanceSensor) {
            attach_thread
            env->DeleteGlobalRef(distanceSensor);
            distanceSensor = nullptr;
        }
    }

    Distance_sensor &Distance_sensor::operator=(const Distance_sensor &distance_sensor) {
        if (this != &distance_sensor && distance_sensor.distanceSensor) {
            attach_thread
            this->distanceSensor = env->NewGlobalRef(distance_sensor.distanceSensor);
        }
        return *this;
    }

    Distance_sensor &Distance_sensor::operator=(const jobject &distanceSensor) {
        if (this->distanceSensor) {
            attach_thread
            env->DeleteGlobalRef(this->distanceSensor);
        }
        this->distanceSensor = distanceSensor;
        return *this;
    }


    double Distance_sensor::get_distance() const {
        attach_thread
        jobject mmObject = env->GetStaticObjectField(DistanceUnit,
                                                     env->GetStaticFieldID(DistanceUnit, "MM",
                                                                           "Lorg/firstinspires/ftc/robotcore/external/navigation/DistanceUnit;"));

        auto result = static_cast<double>(env->CallDoubleMethod(distanceSensor,
                                                                env->GetMethodID(jclazz,
                                                                                 "getDistance",
                                                                                 "(Lorg/firstinspires/ftc/robotcore/external/navigation/DistanceUnit;)D"),
                                                                mmObject));
        env->DeleteLocalRef(mmObject);
        return result;
    }
}
