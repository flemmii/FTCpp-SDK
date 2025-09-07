#include "hardware/Distance_sensor.hpp"

using namespace std;
namespace sdk
{
    jclass Distance_sensor::jclazz;

    Distance_sensor::Distance_sensor(const jobject &distanceSensor) : distanceSensor(
                                                                          distanceSensor) {}

    Distance_sensor::~Distance_sensor()
    {
        if (distanceSensor)
        {
            attach_thread
                env->DeleteGlobalRef(distanceSensor);
            distanceSensor = nullptr;
        }
    }

    Distance_sensor &Distance_sensor::operator=(const Distance_sensor &distance_sensor)
    {
        if (this != &distance_sensor && distance_sensor.distanceSensor)
        {
            attach_thread this->distanceSensor = env->NewGlobalRef(distance_sensor.distanceSensor);
        }
        return *this;
    }

    Distance_sensor &Distance_sensor::operator=(const jobject &distanceSensor)
    {
        if (this->distanceSensor)
        {
            attach_thread
                env->DeleteGlobalRef(this->distanceSensor);
        }
        this->distanceSensor = distanceSensor;
        return *this;
    }

    double Distance_sensor::get_distance(Distance_unit unit)
    {
        attach_thread
            jobject distanceUnit = env->GetStaticObjectField(Distance_unit::jclazz,
                                                             env->GetStaticFieldID(Distance_unit::jclazz,
                                                                                   "MM",
                                                                                   "Lorg/firstinspires/ftc/robotcore/external/navigation/DistanceUnit;"));

        jdouble distance = env->CallDoubleMethod(distanceSensor,
                                                 env->GetMethodID(jclazz, "getDistance",
                                                                  "(Lorg/firstinspires/ftc/robotcore/external/navigation/DistanceUnit;)D"),
                                                 distanceUnit);

        env->DeleteLocalRef(distanceUnit);

        return unit.from_unit(Distance_unit::MM, static_cast<double>(distance));
    }
}
