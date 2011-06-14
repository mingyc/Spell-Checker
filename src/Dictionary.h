#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <string>
using std::string;




class Dictionary {

  public:
    Dictionary(const string &srcFile, const string &destFile);
    ~Dictionary();
    bool exist(const string &word);
    int  getCount(const string &word);

  private:
    void load(const string &file);
    void write(const string &file);

  private:
};


#endif // __DICTIONARY_H__
