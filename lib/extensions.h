#ifndef EXTENSIONS_H
#define EXTENSIONS_H

/* STD */
#include <iostream>
#include <vector>
#include <vector>
#include <iterator>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <utility>
#include <iomanip>
#include <functional>
#include <algorithm>

/* LIB */
#include "macros.h"
#include "data.h"
#include "utilities.h"
#include "exercise.hpp"
#include "category.hpp"

/* C */
#include <Windows.h> 
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <string.h>

using std::cout; 
using std::endl;
using std::string;
using std::vector;
using std::map;

#define PUSH_EXTENSION(seg, func) std::pair<std::string, void(*)(void)>(seg, func)
#define EXTENSION void

extern vector<Exercise> Exercises;
extern Exercise* current_exercise;
extern map<std::string, std::string> Variables;
extern Category living_category;
extern HANDLE hConsole;

extern string FetchDomRaw (string);
extern int FetchDomInt (string);

/* POST COMPILATION EXTENSIONS */

EXTENSION reverse_exercises () {
    if (DETERMINE_VALUE("REVERSE", FetchDomInt)) {
        std::reverse(Exercises.begin(), Exercises.end());
    }
}

EXTENSION handle_start () {
    Exercises.erase(Exercises.begin(), Exercises.begin() + (DETERMINE_VALUE("START", FetchDomInt)));
}

EXTENSION on_mood () {
    if (DETERMINE_VALUE("LOW_MOOD", FetchDomInt)) {
        for (auto& exer : Exercises) {
            exer.reps = 12;
        }
    }
}

EXTENSION handle_offsets () {
    for (auto& exer : Exercises) {
        exer.sets += (DETERMINE_VALUE("SETS_OFFSET", FetchDomInt));
        exer.reps += (DETERMINE_VALUE("REPS_OFFSET", FetchDomInt));
        exer.hold += (DETERMINE_VALUE("HOLD_OFFSET", FetchDomInt));
        exer.ahold += (DETERMINE_VALUE("AHOLD_OFFSET", FetchDomInt));
    }
}

EXTENSION init () {
    cout << (DETERMINE_VALUE("DISPLAY", FetchDomInt)) << endl;
}

/* POST START SCREEN EXTENSIONS */

EXTENSION calculate_total_session_time () {
    // This extension calculates the total amount of time it takes to continue an exercise session
    FOREGROUND_COLOR(13);
    double total_time = 0.00;
    for (auto& exer : Exercises) {
        total_time += exer.CalculateTime();
    }
    total_time *= DETERMINE_VALUE("ROUNDS", FetchDomInt);
    cout << std::fixed;
    cout << std::setprecision(2);
    cout << "T: " << total_time << " minutes" << endl;
    RESET_COLORS();
}

EXTENSION calculate_total_session_reps () {
    // This extension calculates the total amount of reps in a session
    FOREGROUND_COLOR(6);
    int total_reps = 0;
    for (auto& exer : Exercises) {
        total_reps += exer.reps * exer.sets;
    }
    total_reps *= (DETERMINE_VALUE("ROUNDS", FetchDomInt));
    cout << "R: " << total_reps <<  endl;
    RESET_COLORS();
}

EXTENSION n_exercises () {
    FOREGROUND_COLOR(8);
    cout << "N: " << Exercises.size() << endl;
    RESET_COLORS();
}

EXTENSION display_variables () {
    int colors = 0;
    RESET_COLORS(); // Just in case, evaluate later
    if (living_category.Variables.size()) {
        cout << endl;
        for (auto it = living_category.Variables.begin(); it != living_category.Variables.end(); it++) {
            FOREGROUND_COLOR(colors);
            if (it->second != "") {
                cout << it->first << ": " << it->second << endl;    
            }
            colors++;
        }
        RESET_COLORS(); // Just in case, evaluate later
    }
}

EXTENSION display_info () {
    Log("Hello from display_info", 6);
}

/* POST EXERCISE EXTENSIONS */

EXTENSION display_exercise_image () {
    // This extension displays a visual representation of the current exercise at hand
    if ((DETERMINE_VALUE("DISPLAY", FetchDomInt)) and current_exercise->tags.find("NO_DISPLAY") == std::string::npos) {
        string png (current_exercise->name);
        png.append(".png");
        string cmd = "start " + string(FILE_PNGS_PATH) + string(png) + " && timeout 4 && taskkill /IM Microsoft.Photos.exe /F";
        system(cmd.c_str());
        system("cls");
    }
    if (current_exercise->tags.find("DISPLAY") != std::string::npos) {
        string png (current_exercise->name);
        png.append(".png");
        string cmd = "start " + string(FILE_PNGS_PATH) + string(png) + " && timeout 4 && taskkill /IM Microsoft.Photos.exe /F";
        system(cmd.c_str());
        system("cls");
    }
}

EXTENSION on_focus () {
    if (current_exercise->tags.find("F") != string::npos) {
        Exercises.erase(Exercises.begin() + current_exercise->pos, Exercises.end());
    }
}

/* POST ROUND */

EXTENSION handle_decrease () {
    int rounds = (DETERMINE_VALUE("ROUNDS", FetchDomInt));
    int decrease = (DETERMINE_VALUE("DECREASE", FetchDomInt));

    if (rounds > 1 and decrease) {
        for (auto& exer : Exercises) {
            exer.reps -= decrease;
        }
    }
}


EXTENSION handle_trim () {
    if ((DETERMINE_VALUE("TRIM", FetchDomInt))) {
        Exercises.erase(Exercises.begin(), Exercises.begin() + (DETERMINE_VALUE("TRIM", FetchDomInt)));
    }
}

// EXTENSION multi_round_session () {
//     if ((DETERMINE_VALUE("ROUNDS", FetchDomInt)) > 1 and current_exercise->tags.find("NO_NEG") == std::string::npos) {
//         for (auto& exer : Exercises) {
//             exer.reps -= (DETERMINE_VALUE("NEG", FetchDomInt));
//         }
//         std::reverse(Exercises.begin(), Exercises.end());
//     }
// }

vector<std::pair<string, void(*)(void)>> Extensions = {
    //// POST COMPILATION ////
    PUSH_EXTENSION("post_compilation", &reverse_exercises),
    PUSH_EXTENSION("post_compilation", &handle_start),
    PUSH_EXTENSION("post_compilation", &on_mood),
    PUSH_EXTENSION("post_compilation", &handle_offsets),
    //PUSH_EXTENSION("post_compilation", &init),
    //// POST START SCREEN ////
    PUSH_EXTENSION("post_start_screen", &calculate_total_session_time),
    PUSH_EXTENSION("post_start_screen", &calculate_total_session_reps),
    PUSH_EXTENSION("post_start_screen", &n_exercises),
    PUSH_EXTENSION("post_start_screen", &display_variables),
    //// POST EXERCISE ////
    PUSH_EXTENSION("post_exercise", &display_exercise_image),
    PUSH_EXTENSION("post_exercise", &on_focus),
    //// POST ROUND END ////
    PUSH_EXTENSION("round_end", &handle_decrease),
    PUSH_EXTENSION("round_end", &handle_trim)   
};

void compile_extensions (string group) {
    for (auto& p :Extensions) {
        if (p.first == group) {
            p.second();
            FOREGROUND_COLOR(3);
        }
    }
}


#endif
