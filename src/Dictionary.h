#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <string>
using std::string;

// pre Dictionary
struct preDict{
	char *word;
	int count;
	struct preDict *leftPtr;
	struct preDict *rightPtr;
};

// Dictionary
struct Dict{
	int count;
	char *word;
};

class Dictionary {

  public:
    Dictionary(const string &srcFile, const string &destFile);
    ~Dictionary();
    bool exist(const string &word);
    int  getCount(const string &word);

  private:
    void load(const string &file);
    void write(const string &file);
	void preDictAdd(struct preDict **root, const char *word);
	void preDictDump(struct preDict *root, FILE *dump);
	void preDictDestory(struct preDict *preroot);
	struct Dict *DictFind(struct Dict *root, const char *target);
	
  private:
	struct preDict *preroot;
	struct Dict *root;
	int length;
};


#endif // __DICTIONARY_H__
