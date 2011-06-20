#ifndef __SPELLCHECKER_H__
#define __SPELLCHECKER_H__

#include "Dictionary.h"

#include <vector>
#include <string>
#include <memory>
#include <map>
using std::vector;
using std::string;
using std::auto_ptr;


class SpellChecker {

  public:

    static SpellChecker *getInstance() {
      if (_instance.get() == 0)
        _instance.reset(new SpellChecker());
      return _instance.get();
    }

    void Create(const char *textName, const char *dictName);
    void Suggest(const char *articleName, const char *dictName);
    

  private:
    void build(const string &textFile, const string &dictionaryFile);
    void load(const string &idictionaryFile);
    bool check(const string &word);
    vector<string> basic_suggest(const string &word);


    vector<string> getWordsWithEditDistance1(const string &word);
    vector<string> getKnownWordsWithEditDistance2(const string &word);
    vector<string> getKnownWords(const vector<string> &words);
    vector<string> getMostPossibleWords(const vector<string> &words, const int &querySize);


  protected:
    SpellChecker();
    ~SpellChecker();

  private:
    friend class auto_ptr<SpellChecker>;
    // This class member save the only one instance while executing time.
    // The usage is known as the Singleton Pattern.
    static auto_ptr<SpellChecker> _instance;

    auto_ptr<Dictionary>          dict;
};



#endif // __SPELLCHECKER_H__
