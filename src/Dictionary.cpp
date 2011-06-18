#include "Dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>
#define  FD_READ  0
#define  FD_WRITE 1

extern int yylex();
extern FILE *yyin;
extern FILE *yyout;
extern char *yytext;
extern char *dicttext;


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
  preroot = NULL;
  root = NULL;
  length = 0;
  read(srcTxtFileName);
  dump(destBinFileName);
}

//
// public Dictionary(const string &srcBinFileName)
// @srcBinFile:
//
// The constructor will load a binary file which is of the same formatted as internal data structure
//
Dictionary::Dictionary(const string &srcBinFileName) {
  root = NULL;
  length = 0;
  load(srcBinFileName);
}

Dictionary::~Dictionary() {
  delete[] root;
}

//
// public bool exist(const string &word)
// @word:
//    The word to be checked if it is in this dictionary or not
// Return true if found in internal structure
//
bool Dictionary::exist(const string &word) {
  return (bool)DictFind(root, word.c_str());
}


//
// public int getCount(const string &word)
// @word:
//    The word to be counted in this dictionary
// Return the number of apperances of this word
//
int Dictionary::getCount(const string &word){
  /*struct Dict *tmpDict = DictFind(root, word.c_str());
  if(tmpDict == NULL)
    return -1;
  else
    return tmpDict->count;*/
  return -1;
}


//
// private void read(const string &file)
// @file:
//    Given a txt file, parse it into internal data structure and prepared for querying
//
void Dictionary::read(const string &file) {
  FILE *corpus = fopen(file.c_str(), "r");

  // redirect yyin and close yyout
  yyin = corpus;
  //fclose(yyout);
  
  // add words from yylex to dictionary
  while(yylex() != 0){
    //if(preDictAdd(&preroot, dicttext) == true)
	preDictAdd(&preroot, dicttext);
    length++;
  }

  fclose(corpus);
}


//
// private void load(const string &file)
// @file:
//    
// Load the binary file into internal memory format
//
void Dictionary::load(const string &file) {
  FILE *dictFile = fopen(file.c_str(), "r");
  int i;
  char buf[256];
  fscanf(dictFile, "%d", &length);
  root = new struct Dict[length];
  for(i = 0; i < length; i++){
    //fscanf(dictFile, "%s %d", buf, &((root+i)->count));
	fscanf(dictFile, "%s", buf);
    (root+i)->word = new char[strlen(buf) + 1];
    strcpy((root+i)->word, buf);
  }
  fclose(dictFile);
}


//
// private void write(const string &file)
// @file:
//     
// Dump internal data structure into this file in binary format
//
void Dictionary::dump(const string &file) {
  //FILE *dump = fopen(file.c_str(), "w+");
  
  int pfd[2];
  assert(pipe(pfd) != -1);
  FILE *dump = fdopen(pfd[FD_WRITE], "w+");

  root = new struct Dict[length];
  fprintf(dump, "%d\n", length);
  preDictDump(preroot, dump);
  preDictDestroy(preroot);
  preroot = NULL;
  fclose(dump);
}

//bool Dictionary::preDictAdd(struct preDict **preroot, char *word){
void Dictionary::preDictAdd(struct preDict **preroot, char *word){
  // store pre dictionary by binary tree
  if(*preroot == NULL){
    *preroot = new struct preDict[1];
    (*preroot)->word = new char[strlen(word) + 1];
    (*preroot)->leftPtr = (*preroot)->rightPtr = NULL;
    strcpy((*preroot)->word, word);
	free(word);
    //(*preroot)->count = 1;
    //return 1;
  }else{
    if(strcmp((*preroot)->word, word) > 0){
      //return preDictAdd(&((*preroot)->leftPtr), word);
	  preDictAdd(&((*preroot)->leftPtr), word);
	  free(word);
    //}else if(strcmp((*preroot)->word, word) < 0){
	}else{
      //return preDictAdd(&((*preroot)->rightPtr), word);
	  preDictAdd(&((*preroot)->rightPtr), word);
	  free(word);
    }/*else{
      (*preroot)->count += 1;
	  free(word);
      return 0;
    }*/
  }
}

void Dictionary::preDictDump(struct preDict *preroot, FILE *dump){
  // inorder traversal to dump sorted words
  static int count = 0;
  if(preroot != NULL){
    preDictDump(preroot->leftPtr, dump);
    //fprintf(dump, "%s %d\n", preroot->word, preroot->count);
	fprintf(dump, "%s\n", preroot->word);
    //(root + count)->count = preroot->count;
    (root + count)->word = new char [strlen(preroot->word) + 1];
    strcpy((root + count)->word, preroot->word);
    preDictDump(preroot->rightPtr, dump);
  }
}

void Dictionary::preDictDestroy(struct preDict *preroot){
  if(preroot != NULL){
    preDictDestroy(preroot->leftPtr);
    preDictDestroy(preroot->rightPtr);
    delete[] preroot->word;
    delete[] preroot;
  }
}

struct Dict *Dictionary::DictFind(struct Dict *root, const char *target){
  int left = 0, right = length - 1, mid;
  while(left <= right){
    mid = (left + right) / 2;
    if(strcmp((root+mid)->word, target) > 0)
      right = mid - 1;
    else if(strcmp((root+mid)->word, target) < 0)
      left = mid + 1;
    else
      return (root+mid);
  }
  return NULL;
}
