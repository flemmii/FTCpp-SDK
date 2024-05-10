#include "../header/last_position_storage.h"

namespace last_position_storage {
    const char *const filename_last_position_storage = "/sdcard/positionStorage.txt";
    high_resolution_clock::time_point startTime;
    const map<string, int> dataLineManager = {{"position_x",        3},
                                              {"position_y",        4},
                                              {"position_rotation", 5},
                                              {"extra_degrees",     6},
                                              {"store_time",        7}};

    std::fstream &GotoLine(std::fstream &file, int num) {
        file.seekg(std::ios::beg);
        for (int i = 0; i < num; i++) {
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        return file;
    }

    string get_line(int line_number) {
        string ans;
        fstream file(filename_last_position_storage);
        GotoLine(file, line_number);
        file >> ans;
        file.close();
        return ans;
    }

    string get_full_data() {
        string ans;
        fstream file(filename_last_position_storage);
        file >> ans;
        file.close();
        return ans;
    }

    void init(bool is_autonomous) {
        startTime = high_resolution_clock::now();
        ofstream positionWriter;
        positionWriter.open(filename_last_position_storage, ios::app);
        if (is_autonomous) {
            positionWriter.close();
            positionWriter.open(filename_last_position_storage, ios::out | ios::trunc);
            if (positionWriter.is_open()) {
                positionWriter << "-------------POSITION STORAGE-------------" << endl;
                positionWriter << "-------------AUTONOMOUS-------------" << endl;
                positionWriter.flush();
            }
        } else {
            // Do nothing special for non-autonomous mode
        }
        positionWriter.close();
    }

    void store_data(Position position, int extra_degrees) {
        ofstream positionWriter;
        positionWriter.open(filename_last_position_storage, ios::app);
        positionWriter << "t" << endl
                       << to_string(position.x) << endl << to_string(position.y) << endl
                       << to_string(position.rotation) << endl
                       << extra_degrees << endl;
        int current_time = (int) duration_cast<milliseconds>(
                high_resolution_clock::now() - startTime).count();
        positionWriter << to_string(current_time);
        positionWriter.flush();
    }

    double get_data(const string &data_id) {
        return std::stod(get_line(dataLineManager.at(data_id)));
    }

    bool valid_data_available() {
        string saved;
        saved = get_line(2);
        char *t = reinterpret_cast<char *>('t');
        if (saved.size() != 1) {
            for (char x: saved) {
                if (x == *t) {
                    return true;
                }
            }
        }
        return saved == "t";
    }

    Position get_last_position() {
        Position position{
                get_data("position_x"),
                get_data("position_y"),
                get_data("position_rotation")
        };
        return position;
    }

    double get_last_extra_degrees() {
        double extraDegrees = get_data("extra_degrees");
        return extraDegrees;
    }
}
