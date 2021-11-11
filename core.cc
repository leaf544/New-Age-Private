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

class Reader {
public:
    int i = 0;
    int limit = 0;
    std::vector<Exercise>* list;
    void attatch (std::vector<Exercise>* l) {this->list = l; this->limit = l->size();}
    Exercise* at (int pos) {return &list->at(pos);}
    void set (int pos) {this->i = pos;}
    void progress () {this->i++;}
    void regress () {this->i--;}
};

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
            if (std::string(ent->d_name).find(".newage") != std::string::npos) {
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
    std::string category = "";
    std::vector<std::string> values;
    
    /* PRE COMPILATION  */ 
    
    bool variables_compiled = false;
    
    for (std::string line; std::getline(file, line);) {
        if (!line.size()) continue;
        values = UTIL::split_string(line.substr(1, line.length()), " ");
        // DEBUG BEG
        // cout << "Values: ";
        // for (auto& val : values) {
        //     cout << val << ", ";
        // }
        // DEBUG END
        switch (line[0]) {
        case VARIABLE_MARKER: 
            Variables[values[1]] = values[2];
            break;
        case CATEGORY_MARKER:

            // DEBUG BEG
            // for (auto it = Variables.begin(); it != Variables.end(); it++) {
            //     cout << "IDEN: " << it->first << " / " << "VALUE: " << it->second << endl;
            // }
            // DEBUG END
            
            variables_compiled = true;
            category = values[0];
            // DEBUG BEG
            // cout << "Category: " << category << endl;
            // cout << "Category specifier: " << FetchValue("CATEGORY") << endl;
            // cout << std::boolalpha << (category == FetchValue("CATEGORY")) << endl;
            // DEBUG END
            if (category == FetchValue("CATEGORY") and values.size() > 1) {
                // DEBUG BEG
                //cout << "Marked category: " << category << endl;
                // DEBUG END
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
            }
            break;
        case DESCRIPTION_MARKER:
            if (Exercises.size()) Exercises.back().description = line.substr(1, line.length() - 2);
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
    
    Reader reader;
    reader.attatch(&Exercises);
    
    bool finished = false;
    
    FLOOP (int, ROUNDS, (DETERMINE_VALUE("ROUNDS", FetchValueInt))) {
        RESET_COLORS();
        reader.set(0);
        current_exercise = reader.at(0);
        reader.attatch(&Exercises);
        
        if (ROUNDS > 0) {
            current_exercise->Describe();
            ON_KEY_CLS();
        }
        
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
            UTIL::espeak(std::string(current_exercise->name).append(" Finished"));
            UTIL::espeak("Take a break");

            if (reader.i == reader.limit - 1) {
                finished = true;
            } else {
                reader.progress();
                current_exercise = reader.at(reader.i);
                //cout << "# Next Exercise" << endl;
                current_exercise->Describe2();
                ON_KEY_CLS();
            }
        }
        /* Round End Block */
        finished = false;
        compile_extensions("round_end");
    }
    
    UTIL::espeak("Session finished");
    Sleep(MS); UTIL::espeak("Excellent job");

    cin.get();
    return 0;
}

std::string FetchValue (std::string iden) {
    return Variables[iden] != "" ? Variables[iden] : "0";
}

int FetchValueInt (std::string iden) {
    return atoi(FetchValue(iden).c_str());
}

void bar(const char* label, int a, int b, int c) {
    FOREGROUND_COLOR(c);
    //CLEAR();
    std::string sbar(b, ' ');
    FLOOP(int, i, a) {
        sbar[i] = '#';
    }
    std::string a_c = std::to_string(a);
    std::string b_c = std::to_string(b);
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
