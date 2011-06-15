#include "SpellChecker.h"
#include <cstdio>
#include <map>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
using std::pair;
using std::make_pair;

extern int yylex();
extern FILE *yyin;
extern FILE *yyout;
extern char *yytext;

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
void SpellChecker::Create(const char *textName, const char *dictName) {
  build(string(textName), string(dictName));
}


//
// public vector<string> Suggest(const char *article, const char *dict)
// @article: 
// @dict:
//
// Return a list of suggestion words with higher correctness
//
void SpellChecker::Suggest(const char *articleName, const char *dictName) {

  load(string(dictName));

  FILE *article = fopen(articleName, "r");

  // Redirect yyin and close yyout
  yyin = article;
  fclose(yyout);

  while (yylex() != 0) {
    vector<string> candidates = basic_suggest(string(yytext));
    candidates.resize(10);
    //sort(candidates.begin(), candidates.end());
    fprintf(stdout, "%s:", yytext);
    foreach(string pw, candidates)
      fprintf(stdout, " %s", pw.c_str());
    fputc('\n', stdout);
  }
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
  dict.reset(new Dictionary(dictionaryFile));
}

//
// private void check(string word)
// @word:
//
bool SpellChecker::check(const string &word) {
  return dict->exist(word);
}

//
// private vector<string> basic_suggest(string word)
// @word:
//
// Return a list of suggested words from the dictionary
//
vector<string> SpellChecker::basic_suggest(const string &word) {

  unordered_set<string> candidateSet;

  // If the word is already in the dictionary, then its spelling may be correct
  if (check(word))
    candidateSet.insert(word);


  // Obtain words that with edit distance equal to 1 and also in the dictionary
  unordered_set<string> set1 = getKnownWords(getWordsWithEditDistance1(word));
  candidateSet.insert(set1.begin(), set1.end());


  // Obtain words that with edit distance equal to 2 and also in the dictionary
  unordered_set<string> set2 = getKnownWordsWithEditDistance2(word);
  candidateSet.insert(set2.begin(), set2.end());

  // The input word must be the last choice of suggestion
  candidateSet.insert(word);

  candidateSet = getMostPossibleWords(candidateSet);

  return vector<string>(candidateSet.begin(), candidateSet.end());
}


unordered_set<string> SpellChecker::getWordsWithEditDistance1(const string &word) {

  unordered_set<string> retSet;

  // Split word by characters
  typedef pair<string, string> str_pair_t;
  unordered_set<str_pair_t> splitSet;
  for (int i = 0; i < word.size(); ++i)
    splitSet.insert(make_pair<string, string>(word.substr(0, i), word.substr(i)));

  // Obtain a set of words by deleting 1 character from original string
  foreach (str_pair_t ss, splitSet) {
    if (ss.second.size())
      retSet.insert(ss.first + ss.second.substr(1));
  }

  // Obtain a set of words by transposing nearby characters from original string
  foreach (str_pair_t ss, splitSet) {
    if (ss.second.size() > 1)
      retSet.insert(ss.first + ss.second[1] + ss.second[0] + ss.second.substr(2));
  }

  // Obtain a set of words by replacing 1 characters from original string
  foreach (str_pair_t ss, splitSet) {
    for (char alphabet = 'a'; alphabet <= 'z'; ++alphabet)
      retSet.insert(ss.first + alphabet + ss.second.substr(1));
  }

  // Obtain a set of words by inserting 1 characters from original string
  foreach (str_pair_t ss, splitSet) {
    for (char alphabet = 'a'; alphabet <= 'z'; ++alphabet)
      retSet.insert(ss.first + alphabet + ss.second);
  }

  return retSet;
}



unordered_set<string> SpellChecker::getKnownWordsWithEditDistance2(const string &word) {
  unordered_set<string> retSet;
  unordered_set<string> set1 = getWordsWithEditDistance1(word);
  foreach(string w1, set1) {
    unordered_set<string> set2 = getWordsWithEditDistance1(w1);
    foreach(string w2, set2) {
      if (check(w2))
        retSet.insert(w2);
    }
  }
  return retSet;
}

unordered_set<string> SpellChecker::getKnownWords(const unordered_set<string> &words) {
  unordered_set<string> knowns;

  foreach (string w, words) {
    if (check(w))
      knowns.insert(w);
  }

  return knowns;
}



unordered_set<string> SpellChecker::getMostPossibleWords(const unordered_set<string> &words){

  pair<int, unordered_set<string> > top1, top2, top3;
  unordered_set<string> retSet ;

  foreach(string w, words) {
    int prob = 0;
    if ((prob = dict->getCount(w)) > top1.first) {
      fprintf(stderr, "count = %d, top prob = %d\n", prob, top1.first);
      // Update words set with 3rd possible probability
      top3 = top2;

      // Update words set with 2nd possible probability
      top2 = top1;

      // Update words set with the most possible probability
      top1.first = prob;
      top1.second.clear();
      top1.second.insert(w);
    }
    else if (prob == top2.first) {
      top2.second.insert(w);
    }
    else {
      if (prob >= top2.first) {
        top3 = top2;
        top2.second.insert(w);
      }
      else if (prob >= top3.first) {
        top3.second.insert(w);
      }
    }
  }

  fprintf(stderr, "top prob = %d\n", top1.first);
  fprintf(stderr, "second prob = %d\n", top2.first);
  fprintf(stderr, "third prob = %d\n", top3.first);
  retSet.insert(top1.second.begin(), top1.second.end());
  retSet.insert(top2.second.begin(), top2.second.end());
  retSet.insert(top3.second.begin(), top3.second.end());

  return retSet;
}
