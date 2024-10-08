#include "telemetry.h"

using namespace std;


namespace sdk::telemetry {
    jclass jclazz;
    jclass jclazz_DisplayFormat;
    jobject telemetry;

    // TODO: Try to get this to work with objects as well

    template<typename T>
    void add_data(const string &caption, const T &value) {
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

    template<>
    void add_data<bool>(const string &caption, const bool &value) {
        attach_thread
        jstring jcaption = env->NewStringUTF(caption.c_str());
        jclass Boolean = env->FindClass("java/lang/Boolean");
        jobject jvalue = env->NewObject(Boolean, env->GetMethodID(Boolean, "<init>", "(Z)V"),
                                        static_cast<jboolean>(value));
        env->DeleteLocalRef(env->CallObjectMethod(telemetry,
                                                  env->GetMethodID(jclazz, "addData",
                                                                   "(Ljava/lang/String;Ljava/lang/Object;)Lorg/firstinspires/ftc/robotcore/external/Telemetry$Item;"),
                                                  jcaption, jvalue));

        env->DeleteLocalRef(jcaption);
        env->DeleteLocalRef(Boolean);
        env->DeleteLocalRef(jvalue);
    }

    template void add_data<short>(const string &, const short &);

    template void add_data<int>(const string &, const int &);

    template void add_data<long>(const string &, const long &);

    template void add_data<long long>(const string &, const long long &);

    template void add_data<float>(const string &, const float &);

    template void add_data<double>(const string &, const double &);

    template void add_data<char>(const string &, const char &);

    void add_data(const string &caption, const char *value) {
        add_data(caption, string(value));
    }

    template void add_data<string>(const string &, const string &);

    template void add_data<unsigned short>(const string &, const unsigned short &);

    template void add_data<unsigned int>(const string &, const unsigned int &);

    template void add_data<unsigned long>(const string &, const unsigned long &);

    template void
    add_data<unsigned long long>(const string &, const unsigned long long &);

    template void
    add_data<unsigned char>(const string &, const unsigned char &);


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

    const char *display_format_to_string(const Display_format &display_format) {
        switch (display_format) {
            case Display_format::CLASSIC:
                return "CLASSIC";
            case Display_format::HTML:
                return "HTML";
            case Display_format::MONOSPACE:
                return "MONOSPACE";
            default:
                throw invalid_argument("Unimplemented item");
        }
    }

    void set_display_format(const Display_format &display_format) {
        attach_thread
        jobject displayFormat = env->GetStaticObjectField(jclazz_DisplayFormat,
                                                          env->GetStaticFieldID(
                                                                  jclazz_DisplayFormat,
                                                                  display_format_to_string(
                                                                          display_format),
                                                                  "Lorg/firstinspires/ftc/robotcore/external/Telemetry$DisplayFormat;"));
        env->CallVoidMethod(telemetry, env->GetMethodID(jclazz, "setDisplayFormat",
                                                        "(Lorg/firstinspires/ftc/robotcore/external/Telemetry$DisplayFormat;)V"),
                            displayFormat);
        env->DeleteLocalRef(displayFormat);
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

    void set_ms_transmission_interval(const int &ms_transmission_interval) {
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

    void set_retained(const bool &retained) {
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
// sdk