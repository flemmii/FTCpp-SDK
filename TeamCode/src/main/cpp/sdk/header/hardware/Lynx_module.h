//
// Created by fnlg on 11.01.2024.
//

#ifndef FTCROBOTCONTROLLER_LYNX_MODULE_H
#define FTCROBOTCONTROLLER_LYNX_MODULE_H

#include <string>
#include "navigation/Voltage_unit.h"
#include "utils.h"

namespace sdk {
    class Lynx_module {
        // TODO: This is not complete
    public:
        static jclass jclazz;
        static jclass jclazz_BulkCachingMode;
        jobject lynxModule{};
        enum class Bulk_caching_mode {
            OFF,
            MANUAL,
            AUTO
        };

        Lynx_module() = default;

        Lynx_module(const jobject &lynxModule);

        ~Lynx_module();

        Lynx_module &operator=(const jobject &lynxModule);

        static const char *bulk_caching_mode_to_string(const Bulk_caching_mode &bulk_caching_mode);

        [[nodiscard]] bool is_not_responding() const;

        void set_bulk_caching_mode(const Bulk_caching_mode &bulk_caching_mode) const;

        void get_bulk_data() const;

        [[nodiscard]] double get_input_voltage(const Voltage_unit &unit) const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_LYNX_MODULE_H
