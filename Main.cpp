#include "lib/fileH/Screen.h"
#include "lib/fileH/Draw.h"

int main(int argc, char **argv){
    initSprites();
    initScreen(360 , 240, 1, 1);
    startScreen(argc, argv);
    return 0;
}


