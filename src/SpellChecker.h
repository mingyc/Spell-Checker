#ifndef __SPELLCHECKER__
#define __SPELLCHECKER__

#include <vector>
#include <string>
using std::string;
using std::vector;


class SpellChecker {

  public:
    // Default Constructor
    SpellChecker();
    // Default Destructor
    ~SpellChecker();

    void Create(const char *text, const char *dict) {
    }
    vector<string> Suggest(const char *article, const char *dict) {
    }

  private:
    virtual void build(string textFile, string dictionaryFile) = 0;
    virtual void load(string dictionaryFile) = 0;
    virtual void check() = 0;
    virtual vector<string> basic_suggest(string word) = 0;

};



#endif // __SPELLCHECKER__
