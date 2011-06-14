#include "Dictionary.h"
#include <cstdio>


Dictionary::Dictionary(const string &srcFile, const string &destFile) {
  load(srcFile);
  write(destFile);
}


Dictionary::~Dictionary() {
}


//
// public bool exist(const string &word)
// @word:
//    The word to be checked if it is in this dictionary or not
// Return true if found in internal structure
//
bool Dictionary::exist(const string &word) {

}


//
// public int getCount(const string &word)
// @word:
//    The word to be counted in this dictionary
// Return the number of apperances of this word
//
int  Dictionary::getCount(const string &word) {

}


//
// private void load(const string &file)
// @file:
//    Given a txt file, load it into internal data structure and prepared for querying
//
void Dictionary::load(const string &file) {
  FILE *corpus = fopen(file.c_str(), "r");


  fclose(corpus);
}


//
// private void write(const string &file)
// @file:
//     Write internal data structure into this file in binary format
//
void Dictionary::write(const string &file) {
  FILE *dump = fopen(file.c_str(), "w+");

  fclose(dump);
}

