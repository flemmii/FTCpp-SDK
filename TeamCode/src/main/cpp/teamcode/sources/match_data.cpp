//
// Created by fnlg on 18.05.2024.
//

#include "match_data.hpp"

#include "robot.hpp"
#include <regex>

#include "telemetry.hpp"
#include "utils.hpp"

using namespace std;
using namespace sdk;

namespace match_data
{
    // ----------------- Only adjust this variables to set up autonomous for the match -----------------
    namespace all_matches
    {
        constexpr bool red_alliance[] = {true, false, true, false, false, true, true, false, true, true, true};
        constexpr bool start_right[] = {true, false, false, true, true, false, false, false, true, false, true};
        constexpr bool scoring_samples[] = {true, true, true, false, false, true, true, true, false, false, false};

        // Autonomous specifications

    } // all_matches

    // ----------------- DO NOT CHANGE ANYTHING BELOW THIS LINE -----------------
    namespace current_match
    {
        bool red_alliance = true;
        bool start_right = false;

        // Autonomous specifications

        // Do not access this before get_match_number() was called
        int match_number = 0;

        int get_match_number()
        {
            // Get the Logcat output
            FILE *logcat = popen("logcat -d -s Robocol:V | grep 'CMD_SET_MATCH_NUMBER' | tail -n 1",
                                 "r");

            // Buffer to hold each line of the Logcat output
            char buffer[1024];

            // Regular expression to match the line with the match number
            const regex regex(R"(.*CMD_SET_MATCH_NUMBER\((\d+)\) (\d+))");
            smatch match;

            // The last match number found in the Logcat output
            int current_match_number = 0;

            // Read each line of the Logcat output
            if (fgets(buffer, sizeof(buffer), logcat))
            {
                string line(buffer);

                // If the line matches the regular expression, update the last match number
                if (regex_search(line, match, regex))
                {
                    current_match_number = stoi(match[2]);
                }
            }

            // Close the Logcat output
            pclose(logcat);

            // Return the last match number found in the Logcat output
            return current_match_number;
        }

        void replace(string &str, const string &from, const string &to)
        {
            string::size_type pos = 0;
            string::size_type found_pos;

            while ((found_pos = str.find(from, pos)) != std::string::npos)
            {
                str.replace(found_pos, from.length(), to);
                pos = found_pos + to.length();
            }
        }

        void replace_if_no_character_before_and_after(string &str, const string &from,
                                                      const string &to)
        {
            string::size_type pos = 0;
            string::size_type found_pos;

            while ((found_pos = str.find(from, pos)) != std::string::npos)
            {
                const bool before_is_not_letter =
                    (found_pos == 0) || !std::isalpha(str[found_pos - 1]);
                const bool after_is_not_letter =
                    (found_pos == str.length() - 1) || !std::isalpha(str[found_pos + 1]);

                if (before_is_not_letter && after_is_not_letter)
                {
                    str.replace(found_pos, 1, to);
                    pos = found_pos + to.length();
                }
                else
                {
                    pos = found_pos + 1; // Move to the next character
                }
            }
        }

        void setup_variables_for_match()
        {
            match_number = get_match_number();

            const int red_alliance_size =
                sizeof(all_matches::red_alliance) / sizeof(all_matches::red_alliance[0]);
            const int start_right_size = sizeof(all_matches::start_right) /
                                         sizeof(all_matches::start_right[0]);

            if (red_alliance_size <= match_number)
            {
                log(ANDROID_LOG_ERROR, "match data") << "Match number is out of bounds for red alliance";
                return;
            }

            if (start_right_size <= match_number)
            {
                log(ANDROID_LOG_ERROR, "match data") << "Match number is out of bounds for start right";
                return;
            }

            red_alliance = all_matches::red_alliance[match_number];
            start_right = all_matches::start_right[match_number];
            robot::scoring_samples = all_matches::scoring_samples[match_number];

            string log_message = "Match number: " + to_string(match_number) + ", Alliance:";
            if (red_alliance)
                log_message += "Red, ";
            else
                log_message += "Blue, ";

            log_message += "starting in the ";
            if (start_right)
                log_message += "right position";
            else
                log_message += "left position";

            log(ANDROID_LOG_INFO, "match data") << log_message;

            // Autonomous specifications

            /*
            field preset:

            "<tt>"
            "┌-------------------------------┐\n"
            "│ ○ //     -         -     │    │\n"
            "│--//      -         -     \\   │\n"
            "│          -         -       ---│\n"
            "│          /¯¯¯¯¯¯¯¯¯\          │\n"
            "│          ┌---------┐          │\n"
            "│          │         │          │\n"
            "│          └---------┘          │\n"
            "│          \_________/          │\n"
            "│---       -         -          │\n"
            "│   \\     -         -      //--│\n"
            "│    │     -         -     // ○ │\n"
            "└-------------------------------┘"
            "</tt>";


            field preset with red and blue colors:

            "<tt>"
            "┌-------------------------------┐\n"
            "│ b○ //e     -         r-     │e    │\n"
            "│b--//e      -         r-     \\e   │\n"
            "│          -         r-       ---e│\n"
            "│          /¯¯¯¯¯¯¯¯¯\\          │\n"
            "│          ┌b-e---b-e---b-e┐          │\n"
            "│          b│e         r│e          │\n"
            "│          └r-e---r-e---r-e┘          │\n"
            "│          \\_________/          │\n"
            "│b---       -e         -          │\n"
            "│   b\\\\     -e         -      r//--e│\n"
            "│    b│     -e         -     r// ○e │\n"
            "└-------------------------------┘"
            "</tt>";


            red alliance, start right position preset:
            "<tt>"
            "┌-------------------------------┐\n"
            "│ ○ //     -         -     │    │\n"
            "│--//      -         -     \\   │\n"
            "│          -         -       ---│\n"
            "│          /¯¯¯¯¯¯¯¯¯\   ┌-----┐│\n"
            "│          ┌---------┐   │     ││\n"
            "│          │         │   └-----┘│\n"
            "│          └---------┘          │\n"
            "│          \_________/          │\n"
            "│---       -         -          │\n"
            "│   \\     -         -      //--│\n"
            "│    │     -         -     // ○ │\n"
            "└-------------------------------┘"
            "</tt>";



            red alliance, start left position preset:
            "<tt>"
            "┌-------------------------------┐\n"
            "│ ○ //     -         -     │    │\n"
            "│--//      -         -     \\   │\n"
            "│          -         -       ---│\n"
            "│          /¯¯¯¯¯¯¯¯¯\          │\n"
            "│          ┌---------┐          │\n"
            "│          │         │   ┌-----┐│\n"
            "│          └---------┘   │     ││\n"
            "│          \_________/   └-----┘│\n"
            "│---       -         -          │\n"
            "│   \\     -         -      //--│\n"
            "│    │     -         -     // ○ │\n"
            "└-------------------------------┘"
            "</tt>";



            blue alliance, start right position preset:
            "<tt>"
            "┌-------------------------------┐\n"
            "│ ○ //     -         -     │    │\n"
            "│--//      -         -     \\   │\n"
            "│          -         -       ---│\n"
            "│          /¯¯¯¯¯¯¯¯¯\          │\n"
            "│          ┌---------┐          │\n"
            "│┌-----┐   │         │          │\n"
            "││     │   └---------┘          │\n"
            "│└-----┘   \_________/          │\n"
            "│---       -         -          │\n"
            "│   \\     -         -      //--│\n"
            "│    │     -         -     // ○ │\n"
            "└-------------------------------┘"
            "</tt>";



            blue alliance, start left
            "<tt>"
            "┌-------------------------------┐\n"
            "│ ○ //     -         -     │    │\n"
            "│--//      -         -     \\   │\n"
            "│          -         -       ---│\n"
            "│┌-----┐   /¯¯¯¯¯¯¯¯¯\          │\n"
            "││     │   ┌---------┐          │\n"
            "│└-----┘   │         │          │\n"
            "│          └---------┘          │\n"
            "│          \_________/          │\n"
            "│---       -         -          │\n"
            "│   \\     -         -      //--│\n"
            "│    │     -         -     // ○ │\n"
            "└-------------------------------┘"
            "</tt>";

            position preset:
             ┌-----┐
             │     │
             └-----┘
             */

            string field;
            if (red_alliance && start_right)
            {
                field = "<tt>"
                        "┌-------------------------------┐\n"
                        "│ b○ //e     -         r-     │e    │\n"
                        "│b--//e      -         r-     \\\\e   │\n"
                        "│          -         r-       ---e│\n"
                        "│          /¯¯¯¯¯¯¯¯¯\\   r┌-----┐e│\n"
                        "│          ┌b-e---b-e---b-e┐   r│     │e│\n"
                        "│          b│e         r│e   r└-----┘e│\n"
                        "│          └r-e---r-e---r-e┘          │\n"
                        "│          \\_________/          │\n"
                        "│b---       -e         -          │\n"
                        "│   b\\\\     -e         -      r//--e│\n"
                        "│    b│     -e         -     r// ○e │\n"
                        "└-------------------------------┘"
                        "</tt>";
            }
            else if (red_alliance && !start_right)
            {
                field = "<tt>"
                        "┌-------------------------------┐\n"
                        "│ b○ //e     -         r-     │e    │\n"
                        "│b--//e      -         r-     \\e   │\n"
                        "│          -         r-       ---e│\n"
                        "│          /¯¯¯¯¯¯¯¯¯\\          │\n"
                        "│          ┌b-e---b-e---b-e┐          │\n"
                        "│          b│e         r│e   r┌-----┐e│\n"
                        "│          └r-e---r-e---r-e┘   r│     │e│\n"
                        "│          \\_________/   r└-----┘e│\n"
                        "│b---       -e         -          │\n"
                        "│   b\\\\     -e         -      r//--e│\n"
                        "│    b│     -e         -     r// ○e │\n"
                        "└-------------------------------┘"
                        "</tt>";
            }
            else if (!red_alliance && start_right)
            {
                field = "<tt>"
                        "┌-------------------------------┐\n"
                        "│ b○ //e     -         r-     │e    │\n"
                        "│b--//e      -         r-     \\e   │\n"
                        "│          -         r-       ---e│\n"
                        "│          /¯¯¯¯¯¯¯¯¯\\          │\n"
                        "│          ┌b-e---b-e---b-e┐          │\n"
                        "│b┌-----┐e   b│e         r│e          │\n"
                        "│b│     │e   └r-e---r-e---r-e┘          │\n"
                        "│b└-----┘e   \\_________/          │\n"
                        "│b---       -e         -          │\n"
                        "│   b\\\\     -e         -      r//--e│\n"
                        "│    b│     -e         -     r// ○e │\n"
                        "└-------------------------------┘"
                        "</tt>";
            }
            else
            {
                field = "<tt>"
                        "┌-------------------------------┐\n"
                        "│ b○ //e     -         r-     │e    │\n"
                        "│b--//e      -         r-     \\e   │\n"
                        "│          -         r-       ---e│\n"
                        "│b┌-----┐e   /¯¯¯¯¯¯¯¯¯\\          │\n"
                        "│b│     │e   ┌b-e---b-e---b-e┐          │\n"
                        "│b└-----┘e   b│e         r│e          │\n"
                        "│          └r-e---r-e---r-e┘          │\n"
                        "│          \\_________/          │\n"
                        "│b---       -e         -          │\n"
                        "│   b\\\\     -e         -      r//--e│\n"
                        "│    b│     -e         -     r// ○e │\n"
                        "└-------------------------------┘"
                        "</tt>";
            }

            string::size_type pos = 0;

            // Replace spaces with &nbsp; NEEDS TO BE DONE FIRST
            while ((pos = field.find(' ', pos)) != string::npos)
            {
                field.replace(pos, 1, "&nbsp;");
                pos += 6; // length of "&nbsp;"
            }

            // Replace 'e' with the end tag for red or blue color, ensuring no letters before or after
            replace_if_no_character_before_and_after(field, "e", "</span>");

            // Replace 'r' with the start tag for red color, ensuring no letters before or after
            replace_if_no_character_before_and_after(field, "r",
                                                     "<span style='color: red;'>");

            // Replace 'b' with the start tag for blue color, ensuring no letters before or after
            replace_if_no_character_before_and_after(field, "b",
                                                     "<span style='color: blue;'>");

            telemetry::add_line(field);
        }
    } // this_match
} // match_data