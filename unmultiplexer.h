/***********************************************************************
 School works
 Copyright (C) 2015-2016 Philippe VIENNE - Gabriel AUGENDRE

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
