#ifndef _MYTAR_H
#define _MYTAR_H

#include <limits.h>
#include "crc.h"

typedef enum{
  NONE,
  ERROR,
  CREATE,
  CHECKSUM,
  EXTRACT
} flags;

typedef struct {
  char* name;
  unsigned int size;
} stHeaderEntry;

int createTar(int nFiles, char *fileNames[], char tarName[]);
int extractTar(char tarName[]);
int computeChecksums(char tarName[]);


#endif /* _MYTAR_H */
