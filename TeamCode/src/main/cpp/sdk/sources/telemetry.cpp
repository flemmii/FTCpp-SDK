#include "telemetry.h"

using namespace std;

namespace sdk {
    namespace telemetry {
        jclass jclazz;
        jobject telemetry;

        // TODO: Try to get this to work with objects as well

        template<typename T>
        void add_data(const string &caption, T value) {
            attach_thread
            ostringstream os;
            os << value;
            jstring jcaption = env->NewStringUTF(caption.c_str());
            jstring jvalue = env->NewStringUTF(os.str().c_str());
            env->DeleteLocalRef(env->CallObjectMethod(telemetry,
                                                      env->GetMethodID(jclazz, "addData",
                                                                       "(Ljava/lang/String;Ljava/lang/Object;)Lorg/firstinspires/ftc/robotcore/external/Telemetry$Item;"),
                                                      jcaption, jvalue));
            env->DeleteLocalRef(jcaption);
            env->DeleteLocalRef(jvalue);
        }

        template void add_data<bool>(const string &, bool);

        template void add_data<short>(const string &, short);

        template void add_data<int>(const string &, int);

        template void add_data<long>(const string &, long);

        template void add_data<long long>(const string &, long long);

        template void add_data<float>(const string &, float);

        template void add_data<double>(const string &, double);

        template void add_data<char>(const string &, char);

        template void add_data<const char *>(const string &, const char *);

        template void add_data<string>(const string &, string);

        template void add_data<unsigned short>(const string &, unsigned short);

        template void add_data<unsigned int>(const string &, unsigned int);

        template void add_data<unsigned long>(const string &, unsigned long);

        template void
        add_data<unsigned long long>(const string &, unsigned long long);

        template void
        add_data<unsigned char>(const string &, unsigned char);


        void add_line(const string &line_caption) {
            attach_thread
            jstring jline_caption = env->NewStringUTF(line_caption.c_str());
            env->DeleteLocalRef(env->CallObjectMethod(telemetry,
                                                      env->GetMethodID(jclazz, "addLine",
                                                                       "(Ljava/lang/String;)Lorg/firstinspires/ftc/robotcore/external/Telemetry$Line;"),
                                                      jline_caption));
            env->DeleteLocalRef(jline_caption);
        }

        bool update() {
            attach_thread
            return env->CallBooleanMethod(telemetry, env->GetMethodID(jclazz, "update", "()Z"));
        }

        void set_auto_clear(const bool &auto_clear) {
            attach_thread
            jboolean jbool = auto_clear;
            env->DeleteLocalRef(env->CallObjectMethod(telemetry,
                                                      env->GetMethodID(jclazz, "setAutoClear",
                                                                       "(Z;)V"), jbool));
        }

        void clear() {
            attach_thread
            env->CallVoidMethod(telemetry, env->GetMethodID(jclazz, "clear", "()V"));
        }

        void clear_all() {
            attach_thread
            env->CallVoidMethod(telemetry, env->GetMethodID(jclazz, "clear_all", "()V"));
        }

        void speak(const string &text, const string &language_code, const string &country_code) {
            attach_thread
            jstring jtext = env->NewStringUTF(text.c_str());
            jstring jlanguage_code = env->NewStringUTF(language_code.c_str());
            jstring jcountry_code = env->NewStringUTF(country_code.c_str());
            env->CallVoidMethod(telemetry, env->GetMethodID(jclazz, "speak",
                                                            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"),
                                jtext, jlanguage_code, jcountry_code);
            env->DeleteLocalRef(jlanguage_code);
            env->DeleteLocalRef(jcountry_code);
            env->DeleteLocalRef(jtext);
        }

        void speak(const string &text) {
            attach_thread
            jstring jtext = env->NewStringUTF(text.c_str());
            env->CallVoidMethod(telemetry,
                                env->GetMethodID(jclazz, "speak", "(Ljava/lang/String;)V"),
                                jtext);
            env->DeleteLocalRef(jtext);
        }

        string get_caption() {
            attach_thread
            auto jstr = (jstring) (env->CallObjectMethod(telemetry,
                                                         env->GetMethodID(jclazz,
                                                                          "getCaption",
                                                                          "()Ljava/lang/String;")));

            const char *utf_chars = env->GetStringUTFChars(jstr, nullptr);
            std::string caption(utf_chars);
            env->DeleteLocalRef(jstr);
            return caption;
        }

        void set_caption(const string &caption) {
            attach_thread
            jstring jcaption = env->NewStringUTF(caption.c_str());
            env->DeleteLocalRef(env->CallObjectMethod(telemetry,
                                                      env->GetMethodID(jclazz, "setCaption",
                                                                       "(Ljava/lang/String;)Lorg/firstinspires/ftc/robotcore/external/Telemetry$Line;"),
                                                      jcaption));
            env->DeleteLocalRef(jcaption);
        }

        void set_display_format(
                display_format display_format) { //TODO: Test (HIGH RISK OF NOT FUNCTIONING) (This doesnt work (Florian))
            attach_thread
            env->CallVoidMethod(telemetry, env->GetMethodID(jclazz, "setDisplayFormat",
                                                            "(Ljava/lang/String;)Lorg/firstinspires/ftc/robotcore/external/Telemetry$DisplayFormat;)V"),
                                display_format);
        }

        string get_item_separator() {
            attach_thread
            jboolean isCopy = JNI_FALSE;
            auto jstr = (jstring) (env->CallObjectMethod(telemetry,
                                                         env->GetMethodID(jclazz,
                                                                          "getItemSeparator",
                                                                          "()Ljava/lang/String;")));
            const char *utf_chars = env->GetStringUTFChars(jstr, &isCopy);
            std::string item_separator(utf_chars);
            env->DeleteLocalRef(jstr);
            return item_separator;
        }

        void set_item_separator(const string &item_separator) {
            attach_thread
            jstring jitem_separator = env->NewStringUTF(item_separator.c_str());
            env->DeleteLocalRef(env->CallObjectMethod(telemetry, env->GetMethodID(jclazz,
                                                                                  "setItemSeparator",
                                                                                  "(Ljava/lang/String;)V"),
                                                      jitem_separator));
            env->DeleteLocalRef(jitem_separator);
        }

        string get_caption_value_separator() {
            attach_thread
            jboolean isCopy = JNI_FALSE;
            auto jstr = (jstring) (env->CallObjectMethod(telemetry,
                                                         env->GetMethodID(jclazz,
                                                                          "getCaptionValueSeparator",
                                                                          "()Ljava/lang/String;")));
            const char *utf_chars = env->GetStringUTFChars(jstr, &isCopy);
            std::string caption_value_separator(utf_chars);
            env->DeleteLocalRef(jstr);
            return caption_value_separator;
        }

        void set_caption_value_separator(const string &caption_value_separator) {
            attach_thread
            jstring jcaption_value_separator = env->NewStringUTF(caption_value_separator.c_str());
            env->DeleteLocalRef(env->CallObjectMethod(telemetry, env->GetMethodID(jclazz,
                                                                                  "setCaptionValueSeparator",
                                                                                  "(Ljava/lang/String;)V"),
                                                      jcaption_value_separator));
            env->DeleteLocalRef(jcaption_value_separator);
        }

        bool is_auto_clear() {
            attach_thread
            return env->CallBooleanMethod(telemetry,
                                          env->GetMethodID(jclazz, "isAutoClear", "()Z"));
        }

        void set_ms_transmission_interval(int ms_transmission_interval) {
            attach_thread
            jint jms_transmission_interval = static_cast<jint>(ms_transmission_interval);
            env->CallVoidMethod(telemetry,
                                env->GetMethodID(jclazz, "setMsTransmissionInterval", "(I;)V"),
                                jms_transmission_interval);
        }

        int get_ms_transmission_interval() {
            attach_thread
            return static_cast<int>(env->CallIntMethod(telemetry, env->GetMethodID(jclazz,
                                                                                   "getMsTransmissionInterval",
                                                                                   "()I")));
        }

        void set_retained(bool retained) {
            attach_thread
            env->DeleteLocalRef(env->CallObjectMethod(telemetry,
                                                      env->GetMethodID(jclazz, "setRetained",
                                                                       "(Z;)Lorg/firstinspires/ftc/robotcore/external/Telemetry$Item;"),
                                                      retained));
        }

        bool is_retained() {
            attach_thread
            return static_cast<bool>(env->CallBooleanMethod(telemetry, env->GetMethodID(jclazz,
                                                                                        "isRetained",
                                                                                        "()Z")));
        }

    }
} // sdk