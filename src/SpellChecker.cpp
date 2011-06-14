#include "SpellChecker.h"

auto_ptr<SpellChecker> SpellChecker::_instance;


SpellChecker::SpellChecker() {

}
SpellChecker::~SpellChecker() {

}

//
// public void Create(const char *text, const char *dict)
// @text: 
// @dict:
//
void SpellChecker::Create(const char *text, const char *dict) {
  build(string(text), string(dict));
}


//
// public vector<string> Suggest(const char *article, const char *dict)
// @article: 
// @dict:
//
// Return a list of suggestion words with higher correctness
//
vector<string> SpellChecker::Suggest(const char *article, const char *dict) {

}




//
// private void build(string textFile, string dictionaryFile)
// @textFile: 
// @dictionary:
//
void SpellChecker::build(const string &textFile, const string &dictionaryFile) {
  dict.reset(new Dictionary(textFile, dictionaryFile));
}


//
// private void load(string dictionaryFile)
// @dictionaryFile:
//
void SpellChecker::load(const string &dictionaryFile) {

}

//
// private void check(string word)
// @word:
//
void SpellChecker::check(const string &word) {

}

//
// private vector<string> basic_suggest(string word)
// @word:
//
vector<string> SpellChecker::basic_suggest(const string &word) {

}
