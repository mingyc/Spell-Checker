#include "Compressor.h"
#include "bzlib.h"

#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUFSIZE 4096
#define FD_READ   0
#define FD_WRITE  1
#define MAX_SHARED_MEMORY 10485760 // 10MB



Compressor::Compressor() {
}

Compressor::~Compressor() {
}

//
// public static void Compress(int srcFd, const char *destFileName)
// @srcFd:
// @destFileName:
//    Apply bzip2 algorithm to compress source file from srcFd to destination file destFileName
//
void Compressor::Compress(int srcFd, const char *destFileName) {

  FILE*   outFile;
  BZFILE* b;
  int     nBuf;
  char    buf[BUFSIZE+1];
  int     bzerror;



  outFile = fopen ( destFileName , "w" );
  b = BZ2_bzWriteOpen ( &bzerror, 
      outFile, 
      blockSize100k,  // int blockSize100k: specifies the block size to be used for compression between 1, 9]. 9 gives the best compression but takes most memory.
      verbosity,      // int verbosity: specifies verbose monitoring/debugging output. 0: silent, 4:most
      workFactor      // int workFactor: set this carefully. [0, 250]
      );
  if (bzerror != BZ_OK) {
    BZ2_bzWriteClose ( &bzerror, b, 1, NULL, NULL );
    /* handle error */
  }

  nBuf = 0;
  while ( (nBuf = read(srcFd, buf, BUFSIZE)) > 0 ) {
    /* get data to write into buf, and set nBuf appropriately */
    BZ2_bzWrite ( &bzerror, b, buf, nBuf );
    if (bzerror == BZ_IO_ERROR) { 
      BZ2_bzWriteClose ( &bzerror, b, 1, NULL, NULL );
      /* handle error */
    }
  }

  BZ2_bzWriteClose ( &bzerror, b, 0, NULL, NULL );
  if (bzerror == BZ_IO_ERROR) {
    /* handle error */
  }

  close(srcFd);
  fclose(outFile);
}

void Compressor::Decompress(const char *srcFileName, int destFd) {
  FILE*   inFile;
  BZFILE* b;
  int     nBuf;
  char    buf[BUFSIZE];
  int     bzerror;
  int     nRead;

  inFile = fopen ( srcFileName, "r" );

  b = BZ2_bzReadOpen ( &bzerror,
      inFile,
      0,            // int small:
      verbosity,    // int verbosity: specifies verbose monitoring/debugging output. 0: silent, 4: most
      NULL,         // void *unused:
      0             // int nUnused:
      );
  if (bzerror != BZ_OK) {
    BZ2_bzReadClose ( &bzerror, b);
    /* handle error */
  }

  bzerror = BZ_OK;
  nRead = 0;
  while ( bzerror == BZ_OK ) {
    /* get data to write into buf, and set nBuf appropriately */
    nRead = BZ2_bzRead ( &bzerror, b, buf, BUFSIZE );
    if (bzerror == BZ_IO_ERROR) {
      BZ2_bzReadClose ( &bzerror, b);
      /* handle error */
    }
    for (nBuf = 0; (nBuf += write(destFd, buf+nBuf, nRead)) < nRead; );
  }


  BZ2_bzReadClose ( &bzerror, b);
  if (bzerror == BZ_IO_ERROR) {
    /* handle error */
  }

  fclose(inFile);
  close(destFd);
}
