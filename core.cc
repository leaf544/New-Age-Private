 /* Written by leaf544 aka grandma */

/* STD */
#include <iostream>
#include <vector>
#include <vector>
#include <iterator>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <cctype>

/* LIB */
#include "lib/reader.hpp"
#include "lib/macros.h"
#include "lib/data.h"
#include "lib/utilities.h"
#include "lib/exercise.hpp"
#include "lib/extensions.h"
#include "lib/category.hpp"

/* C */
#include <Windows.h> 
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>

int run_iteration = 1;
std::vector<Exercise> Exercises;
Exercise* current_exercise = NULL;
Category living_category;
std::map<std::string, std::string> Variables;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

std::string FetchValue (std::string);
int FetchValueInt (std::string);
void bar (const char*, int, int, int);
void ClearScreen ();

/* DEFAULTS */

int DEFAULT_SETS = 0;
int DEFAULT_FREESTYLE = 0;
int DEFAULT_REPS = 0;
int DEFAULT_HOLD = 0;
int DEFAULT_AHOLD = 0;

using std::cout;
using std::endl;
using std::cin;
using std::string;

int main (void) {
    
    /* SETUP & PREREQUISITES */
    
    system("title NewAge");
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

    /* LOCATE .NEWAGE FILE */
    
    DIR* dir;
    struct dirent* ent;
    char cwd [PATH_MAX];
    
    getcwd(cwd, sizeof(cwd));
    char file_path [FILENAME_MAX];

    if ((dir = opendir(cwd)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (string(ent->d_name).find(".newage") != string::npos) {
                strcpy(file_path, ent->d_name);
            }
        }
        closedir(dir);
    } else {
        perror("");
        return -1;
    }
    
    /* COMPILE EXERCISES AND VARIABLES */
    
    std::ifstream file(file_path);
    string category = "";
    std::vector<string> values;
    
    /* PRE COMPILATION  */ 
    
    bool variables_compiled = false;
    std::vector<string> lines;    
    
    for (string line; std::getline(file, line);) {
        if (!line.size()) continue;
        lines.push_back(line);
    }    

    Reader<string> lines_reader;
    lines_reader.attatch(&lines);
    lines_reader.set(0);
    string current_line = *(lines_reader.at(lines_reader.i));

    while (true) {
        values = UTIL::split_string(current_line.substr(1, current_line.length()), " ");
        switch (current_line[0]) {
        case VARIABLE_MARKER: 
            Variables[values[1]] = values[2];
            break;
        case CATEGORY_MARKER:
            variables_compiled = true;
            category = values[0];
            
            if (category == FetchValue("CATEGORY") and values.size() > 1) {
                living_category.hasVariables = true;
                values.erase(values.begin());
                int p = 1;
                FLOOP (int, n, values.size() / 3) { living_category.Variables[values[p]] = values[p+1]; p+=3; }
            }
            break;
        case EXERCISE_MARKER:
            if (category == FetchValue("CATEGORY")) {
                Exercises.push_back(Exercise(
                                             values[0],
                                             (values.size() > 2) == false ? DEFAULT_FREESTYLE : values[2][0],
                                             (values.size() > 1) == false ? DEFAULT_SETS : CATOI(values[1]), 
                                             (values.size() > 3) == false ? DEFAULT_REPS : CATOI(values[3]), 
                                             (values.size() > 4) == false ? DEFAULT_HOLD : CATOI(values[4]), 
                                             (values.size() > 5) == false ? DEFAULT_AHOLD : CATOI(values[5])));

                if (values.size() > 6) {
                    FLOOPS (int, i, 6, values.size()) {
                        Exercises.back().tags.append(values[i] + " ");
                    }
                }
            }
            break;
        case DESCRIPTION_MARKER:
            if (Exercises.size() and category == FetchValue("CATEGORY")) Exercises.back().description = current_line.substr(1, current_line.length() - 2);
            break;
        }
        
        if (variables_compiled) {
            DEFAULT_SETS = (DETERMINE_VALUE("DEFAULT_SETS", FetchValueInt));
            DEFAULT_FREESTYLE = (DETERMINE_VALUE("DEFAULT_FREESTYLE", FetchValueInt));
            DEFAULT_REPS = (DETERMINE_VALUE("DEFAULT_REPS", FetchValueInt));
            DEFAULT_HOLD = (DETERMINE_VALUE("DEFAULT_HOLD", FetchValueInt));
            DEFAULT_AHOLD = (DETERMINE_VALUE("DEFAULT_AHOLD", FetchValueInt));
        }
        
        values.clear();
        
        if (lines_reader.i == lines_reader.limit - 1) {break;} else {
            lines_reader.progress();
            current_line = *(lines_reader.at(lines_reader.i));
        }
    }
   
    if (!Exercises.size()) {
        Log("No exercises were registered", 8);
        ON_KEY_CLS();
        return 0;
    }
    
    /* POST COMPILATION */
    
    compile_extensions("post_compilation");
    
    /* START SCREEN */
    
    RESET_COLORS();
    compile_extensions("post_start_screen");
    RESET_COLORS();
    cout << endl;
    (Exercises.begin())->Describe2();
    RESET_COLORS();
    cout << endl << "Press any key to start" << endl;
    ON_KEY_CLS();

    /* MAIN LOOP */
    
    Reader<Exercise> exercise_reader;
    exercise_reader.attatch(&Exercises);
    
    bool finished = false;

    FLOOP (int, RUN, run_iteration) {
        FLOOP (int, ROUNDS, (DETERMINE_VALUE("ROUNDS", FetchValueInt))) {
            RESET_COLORS();
            exercise_reader.set(0);
            current_exercise = exercise_reader.at(0);
            exercise_reader.attatch(&Exercises);
        
            if (ROUNDS > 0) {
                current_exercise->Describe();
                ON_KEY_CLS();
            }

            // RB
            
            while (not finished) {
                /* Begin Exercise Block */
                UTIL::espeak(current_exercise->name);
                compile_extensions("post_exercise");
                /* End Exercise Block */
            
                FLOOP (int, SETS, current_exercise->sets) {
                    /* Begin Sets Block */
                    bool alternate = false;
                    bool skipped = false;
                    int  current_reps = 0;
                    Sleep((DETERMINE_VALUE("RDELAY", FetchValueInt)) * MS);
                    /* End Sets Block */
                    FLOOP (int, REPS, current_exercise->reps * 2) {
                        if (not alternate) {
                            ClearScreen();
                            bar("REPS: ", current_reps, current_exercise->reps, 11);
                            bar("HOLD: ", 0, current_exercise->hold, 5);
                            current_reps++;
                            UTIL::espeak(std::to_string(current_reps), current_exercise->freestyle);
                            SLEEP_TIME_FUNCTION(current_exercise->hold, if (GetAsyncKeyState(0x39)){
                                    Log("Skipping..", 4);
                                    skipped = true;
                                    break;
                                }
                                if (elapsed != last) {
                                    ClearScreen();
                                    bar("REPS: ", current_reps-1, current_exercise->reps, 11);
                                    bar("HOLD: ", elapsed, current_exercise->hold, 5);
                                });
                        
                            if (skipped) break;
                        } else {
                            //ClearScreen();
                            CLEAR();
                            bar("REPS: ", current_reps, current_exercise->reps, 11);
                            bar("AHOLD: ", 0, current_exercise->ahold, 5);
                            UTIL::espeak("Alternate", current_exercise->freestyle);
                            SLEEP_TIME_FUNCTION(current_exercise->ahold, if (GetAsyncKeyState(0x39)){
                                    Log("Skipping..", 4);
                                    skipped = true;
                                    break;
                                }

                                if (elapsed != last) {
                                    ClearScreen();
                                    bar("REPS: ", current_reps, current_exercise->reps, 11);
                                    bar("AHOLD: ", elapsed, current_exercise->ahold, 5);
                                }
                                );
                        
                            if (skipped) break;
                        }
                        alternate = (!alternate);
                    }
                    UTIL::espeak("Set " + std::to_string(SETS + 1) + " of " + std::to_string(current_exercise->sets) + " Finished");
                }

                CLEAR();
                UTIL::espeak(string(current_exercise->name).append(" Finished"));
                UTIL::espeak("Take a break");

                if (exercise_reader.i == exercise_reader.limit - 1) {
                    finished = true;
                } else {
                    exercise_reader.progress();
                    current_exercise = exercise_reader.at(exercise_reader.i);
                    //cout << "# Next Exercise" << endl;
                    current_exercise->Describe2();
                    cout << endl << "Press any key to proceed to next exercise" << endl;
                    ON_KEY_CLS();
                }
            }
            /* Round End Block */
            finished = false;
            // RE
            compile_extensions("round_end");
        }
    }

    
    UTIL::espeak("Session finished");
    UTIL::espeak("Excellent job");

    cin.get();
    return 0;
}

string FetchValue (string iden) {
    return Variables[iden] != "" ? Variables[iden] : "0";
}

int FetchValueInt (string iden) {
    return atoi(FetchValue(iden).c_str());
}

void bar(const char* label, int a, int b, int c) {
    FOREGROUND_COLOR(c);
    //CLEAR();
    string sbar(b, ' ');
    FLOOP(int, i, a) {
        sbar[i] = '#';
    }
    string a_c = std::to_string(a);
    string b_c = std::to_string(b);
    sbar.append(" " + a_c + "/" + b_c);
    cout << label << sbar << endl;
    RESET_COLORS();
}


void ClearScreen () {	
    COORD cursorPosition;
    cursorPosition.X = 0;
    cursorPosition.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

