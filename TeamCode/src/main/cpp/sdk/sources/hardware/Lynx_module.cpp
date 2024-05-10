//
// Created by fnlg on 11.01.2024.
//

#include "hardware/Lynx_module.h"

using namespace std;

namespace sdk {
    jclass LynxModule;
    jclass BulkCachingMode;

    Lynx_module::Lynx_module() = default;

    Lynx_module::Lynx_module(jobject lynxModule) {
        attach_thread
        this->lynxModule = env->NewGlobalRef(lynxModule);
    }

    Lynx_module::~Lynx_module() {
        if (lynxModule) {
            attach_thread
            env->DeleteGlobalRef(lynxModule);
        }
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

    double Lynx_module::get_input_voltage(voltage_unit::voltage_unit_type unit) const {
        attach_thread
        string unit_string = "NO_TYPE";
        switch (unit) {
            case voltage_unit::voltage_unit_type::MILLIVOLTS:
                unit_string = "MILLIVOLTS";
            case sdk::voltage_unit::voltage_unit_type::VOLTS:
                unit_string = "VOLTS";
        }

        jobject voltageUnit = env->GetStaticObjectField(voltage_unit::VoltageUnit,
                                                        env->GetStaticFieldID(
                                                                voltage_unit::VoltageUnit,
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