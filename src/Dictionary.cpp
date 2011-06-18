#include "Dictionary.h"
#include "Compressor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <sys/wait.h>
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
  //preroot = NULL;
  root = (struct Dict *)calloc(1, sizeof(struct Dict));
  //length = 0;
  read(srcTxtFileName);
  dump(destBinFileName);
  //printf("construct OK\n");
}

//
// public Dictionary(const string &srcBinFileName)
// @srcBinFile:
//
// The constructor will load a binary file which is of the same formatted as internal data structure
//
Dictionary::Dictionary(const string &srcBinFileName) {
  root = (struct Dict *)calloc(1, sizeof(struct Dict));
  //length = 0;
  load(srcBinFileName);
}

Dictionary::~Dictionary() {
  //DictDestroy(root);
}

//
// public bool exist(const string &word)
// @word:
//    The word to be checked if it is in this dictionary or not
// Return true if found in internal structure
//
bool Dictionary::exist(const string &word) {
  //return (bool)DictFind(root, word.c_str());
  return DictFind(root, word.c_str());
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
	//printf("insert word: %s\n", dicttext);
    DictAdd(root, dicttext);
    //length++;
  }

  fclose(corpus);
  //printf("read OK\n");
}


//
// private void load(const string &file)
// @file:
//    
// Load the binary file into internal memory format
//
void Dictionary::load(const string &file) {
  //FILE *dictFile = fopen(file.c_str(), "r");

/*  int i;
  char buf[256];
  pid_t cpid = 0;
  int pfd[2];
  assert(pipe(pfd) != -1);
  FILE *dictFile = fdopen(pfd[FD_READ], "r");
  
  if ((cpid = fork()) == 0) { // child process 

    // Communicate with the compressor by pipe
    Compressor::Decompress(file.c_str(), pfd[FD_WRITE]);
    _exit(EXIT_SUCCESS);
  }
  else if (cpid > 0) { // main process

    fscanf(dictFile, "%d", &length);
    root = new struct Dict[length];
    for(i = 0; i < length; i++){
      //fscanf(dictFile, "%s %d", buf, &((root+i)->count));
      fscanf(dictFile, "%s", buf);
      (root+i)->word = new char[strlen(buf) + 1];
      strcpy((root+i)->word, buf);
    }
    fclose(dictFile);
    wait(NULL);
  }
  else {
    // error 
  }*/
}


//
// private void write(const string &file)
// @file:
//     
// Dump internal data structure into this file in binary format
//
void Dictionary::dump(const string &file) {
  //FILE *dump = fopen(file.c_str(), "w+");
  
  pid_t cpid = 0;
  int pfd[2];
  assert(pipe(pfd) != -1);
  FILE *dump = fdopen(pfd[FD_WRITE], "w");
  //FILE *testout = fdopen(1, "w");
	

  if ((cpid = fork()) == 0) { /* child process */
    // Communicate with the bzip2 compressor by pipe
    close(pfd[FD_WRITE]);
    Compressor::Compress(pfd[FD_READ], file.c_str());
    _exit(EXIT_SUCCESS);
  }
  else if (cpid > 0) { /* parent process */
    close(pfd[FD_READ]);
    //root = new struct Dict[length];
    //fprintf(dump, "%d\n", length);
    //preDictDump(preroot, dump);
    DictDump(root, dump);
	fclose(dump);
    wait(NULL);

    //preDictDestroy(preroot);
    //preroot = NULL;
  }
  else { /* error */
  }
	//printf("DictDump OK\n");
}

//bool Dictionary::preDictAdd(struct preDict **preroot, char *word){
/*void Dictionary::preDictAdd(struct preDict **preroot, char *word){
  // store pre dictionary by binary tree
  if(*preroot == NULL){
    *preroot = new struct preDict[1];
    (*preroot)->word = new char[strlen(word) + 1];
    (*preroot)->leftPtr = (*preroot)->rightPtr = NULL;
    strcpy((*preroot)->word, word);
    //(*preroot)->count = 1;
    //return 1;
  }else{
    if(strcmp((*preroot)->word, word) > 0){
      //return preDictAdd(&((*preroot)->leftPtr), word);
	  preDictAdd(&((*preroot)->leftPtr), word);
    //}else if(strcmp((*preroot)->word, word) < 0){
	}else{
      //return preDictAdd(&((*preroot)->rightPtr), word);
	  preDictAdd(&((*preroot)->rightPtr), word);
    }
  }
}*/

/*void Dictionary::preDictDump(struct preDict *preroot, FILE *dump){
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
}*/

/*void Dictionary::preDictDestroy(struct preDict *preroot){
  if(preroot != NULL){
    preDictDestroy(preroot->leftPtr);
    preDictDestroy(preroot->rightPtr);
    delete[] preroot->word;
    delete[] preroot;
  }
}*/

void Dictionary::DictAdd(struct Dict *root, char *word){
	int i;
	struct Dict* currentPtr = root;

	for(i = 0; ; i++){
		if(word[i] == '\0'){
			currentPtr->exist = true;
			break;
		}else{
			//printf("word[i] - 'a' = %d\n", word[i] - 'a');
			if(currentPtr->next[word[i]-'a'] == NULL){
				currentPtr->next[word[i]-'a'] = (struct Dict *)calloc(1, sizeof(struct Dict));
			}
			currentPtr = currentPtr->next[word[i]-'a'];
		}
	}
}

void Dictionary::DictDump(struct Dict *root, FILE *dump){
	int i;
	bool leaf = true;

	//printf("#%p\t", root);
	//printf("#value %d\n", root->exist);
	for(i = 0; i < DICT_WIDTH; i++){
		//printf("for loop\n");
		if(root->next[i] != NULL){
			if(leaf == true){
				leaf = false;
				//fputc('(', dump);
				fprintf(dump, "(");
			}
			if(root->next[i]->exist == true){
				//fputc(toupper(i+'a'), dump);
				fprintf(dump, "%c", toupper(i+'a'));
			}else{
				//fputc(i+'a', dump);
				fprintf(dump, "%c", i+'a');
			}
			DictDump(root->next[i], dump); 
		}
	}
	if(leaf == false){
		fprintf(dump, ")");
		//fputc(')', dump);
	}
}

void Dictionary::DictDestroy(struct Dict *preroot){

}


bool Dictionary::DictFind(struct Dict *root, const char *target){
  /*int left = 0, right = length - 1, mid;
  while(left <= right){
    mid = (left + right) / 2;
    if(strcmp((root+mid)->word, target) > 0)
      right = mid - 1;
    else if(strcmp((root+mid)->word, target) < 0)
      left = mid + 1;
    else
      return (root+mid);
  }
  return NULL;*/
  int i;
  struct Dict *currentPtr = root;

  for(i = 0; ; i++){
  	if(target[i] == '\0'){
		if(currentPtr->exist == true){
			return true;
		}else{
			return false;
		}
	}else{
		if(currentPtr->next[target[i]-'a'] == NULL){
			return false;
		}else{
			currentPtr = currentPtr->next[target[i]-'a'];
		}
	}
  }

}

