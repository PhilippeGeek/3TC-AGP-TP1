//
// Created by pvienne on 25/09/15.
//

#include <stdio.h>
#include <assert.h>
#include "unmultiplexer.h"

int main(char **args, int argc){

    FILE *data = fopen("data.txt", "r");
    FILE *outs[MAX_OUT_FILES + 1];
    char fileName[15];
    int i;
    struct stats statistics;
    statistics.invalid = 0;
    statistics.total = 0;
    for(i=0; i<MAX_OUT_FILES+1; i++){
        sprintf(fileName, "f%d", i);
        outs[i] = fopen(fileName, "w+");
        statistics.data[i] = 0;
    }
    // Loop to handle all lines
    while(processLine(data, outs, &statistics) == CONTINUE);

    // Report
    printf("%d entries are invalid on %d\n", statistics.invalid, statistics.total);
    for(i=0; i<MAX_OUT_FILES+1; i++){
        printf("File f%d: %d elements written\n", i, statistics.data[i]);
    }

    return 0;
}

void initArray(char data[], int size, char value){
    int i;
    for(i=0; i<size; i++){
        data[i] = value;
    }
}

// Handle each line for process
int processLine(FILE *data, FILE *outs[], struct stats *statistics) {
    int fileCode = -1;
    char content[TAILLE_MAX_TRAME];
    initArray(content, TAILLE_MAX_TRAME, '\0');
    int hash = -1;
    int readingIndex = 0;
    int c;
    while((c=fgetc(data)) != EOF){
        if(c=='\n'){
            hash = charToHex(hash);
            if(fileCode == -1) {
                return STOP; // We have try to read an empty line, consider as an EOF
            } else {
                (*statistics).total++;
                char value[7];
                int hasEmptyElements = 0;
                int i;
                for(i=0; i<7; i++){
                    if (content[i] == '\0') {
                        value[i] = '?';
                        hasEmptyElements = 1;
                    } else {
                        value[i] = content[i];
                    }
                }
                if(hash != -1 && checkSum(content, hash)){
                    if(hash == 0 && content[0] == '\0')
                        return CONTINUE; // Valid but empty data
                    fprintf(outs[fileCode], "%s%hx\n", value, hasEmptyElements?hash:7);
                    (*statistics).data[fileCode] += hash;
                } else {
                    (*statistics).invalid++;
                    fprintf(outs[0], "%s\n", content);
                    int realLength = 0;
                    while(content[realLength] != '\0'&&realLength<TAILLE_MAX_TRAME){
                        realLength++;
                    }
                    (*statistics).data[0] += realLength;
                }
            }
            return CONTINUE;
        }
        if(fileCode == -1){
            fileCode = c - 48;
        } else {
            if(hash!=-1){
                /* If we've got an hash in memory but we are reading a new data,
                 * then it's a content data, so put it in the content array.
                 * The index is readingIndex - 2 because the first char is the file
                 * and here the readingIndex is already on the next value.
                 */
                if(readingIndex - 2 > TAILLE_MAX_TRAME){
                    assert(0); // If the message is longer than it should, stop all (System error)
                }
                content[readingIndex - 2] = (char) hash;
            }
            hash = c;
        }
        readingIndex++;
    }
    return STOP; // Reach end of file, prevent to stop reading file
}


int checkSum(char data[], int hash){
    int realLength = 0;
    int i = 0;
    while(data[i] != '\0'&&i<TAILLE_MAX_TRAME){
        i++;
        realLength++;
    }
    return realLength%16 == hash;
}

int charToHex(int c){
    if(c>=48&&c<=57){ // ASCII char 0 to 9
        return c-48;
    }
    if(c>=65&&c<=70){ // ASCII char A to F
        return c-65+10;
    }
    if(c>=97&&c<=102){ // ASCII char a to f
        return c-97+10;
    }
    return -1;
}