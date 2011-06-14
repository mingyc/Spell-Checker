#ifndef __SPELLCHECKER_H__
#define __SPELLCHECKER_H__

#include "Dictionary.h"

#include <vector>
#include <string>
#include <memory>
using std::string;
using std::vector;
using std::auto_ptr;


class SpellChecker {

  public:

    static SpellChecker *getInstance() {
      if (_instance.get() == 0)
        _instance.reset(new SpellChecker());
      return _instance.get();
    }

    void Create(const char *text, const char *dict);
    vector<string> Suggest(const char *article, const char *dict);
    

  private:
    void build(const string &textFile, const string &dictionaryFile);
    void load(const string &idictionaryFile);
    void check(const string &word);
    vector<string> basic_suggest(const string &word);


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
