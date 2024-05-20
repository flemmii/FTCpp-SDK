//
// Created by fnlg on 11.01.2024.
//

#include "hardware/Lynx_module.h"

using namespace std;

namespace sdk {
    jclass LynxModule;
    jclass BulkCachingMode;

    Lynx_module::Lynx_module(jobject lynxModule) : lynxModule(lynxModule) {}

    Lynx_module::~Lynx_module() {
        if (lynxModule) {
            attach_thread
            env->DeleteGlobalRef(lynxModule);
            lynxModule = nullptr;
        }
    }

    Lynx_module &Lynx_module::operator=(jobject lynxModule) {
        if (this->lynxModule) {
            attach_thread
            env->DeleteGlobalRef(this->lynxModule);
        }
        this->lynxModule = lynxModule;
        return *this;
    }

    const char *
    Lynx_module::bulk_caching_mode_to_string(Lynx_module::Bulk_caching_mode bulk_caching_mode) {
        switch (bulk_caching_mode) {
            case Bulk_caching_mode::OFF:
                return "OFF";
            case Bulk_caching_mode::MANUAL:
                return "MANUAL";
            case Bulk_caching_mode::AUTO:
                return "AUTO";
            default:
                throw invalid_argument("Unimplemented item");
        }
    }

    bool Lynx_module::is_not_responding() const {
        attach_thread
        return env->CallBooleanMethod(lynxModule,
                                      env->GetMethodID(LynxModule, "isNotResponding", "()Z"));
    }

    void Lynx_module::set_bulk_caching_mode(Bulk_caching_mode bulk_caching_mode) const {
        attach_thread
        jobject bulkCachingMode = env->GetStaticObjectField(BulkCachingMode,
                                                            env->GetStaticFieldID(BulkCachingMode,
                                                                                  bulk_caching_mode_to_string(
                                                                                          bulk_caching_mode),
                                                                                  "Lcom/qualcomm/hardware/lynx/LynxModule$BulkCachingMode;"));
        env->CallVoidMethod(lynxModule, env->GetMethodID(LynxModule, "setBulkCachingMode",
                                                         "(Lcom/qualcomm/hardware/lynx/LynxModule$BulkCachingMode;)V"),
                            bulkCachingMode);
        env->DeleteLocalRef(bulkCachingMode);
    }

    void Lynx_module::get_bulk_data() const {
        attach_thread
        env->DeleteLocalRef(env->CallObjectMethod(lynxModule,
                                                  env->GetMethodID(LynxModule, "getBulkData",
                                                                   "()Lcom/qualcomm/hardware/lynx/LynxModule$BulkData;")));
    }

    double Lynx_module::get_input_voltage(Voltage_unit unit) const {
        attach_thread
        string unit_string = "NO_TYPE";
        switch (unit) {
            case Voltage_unit::MILLIVOLTS:
                unit_string = "MILLIVOLTS";
            case Voltage_unit::VOLTS:
                unit_string = "VOLTS";
        }

        jobject voltageUnit = env->GetStaticObjectField(VoltageUnit,
                                                        env->GetStaticFieldID(
                                                                VoltageUnit,
                                                                unit_string.c_str(),
                                                                "Lorg/firstinspires/ftc/robotcore/external/navigation/VoltageUnit;"));

        auto volt = static_cast<double>(env->CallDoubleMethod(lynxModule,
                                                              env->GetMethodID(
                                                                      LynxModule,
                                                                      "getInputVoltage",
                                                                      "(Lorg/firstinspires/ftc/robotcore/external/navigation/VoltageUnit;)D"),
                                                              voltageUnit));
        env->DeleteLocalRef(voltageUnit);
        return volt;
    }

} // sdk