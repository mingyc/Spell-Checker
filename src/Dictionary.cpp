#include "Dictionary.h"
#include "Compressor.h"
#include "predict.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <sys/wait.h>
#define  FD_READ  0
#define  FD_WRITE 1

//
// public Dictionary(const string &srcTxtFile, const string &destBinFile)
// @srcTxtFile:
//    The name of a text file worked as the source of this dictionary
// @destBinFile:
//    The name of where this dictionary should dump the internal data structure
// The constructor will read from srcTxtFile, build a dictionary to be queried and then dump the whole
// data structure into destBinFile
//
Dictionary::Dictionary(const string &srcTxtFileName, const string &destBinFileName) {
  root = (struct Dict *)calloc(1, sizeof(struct Dict));
  preroot = (struct Dict *)calloc(1, sizeof(struct Dict));
  find = (int *)calloc(preSize, sizeof(int));
  preread(preroot, preDict, preSize);
  read(srcTxtFileName);
  dump(destBinFileName);
}

//
// public Dictionary(const string &srcBinFileName)
// @srcBinFile:
//    The name of a dictionary file worked as the source of this dictionary
// The constructor will load a binary file which is of the same formatted as internal data structure
//
Dictionary::Dictionary(const string &srcBinFileName) {
  root = (struct Dict *)calloc(1, sizeof(struct Dict));
  preroot = NULL;
  find = (int *)calloc(preSize, sizeof(int));
  load(srcBinFileName);
}

//
// public ~Dictionary()
//
// The deconstructor will release all memory used by this object
//
Dictionary::~Dictionary() {
  if(preroot) DictDestroy(preroot);
  if(root) DictDestroy(root);
  if(find) free(find);
}

//
// public bool exist(const string &word)
// @word:
//    The word to be checked if it is in this dictionary or not
// Return true if found in internal structure
//
bool Dictionary::exist(const string &word) {
  return DictFind(root, word.c_str());
}


// Attention: This function doesn't work now
//
// public int getCount(const string &word)
// @word:
//    The word to be counted in this dictionary
// Return the number of apperances of this word
//
int Dictionary::getCount(const string &word){
  return -1;
}

void Dictionary::preread(struct Dict *root, const char *dict[], const int size){
  int i;

  for(i = 0; i < size; i++){
    DictAdd(root, dict[i]);
  }

}

//
// private void read(const string &file)
// @file:
//    Given a txt file, parse it into internal data structure and prepared for querying
//
void Dictionary::read(const string &file) {
  FILE *corpus = fopen(file.c_str(), "r");
  char buf[32];
  int i;
  struct Dict *tmp = (struct Dict *)calloc(1, sizeof(struct Dict));

  // add words read from yylex to dictionary
  while(fscanf(corpus, "%s", buf) == 1){
    DictAdd(tmp, buf);
	if(DictFind(preroot, buf) == false){
	  DictAdd(root, buf);
	}
  }

  for(i = 0; i < preSize; i++){
    if(DictFind(tmp, preDict[i]) == true){
	  find[i] = 1;
	}
  }

  fclose(corpus);
  DictDestroy(tmp);
}


//
// private void load(const string &file)
// @file:
//     The file name of the dictionary file which we want to read dictionary
// Load the binary file into internal memory format
//
void Dictionary::load(const string &file) {

  // create pipe that used to connected parent process and child process
  pid_t cpid = 0;
  int pfd[2];
  int i;
  struct Dict* tmp = (struct Dict*)calloc(1, sizeof(struct Dict));
  assert(pipe(pfd) != -1);

  // that is the file pointer to the Dictionary file
  FILE *dictFile = fdopen(pfd[FD_READ], "r");
  
  if ((cpid = fork()) == 0) { /* child process */
    close(pfd[FD_READ]);
  
    // Communicate with the compressor by pipe
    Compressor::Decompress(file.c_str(), pfd[FD_WRITE]);
    //Compressor::ParallelDecompress(file.c_str(), pfd[FD_WRITE]);
    _exit(EXIT_SUCCESS);

  }else if (cpid > 0) { /* main process */
    close(pfd[FD_WRITE]);
  
    // read dictionary file to reconstruct the dictionary
    fgetc(dictFile);
    DictLoad(root, dictFile);

	fgetc(dictFile);
	DictLoad(tmp, dictFile);
	
    fclose(dictFile);
    wait(NULL);

  }else{ /* error */ }
  
  for(i = 0; i < preSize; i++){
    if(DictFind(tmp, preDict[i]) == false){
	  DictAdd(root, preDict[i]);
	}
  }
  
  DictDestroy(tmp);
}

//
// private void dump(const string &file)
// @file:
//     The file name of the dictionary file which we want to dump dictionary
// Dump internal data structure into this file in binary format
//
void Dictionary::dump(const string &file) {
  
  // create pipe that used to connected parent process and child process
  pid_t cpid = 0;
  int pfd[2];
  int i;
  struct Dict* tmp = (struct Dict*)calloc(1, sizeof(struct Dict));
  assert(pipe(pfd) != -1);
 
  // that is the file pointer to the Dictionary file
  FILE *dump = fdopen(pfd[FD_WRITE], "w");  

  if ((cpid = fork()) == 0) { /* child process */
    close(pfd[FD_WRITE]);

    // Communicate with the bzip2 compressor by pipe   
    Compressor::Compress(pfd[FD_READ], file.c_str());
    _exit(EXIT_SUCCESS);

  }else if (cpid > 0) { /* parent process */
    close(pfd[FD_READ]);

    // dump the dictionary to a dictionary file
    DictDump(root, dump);

	for(i = 0; i < preSize; i++){
	  if(find[i] == 0){
	    DictAdd(tmp, preDict[i]);
	  }
	}
	DictDump(tmp, dump);
	
	fclose(dump);
    wait(NULL);

  }else { /* error */ }
  
  DictDestroy(tmp);
}

//
// private void DictLoad(struct Dict *root, FILE *dictFile)
// @root:
//     Pointer of the dictionary which we want to load
// @dictFile:
//     The file pointer of the dictionary file which we want to load
// Load the binary file into internal memory format
//
void Dictionary::DictLoad(struct Dict *root, FILE *dictFile){

  int c, index = 0;
  bool exist;

  while((c=fgetc(dictFile))!=')'){ /* if we didn't reach the end of this scope */
    if(c == '('){ /* meet the start of next scope */
      DictLoad(root->next[index], dictFile);
    }else{
      
	  if(c == '!'){ /* uppercase means there is a word ends in this node */
        root->next[index]->exist = true;
		continue;
      }

      // construct a new node
      index = c - 'a';
      root->next[index] = (struct Dict *)calloc(1, sizeof(struct Dict));
      if(exist){
        root->next[index]->exist = true;
      }

    }
  }
}

//
// private void DictLAdd(struct Dict *root, char *word)
// @root:
//     Pointer of the dictionary which we want to insert word
// @word:
//     The word which we want to insert in this dictionary
// Insert word to the dictionary
//
void Dictionary::DictAdd(struct Dict *root, const char *word){
  int i;
  struct Dict* currentPtr = root;

  for(i = 0; ; i++){
    if(word[i] == '\0'){ /* reach the word's end, add it in this node */
      currentPtr->exist = true;
      break;
    }else{
      if(currentPtr->next[word[i]-'a'] == NULL){ /* didn't have path to go to the final node of this word */
        currentPtr->next[word[i]-'a'] = (struct Dict *)calloc(1, sizeof(struct Dict));
      }

      // go to the next level
      currentPtr = currentPtr->next[word[i]-'a'];
    }
  }
}

//
// private void DictDump(struct Dict *root, FILE *dump)
// @root:
//     Pointer of the dictionary which we want to dump
// @dump:
//     The file pointer of the dictionary file which we want to dump
// Dump internal data structure into this file in binary format
//
void Dictionary::DictDump(struct Dict *root, FILE *dump){
  int i;
  bool leaf = true;

  for(i = 0; i < DICT_WIDTH; i++){
    if(root->next[i] != NULL){ /* this node is not a leaf anymore */
      if(leaf == true){
        leaf = false;
        fputc('(', dump);
      }
      
	  fputc(i+'a', dump);

      if(root->next[i]->exist){ /* if there is a word ends at this node, the print the uppercase */
        fputc('!', dump);
      }

      // go to the next level recursively
      DictDump(root->next[i], dump); 
    }
  }
  if(leaf == false){
    fputc(')', dump);
  }
}

//
// public bool DictDestroy(struct Dict *root)
// @root:
//     Pointer to the dictionary which we want to destroy
// Release all memory used by the Dictionary root
//
void Dictionary::DictDestroy(struct Dict *root){

  int i;

  if(root != NULL){
    for(i = 0; i < DICT_WIDTH; i++){
      // destroy all node recusively
      DictDestroy(root->next[i]);
    }
  }
  free(root);
}

//
// public bool DictFind(struct Dict *root, const char *target)
// @root:
//     Pointer of the dictionary which we want to query
// @target:
//     The target to be checked if it is in this dictionary or not
// 
// Return true if found in internal structure
//
bool Dictionary::DictFind(struct Dict *root, const char *target){
  int i;
  struct Dict *currentPtr = root;

  for(i = 0; ; i++){
    if(target[i] == '\0'){ /* reach the target's end */
      if(currentPtr->exist){
        return true;
      }else{
        return false;
      }
    }else{
      if(currentPtr->next[target[i]-'a'] == NULL){ /* don't have a node to this target */
        return false;
      }else{
        currentPtr = currentPtr->next[target[i]-'a'];
      }
    }
  }
}


