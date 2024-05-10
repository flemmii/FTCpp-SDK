#ifndef FTC_FROG_LAST_POSITION_STORAGE_H
#define FTC_FROG_LAST_POSITION_STORAGE_H

#include <fstream>
#include <string>
#include <map>

#include "Position.h"

using namespace std;
using namespace chrono;
namespace last_position_storage {
    void init(bool is_autonomous);

    void store_data(Position position, int extra_degrees);

    bool valid_data_available();

    Position get_last_position();

    double get_last_extra_degrees();

    string get_line(int line_number); //comment out since usually unnecessary
}
#endif //FTC_FROG_LAST_POSITION_STORAGE_H
