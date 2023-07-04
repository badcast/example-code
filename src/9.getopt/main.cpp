#include <stdio.h>
#include <unistd.h>

int main(int argc, char * argv[]){

    for(;;){

        //Здесь a - одиночный аргумент для программы!
        //Здесь b - Аргумент требующее дополнительной опций
        //Здесь h - вызов помощи или тому подобное
        switch (getopt(argc,argv,"ab:h")) {
            case 'a':
                printf("Option a\n");
                continue;
            case 'b':
                printf("parameter 'b' specified with the value %s\n", optarg);
                continue;
            case 'h':
            case '?':
            default:
                printf("help usage h/a/b\n");
            case -1:
                break;
        }
        break;
    }

    return 0;
}
