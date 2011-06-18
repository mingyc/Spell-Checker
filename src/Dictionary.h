#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <string>

#define DICT_WIDTH 26

using std::string;

// pre Dictionary
/*struct preDict{
  char *word;
  int count;
  struct preDict *leftPtr;
  struct preDict *rightPtr;
};*/

// Dictionary
struct Dict{
  bool exist;
  struct Dict *next[DICT_WIDTH];
};

class Dictionary {

  public:
    //
    // public Dictionary(const string &srcTxtFile, const string &destBinFile)
    // @srcTxtFile:
    //    The name of a text file worked as the source of this dictionary
    // @destBinFile:
    //    The name of where this dictionary should dump the internal data structure
    // The constructor will read from srcTxtFile, build a dictionary to be queried and then dump the whole
    // data structure into destBinFile
    //
    Dictionary(const string &srcTxTFileName, const string &destBinFileName);

    //
    // public Dictionary(const string &srcBinFileName)
    // @srcBinFile:
    //
    // The constructor will load a binary file which is of the same formatted as internal data structure
    //
    Dictionary(const string &srcBinFileName);
    ~Dictionary();


    //
    // public bool exist(const string &word)
    // @word:
    //    The word to be checked if it is in this dictionary or not
    // Return true if found in internal structure
    //
    bool exist(const string &word);

    //
    // public int getCount(const string &word)
    // @word:
    //    The word to be counted in this dictionary
    // Return the number of apperances of this word
    //
    int  getCount(const string &word);


  private:
    //
    // private void read(const string &file)
    // @file:
    //    Given a txt file, parse it into internal data structure and prepared for querying
    //
    void read(const string &file);

    //
    // private void load(const string &file)
    // @file:
    //    
    // Load the binary file into internal memory format
    //
    void load(const string &file);

    //
    // private void write(const string &file)
    // @file:
    //     
    // Dump internal data structure into this file in binary format
    //
    void dump(const string &file);

    //bool preDictAdd(struct preDict **root, char *word);
	//void preDictAdd(struct preDict **root, char *word);
    //void preDictDump(struct preDict *root, FILE *dump);
    //void preDictDestroy(struct preDict *preroot);
    void DictAdd(struct Dict *root, char *word);
	bool DictFind(struct Dict *root, const char *target);
	void DictDestroy(struct Dict *root);
	void DictDump(struct Dict *root, FILE *dump);

  private:

    //struct preDict *preroot;
    struct Dict *root;
    //int length;
};


#endif // __DICTIONARY_H__
