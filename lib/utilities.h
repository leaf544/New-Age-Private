#ifndef UTIL_H
#define UTIL_H

/* STD */
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

/* C */
#include <Windows.h>
#include <time.h>
#include <stdlib.h>
#include <string.h> // strtok(), strcat()


using std::cout;
using std::endl;

namespace UTIL {
    
    std::vector<std::string> split_string (std::string str, char* delim) {
        std::vector<std::string> ret;
        char cstring [str.length() + 1];
        strcpy(cstring, str.c_str());
        for (char* token = strtok(cstring, delim); token != NULL; token = strtok(NULL, delim)) {
            ret.push_back(std::string(token));
        } 
        return ret;
    }

    void say () {
        static int folder = 0;
        static int snippet = 0;

        char cmd[] = "@echo off\n"
        "set \"file=C:/Users/doode/Desktop/private-new-age/0/0.mp3\"\n"
        "( echo Set Sound = CreateObject(\"WMPlayer.OCX.7\"^)\n"
        "echo Sound.URL = \"%file%\"\n"
        "echo Sound.Controls.play\n"
        "echo do while Sound.currentmedia.duration = 0\n"
        "echo wscript.sleep 100\n"
        "echo loop\n"
        "echo wscript.sleep (int(Sound.currentmedia.duration^)+1^)*1000) >sound.vbs\n"
        "start /min sound.vbs\n"
        "";

        cmd[59] = std::to_string(folder)[0];
        cmd[61] = std::to_string(snippet)[0];

        FILE* bat = fopen("sound.bat", "w");
        fputs(cmd, bat);
        fclose(bat);

        system("sound.bat");
        
        snippet++;
    }
    
    void espeak (std::string message, char free = 'F') {
        //string command = ""
        if (free == 'F') {
            std::replace(message.begin(), message.end(), ' ', '_');
            char system_command [128] = "espeak ";
            strcat(system_command, message.c_str());
            system(system_command);
        }
    }
}


#endif
