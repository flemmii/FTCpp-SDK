#include "subsystems/driving/last_position_storage.hpp"

#include <sys/stat.h>
#include <fstream>

#include "json/single_include/nlohmann/json.hpp"
#include "utils.hpp"

using namespace std;
using namespace chrono;
using json = nlohmann::json;

namespace last_position_storage
{
    const Second MAX_TIME_FOR_VALID_DATA = 1_min;

    tm get_compile_time()
    {
        tm compile_time = {};
        stringstream ss;
        ss << __DATE__ << " " << __TIME__;
        ss >> get_time(&compile_time, "%b %d %Y %H:%M:%S");
        compile_time.tm_isdst = -1; // Ensure daylight saving time is not considered
        return compile_time;
    }

    Second get_seconds_since_compile()
    {
        // Kompilierungszeitpunkt
        tm compile_time = get_compile_time();
        const time_t compile_time_t = mktime(&compile_time);

        // Aktueller Zeitpunkt
        const time_t current_time_t = system_clock::to_time_t(system_clock::now());

        // Berechne die verstrichene Zeit seit dem Kompilierungszeitpunkt
        return Second(difftime(current_time_t, compile_time_t));
    }

    void create_directory_if_not_exists(const string &path)
    {
        struct stat info{};
        if (stat(path.c_str(), &info) != 0)
        {
            // Ordner existiert nicht, erstelle ihn
            if (mkdir(path.c_str(), 0777) != 0)
            {
                // Fehler: Konnte den Ordner nicht erstellen
                return;
            }
        }
        else if (!(info.st_mode & S_IFDIR))
        {
            // Fehler: Der Pfad ist kein Ordner
            return;
        }
    }

    void store_position(Position<true, Millimetre, Degree> position)
    {
        log(ANDROID_LOG_INFO, "last_position_storage") << "Storing position x: " << position.x << " y: " << position.y << " rotation: " << position.rotation << endl;

        // Berechne die verstrichene Zeit seit dem Kompilierungszeitpunkt
        const Second seconds_since_compile = get_seconds_since_compile();

        json data;
        data["position_x"] = position.x.Value();
        data["position_y"] = position.y.Value();
        data["position_rotation"] = position.rotation.Value();
        data["store_time"] = seconds_since_compile.Value();

        string directory_path = "/sdcard/code_generated_files";
        string file_path = directory_path + "/position_storage.json";

        // Erstelle den Ordner, falls er nicht existiert
        create_directory_if_not_exists(directory_path);

        ofstream file(file_path, ios::out | ios::trunc);
        if (file.is_open())
        {
            file << data.dump(4);
            file.close();
        }
    }

    bool is_data_valid()
    {
        ifstream file("/sdcard/code_generated_files/position_storage.json");
        if (!file.is_open())
        {
            log(ANDROID_LOG_WARN, "last_position_storage") << "No stored position data found" << endl;
            return false;
        }

        json data;
        file >> data;
        file.close();

        if (!data.contains("store_time"))
        {
            log(ANDROID_LOG_WARN, "last_position_storage") << "No store time found in stored position data" << endl;
            return false;
        }

        const Second store_time(data["store_time"].get<double>());
        const Second seconds_since_store = get_seconds_since_compile() - store_time;

        if (!data.contains("position_x"))
        {
            log(ANDROID_LOG_WARN, "last_position_storage") << "No x position found in stored position data" << endl;
            return false;
        }

        if (!data.contains("position_y"))
        {
            log(ANDROID_LOG_WARN, "last_position_storage") << "No y position found in stored position data" << endl;
            return false;
        }

        if (!data.contains("position_rotation"))
        {
            log(ANDROID_LOG_WARN, "last_position_storage") << "No rotation found in stored position data" << endl;
            return false;
        }

        if (seconds_since_store >= MAX_TIME_FOR_VALID_DATA)
        {
            log(ANDROID_LOG_WARN, "last_position_storage") << "Stored position data is too old" << endl;
            return false;
        }

        return true;
    }

    Position<true, Millimetre, Degree> get_stored_position()
    {
        ifstream file("/sdcard/code_generated_files/position_storage.json");
        if (!file.is_open())
            return {0_mm, 0_mm, 0_deg};

        json data;
        file >> data;
        file.close();

        const Position<true, Millimetre, Degree> position(Millimetre(data["position_x"].get<double>()),
                                                          Millimetre(data["position_y"].get<double>()),
                                                          Degree(data["position_rotation"].get<double>()));
        log(ANDROID_LOG_INFO, "last_position_storage") << "Retrieved position x: " << position.x << "S y: " << position.y << " rotation: " << position.rotation << endl;

        return position;
    }
}
