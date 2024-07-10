#ifndef FTCROBOTCONTROLLER_TELEMETRY_H
#define FTCROBOTCONTROLLER_TELEMETRY_H

#include <string>
#include <type_traits>
#include <sstream>

#include "utils.h"

namespace sdk {
    extern jclass Telemetry;
    namespace telemetry {
        extern jobject telemetry;
        using namespace std;
        enum class display_format {
            CLASSIC,
            MONOSPACE,
            HTML
        };

        template<typename T, typename = void>
        struct has_to_string : std::false_type {
        };

        template<typename T>
        struct has_to_string<T, std::void_t<decltype(std::declval<T>().to_string())>>
                : std::true_type {
        };

        template<typename T>
        void add_data(const std::string &caption, T value);

        void add_line(const string &line_caption);

        void set_auto_clear(const bool &auto_clear);

        bool update();

        void clear();

        void clear_all();

        void speak(const string &text, const string &language_code, const string &country_code);

        void speak(const string &text);


        string get_caption();

        void set_caption(const string &caption);

        void set_display_format(display_format display_format);

        //TODO: FInd a way to implement setValue (because of args)

        string get_item_separator();

        void set_item_separator(const string &item_separator);

        void set_caption_value_separator(const string &caption_value_separator);

        string get_caption_value_separator();

        bool is_auto_clear();

        int get_ms_transmission_interval();

        void set_ms_transmission_interval(int ms_transmission_interval);

        void set_retained(bool retained);

        bool is_retained();

    }
} // sdk

#endif //FTCROBOTCONTROLLER_TELEMETRY_H
