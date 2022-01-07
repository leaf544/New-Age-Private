#include <iostream>
#include <string>

main () {

    std::string cmd = "@echo off"
        "set \"file=C:/Users/doode/Desktop/private-new-age/0/0.mp3\""
    "( echo Set Sound = CreateObject(\"WMPlayer.OCX.7\"^)"
        "echo Sound.URL = \"%file%\""
        "echo Sound.Controls.play"
        "echo do while Sound.currentmedia.duration = 0"
        "echo wscript.sleep 100"
        "echo loop"
        "echo wscript.sleep (int(Sound.currentmedia.duration^)+1^)*1000) >sound.vbs"
        "start /min sound.vbs";


    
    //cout << cmd << endl;;
    system(cmd.c_str());
        
    std::cin.get();
    return 0;
}
 
