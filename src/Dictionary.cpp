#include "Dictionary.h"
#include <cstdio>

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
  load(srcBinFileName);
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
// private void read(const string &file)
// @file:
//    Given a txt file, parse it into internal data structure and prepared for querying
//
void Dictionary::read(const string &file) {
  FILE *corpus = fopen(file.c_str(), "r");


  fclose(corpus);
}




//
// private void load(const string &file)
// @file:
//    
// Load the binary file into internal memory format
//
void Dictionary::load(const string &file) {
  FILE *corpus = fopen(file.c_str(), "r");


  fclose(corpus);
}


//
// private void write(const string &file)
// @file:
//     
// Dump internal data structure into this file in binary format
//
void Dictionary::dump(const string &file) {
  FILE *dump = fopen(file.c_str(), "w+");

  fclose(dump);
}

