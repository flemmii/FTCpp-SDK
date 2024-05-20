//
// Created by fnlg on 11.01.2024.
//

#ifndef FTCROBOTCONTROLLER_LYNX_MODULE_H
#define FTCROBOTCONTROLLER_LYNX_MODULE_H

#include <string>
#include "navigation/Voltage_unit.h"
#include "utils.h"

namespace sdk {
    extern jclass LynxModule;
    extern jclass BulkCachingMode;

    class Lynx_module {
        // TODO: This is not complete
    public:
        jobject lynxModule{};
        enum Bulk_caching_mode {
            OFF,
            MANUAL,
            AUTO
        };

        Lynx_module() = default;

        Lynx_module(jobject lynxModule);

        ~Lynx_module();

        Lynx_module &operator=(jobject lynxModule);

        static const char *bulk_caching_mode_to_string(Bulk_caching_mode bulk_caching_mode);

        [[nodiscard]] bool is_not_responding() const;

        void set_bulk_caching_mode(Bulk_caching_mode bulk_caching_mode) const;

        void get_bulk_data() const;

        [[nodiscard]] double get_input_voltage(Voltage_unit unit) const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_LYNX_MODULE_H
