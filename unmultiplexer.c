/***********************************************************************
 School works
 Copyright (C) 2015-2016 Philippe VIENNE

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "unmultiplexer.h"

int main() {

    // Open the original file with data
    FILE *data = fopen("data.txt", "r");
    // This will store the outputs file (depending on MAX_OUT_FILES)
    FILE *outs[MAX_OUT_FILES + 1]; // +1 is because we have the f0 file for error output
    char fileName[15]; // Just to store the name before open
    int i;
    struct stats statistics; // Store statistics into a structure because you can pass it to functions
    statistics.invalid = 0;
    statistics.total = 0;
    for(i=0; i<MAX_OUT_FILES+1; i++){
        // Update the fileName var with the filename
        sprintf(fileName, "f%d", i);
        // Open the out file
        outs[i] = fopen(fileName, "w+");
        // Init the stats
        statistics.data[i] = 0;
    }
    // Loop to handle all lines
    // While the processed line is not the last, we continue
    while(processLine(data, outs, &statistics) == CONTINUE);

    // Display the Report
    printf("%d entries are invalid on %d\n", statistics.invalid, statistics.total);
    for(i=0; i<MAX_OUT_FILES+1; i++){
        printf("File f%d: %d elements written\n", i, statistics.data[i]);
    }

    return 0;
}

/**
 * Initialize an array with a given value.
 * Used for content var in next function.
 */
void initArray(char data[], int size, char value){
    int i;
    for(i=0; i<size; i++){
        data[i] = value;
    }
}

/**
 * Handle each line, write output to outs files and update statistics
 */
int processLine(FILE *data, FILE *outs[], stats *statistics) {
    // Designates in which file we will store this line
    // -1 designates an unknown file
    int fileCode = -1;
    // Storage for the line content while we read
    char content[TAILLE_MAX_TRAME];
    int dataLength = 0;
    initArray(content, TAILLE_MAX_TRAME, '\0');
    // Must store the hash
    // Hash var is also the tmp value when we are reading the line
    int hash = -1;
    // Which index are us ?, useful for the content array update
    int readingIndex = 0;
    // Current read char
    int c;
    while ((c = fgetc(data)) != EOF) {
        /* While we are not on EOF, we suppose that we should get a
         * \n before the EOF, unless the line will be ignored */
        if (c == '\n') { // We reach the end of line, now we should write data
            hash = charToHex(hash); // Convert the ASCII valued hash to the Numeric value
            if(fileCode == -1) {
                return STOP; // We have try to read an empty line, consider as an EOF
            } else {
                (*statistics).total++; // Increments the number of total lines
                if(hash != -1 && checkSum(content, hash)){
                    // If the hash is correct
                    if (hash == 0 && content[0] == '\0') // Means no data !
                        return CONTINUE; // Valid but empty data
                    int startIndex = 0;
                    for(startIndex=0;startIndex<dataLength;startIndex+=7){
                        char value[7]; // The truncated value we should write
                        int i;
                        int count = 0;
                        for(i=startIndex; i<startIndex+7; i++){
                            if (content[i] == '\0') { // Get an end of string before the 7th char
                                value[i%7] = '?'; // Add some '?' chars
                            } else {
                                value[i%7] = content[i]; // Just copy our value
                                count++;
                            }
                        }
                        fprintf(outs[fileCode], "%s%hx\n", value, count); // Write formatted data
                        (*statistics).data[fileCode] += count + 1; // Increments statistics for this file

                    }
                    // Don't count the ? as value char, but count the hash added code.
                } else {
                    // If the hash is not valid
                    (*statistics).invalid++;
                    fprintf(outs[0], "%s\n", content); // Write formatted content, without any hash
                    // Count length of data
                    int realLength = 0;
                    while(content[realLength] != '\0'&&realLength<TAILLE_MAX_TRAME){
                        realLength++;
                    }
                    (*statistics).data[0] += realLength; // Update stats
                }
            }
            return CONTINUE; // Ended process, try to read another line
        }
        if (fileCode == -1) { // First read char will be the file code
            fileCode = c - '0'; // Convert ASCII data to Numeric data
            if (fileCode < 0) { // Got some problems here, exit !
                exit(64);
            }
        } else { // Not the first char
            if(hash!=-1){
                /* If we've got an hash in memory but we are reading a new data,
                 * then it's a content data, so put it in the content array.
                 * The index is readingIndex - 2 because the first char is the file
                 * and here the readingIndex is already on the next value.
                 */
                if(readingIndex - 2 > TAILLE_MAX_TRAME){
                    assert(0); // If the message is longer than it should, stop all (System error)
                }
                content[readingIndex - 2] = (char) hash; // Put it in the content
                dataLength++;
            }
            hash = c; // Consider the last read char as the hash char
        }
        readingIndex++;
    }
    return STOP; // Reach end of file, prevent to stop reading file
}

/**
 * Check the sum for a given data array.
 * We suppose that the data ends on first \0 char.
 */
int checkSum(char data[], int hash){
    int realLength = 0;
    int i = 0;
    while(data[i] != '\0'&&i<TAILLE_MAX_TRAME){
        i++;
        realLength++;
    }
    return realLength%16 == hash;
}

/**
 * Convert a simple char/int ASCII value to its Hex Value.
 */
int charToHex(int c){
    if (c >= '0' && c <= '9') { // ASCII char 0 to 9
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') { // ASCII char A to F
        return c - 'A' + 10; // +10 is the value in Hex of A
    }
    if (c >= 'a' && c <= 'f') { // ASCII char a to f
        return c - 'a' + 10; // +10 is the value in Hex of a
    }
    return -1;
}