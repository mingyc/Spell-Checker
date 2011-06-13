#ifndef __SPELLCHECKER__
#define __SPELLCHECKER__

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
    void build(string textFile, string dictionaryFile);
    void load(string dictionaryFile);
    void check();
    vector<string> basic_suggest(string word);


  protected:
    SpellChecker();
    ~SpellChecker();

  private:
    friend class auto_ptr<SpellChecker>;
    // This class member save the only one instance while executing time.
    // The usage is known as the Singleton Pattern.
    static auto_ptr<SpellChecker> _instance;
};



#endif // __SPELLCHECKER__
