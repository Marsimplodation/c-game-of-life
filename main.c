#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include "main.h"

int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Usage: <world size>\n");
        fieldSize=100;
        //return EXIT_FAILURE;
    } else {
        /* declare the basic variables */
        fieldSize = atoi(argv[1]);
    }
    
    //init field
    int field[fieldSize][fieldSize];
    initField(field);
    printField(field);

    //runs through the simulation
    while (true) {
        updateField(field);
        printField(field);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief enforces the rules of the game of life
 * 
 * @param newField 
 */
void updateField(int newField[fieldSize][fieldSize]) {
    int oldField[fieldSize][fieldSize];

    //copy old field
    for(size_t i = 0; i < fieldSize; i++) {
        for(size_t j = 0; j < fieldSize; j++) {
            oldField[i][j] = newField[i][j];
        }
    }

    //infect
    for(size_t i = 0; i < fieldSize; i++) {
        for(size_t j = 0; j < fieldSize; j++) {
                int livingNeigbours=0;
                
                //vertically
                if(i!=0 && oldField[i-1][j] == 1) {
                    livingNeigbours++;
                }
                if(i!=(fieldSize-1) && oldField[i+1][j] == 1) {
                    livingNeigbours++;
                }
                
                //horizontally
                if(j!=0 && oldField[i][j-1] == 1) {
                    livingNeigbours++;
                }
                if(j!=(fieldSize-1) && oldField[i][j+1] == 1) {
                    livingNeigbours++;
                }

                //diagonaly
                if(j!=0 && i!=0 && oldField[i-1][j-1] == 1) {
                    livingNeigbours++;
                }
                if(j!=(fieldSize-1) && i!=0 && oldField[i-1][j+1] == 1) {
                    livingNeigbours++;
                }
                if(j!=(fieldSize-1) && i!=(fieldSize-1) && oldField[i+1][j+1] == 1) {
                    livingNeigbours++;
                }
                if(i!=(fieldSize-1) && j!=0 && oldField[i+1][j-1] == 1) {
                    livingNeigbours++;
                }


                /* implemnting the three rules*/
                if(oldField[i][j] == 1 && livingNeigbours == 2 || oldField[i][j] == 1 && livingNeigbours == 3) {
                    //rule1 if a living cell has two or three living neighbours, nothing happens
                    newField[i][j]=1;
                } else if(oldField[i][j] == 0 && livingNeigbours == 3) {
                    //rule2 if a dead cell has three living neighbours, it gets reborn
                    newField[i][j]=1;
                } else if(oldField[i][j] == 1) {
                    //rule3 every other cell shall be dead
                    newField[i][j]=0;
                }      
        }
    }
}


/**
 * @brief prints out the whole field at once
 * 
 * @param field 
 */
void printField(int field[fieldSize][fieldSize]) {
    //clear screen
    printf("\e[1;1H\e[2J");

    int length=fieldSize*(fieldSize*6+1);
    char *fieldString = NULL;

    //allocate the neeeded memory for the line string
    fieldString = malloc((sizeof(char) * (length +1)));
    int strPos=0;

    //set each position of the line properly
    for(size_t i = 0; i < fieldSize; i++) {
        for(size_t j = 0; j < fieldSize; j++) {
            if(field[i][j] == 0) {
                fieldString[strPos++]="░"[0];
                fieldString[strPos++]="░"[1];
                fieldString[strPos++]="░"[2];
                fieldString[strPos++]="░"[0];
                fieldString[strPos++]="░"[1];
                fieldString[strPos++]="░"[2];
            } else {
                fieldString[strPos++]="█"[0];
                fieldString[strPos++]="█"[1];
                fieldString[strPos++]="█"[2];
                fieldString[strPos++]="█"[0];
                fieldString[strPos++]="█"[1];
                fieldString[strPos++]="█"[2];
            }
        }
        fieldString[strPos++]='\n';//new line
    }

    //add NULL character at the end
    fieldString[length] = '\0';
    //printField
    printf(fieldString);
    //free up memory
    free(fieldString);
    //sleep 16 miliseconds
    msleep(16);
}

/**
 * @brief sets each cell to 0 and places the config shapes into the grid
 * 
 * @param field 
 */
void initField(int field[fieldSize][fieldSize]) {
    for(size_t i = 0; i < fieldSize; i++) {
        for(size_t j = 0; j < fieldSize; j++) {
            field[i][j] = 0;
        }
    }

    //add basic shapes defined in config
    size_t len=0;
    char *line=NULL;
    char *tmp=NULL;
    int locX=0;
    int locY=0;
    int row=0;
    int col=0;
    FILE *cptr=fopen("./config","rb");
    while (getline(&line, &len, cptr) != -1) {
        int lineLen=strlen(line);
        //checks if the substring 'location:' is in the line
        tmp=strstr(line, "location:");
        if(tmp) {
            bool readPos1=false;
            bool readPos2=false;
            int pos=0;
            int numLen=numPlaces(fieldSize);
            char x[numLen+1];
            char y[numLen+1];
            /* splitting the string at ' ' for x and at ',' for y */
            for (int i=0; i<lineLen; i++) {
                //parse the location value
                if(readPos1 == true){
                    //enable reading the second position
                    if(line[i] == ',') {
                        pos=0;
                        readPos2=true;
                        readPos1=false;
                    } else {
                        //read the first position
                        x[pos++]=line[i];
                        x[pos]='\0';
                    }
                }

                //actually read the second position
                if(readPos2 == true){
                    if(line[i] != ',') {
                        y[pos++]=line[i];
                        y[pos]='\0';
                    }
                }

                //enable reading the first position
                if(line[i] == ' ') {
                    readPos1=true;
                }
               
            }
            //covert the character to int values
            locX=atoi(x); 
            locY=atoi(y);
        }

        //check if the shape fits in to the field
        if(locX < fieldSize && locY < fieldSize) {
        //actually putting the shape into the field
            col=0;
            if(line[0] == ':'){
                for(int i=1; i<lineLen; i++) {
                    //adding the NULL character
                    char c[2]={line[i], '\0'};
                    if(c){
                        field[locX+row][locY+col]=atoi(c);
                        col++;
                    }
                }
                row++;
            } else {
                row=0;
            }
        }
    }

    //closes config file and free up some memory
    free(line); 
    fclose(cptr);
    
}

/**
 * @brief 
 * 
 * @param msec 
 * @return int 
 */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

/**
 * @brief retruns the number of places an integer has
 * 
 * @param n 
 * @return int 
 */
int numPlaces(int n) {
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
}
