//
// Created by pvienne on 25/09/15.
//

#ifndef TP1_UNMULTIPLEXER_H
#define TP1_UNMULTIPLEXER_H

#define TAILLE_MAX_TRAME 32
#define MAX_OUT_FILES 2
#define STOP 1
#define CONTINUE 0

struct stats{
    int invalid;
    int total;
    int data[MAX_OUT_FILES + 1];
};

int processLine(FILE *data, FILE *pFILE[], struct stats *stats1);
int checkSum(char data[], int hash);
int charToHex(int c);
#endif //TP1_UNMULTIPLEXER_H
