#ifndef MACROS_H
#define MACROS_H

#include "data.h"

/* GENERAL USE MACROS */

#define BIGGER(a, b) a > b

#define STANDARD_EXIT_PROCEDURE() { \
    std::cin.get(); \
    std::exit(1);   \
}

#define CLEAR() system("cls");

#define ON_KEY_CLS() {                          \
        cin.get();                              \
        CLEAR();                                \
    }                                               

#define SLEEP_TIME_FUNCTION(t, body) {          \
        int start = time(NULL);                 \
        int elapsed = 0;                        \
        int last = 0;                           \
        while (elapsed < t) {                   \
            elapsed = time(NULL) - start;       \
            body;                               \
        }                                       \
    }



#define FLOOP(t, it, n) for(t it = 0; it < n; it++)
#define FLOOPS(t, it, v,  n) for(t it = v; it < n; it++)

#define CSTRING_EQUAL 0
#define MS 1000

/* NEW AGE RELATED MACROS */

#define FOREGROUND_COLOR(c) SetConsoleTextAttribute(hConsole, c);
#define BACKGROUND_COLOR(c) SetConsoleTextAttribute(hConsole, c);
#define RESET_COLORS() FOREGROUND_COLOR(DEFAULT_FOREGROUND);
#define DETERMINE_VALUE(val, f) living_category.hasVariables ? living_category.f(val) == 0 ? f(val) : living_category.f(val): f(val)
#define CATOI(o) atoi(o.c_str())
#define EXTENSION void

#define Log(msg, c) FOREGROUND_COLOR(c); cout << msg << endl; RESET_COLORS();

#endif
