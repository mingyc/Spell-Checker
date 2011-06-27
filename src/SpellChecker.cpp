#include "SpellChecker.h"
#include <cstdio>
#include <cassert>
#include <map>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>
#define foreach BOOST_FOREACH

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#define FD_READ   0
#define FD_WRITE  1
using std::pair;
using std::make_pair;
using std::map;
using boost::unordered_set;

extern int yylex();
extern FILE *yyin;
extern int spelltextleng;
extern char *spelltext;
extern char *dicttext;

#define yyconst const
typedef struct yy_buffer_state *YY_BUFFER_STATE;
typedef size_t yy_size_t;
extern YY_BUFFER_STATE yy_scan_bytes(yyconst char *bytes, int __yybytes_len);
extern void yy_switch_to_buffer(YY_BUFFER_STATE new_buffer);
extern void yy_delete_buffer(YY_BUFFER_STATE b);

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
  
  const int OUTPUT_BUF_SIZE = 1048576; // 1 MB dynamic memory
  //char *outbuf = (char *)malloc(sizeof(char) * OUTPUT_BUF_SIZE);
  char outbuf[OUTPUT_BUF_SIZE] = {'\0'};
  int outbuf_i = 0;

  load(string(dictName));

  bool printed;
  unordered_set<string> appeared;


  const int FILE_SPLIT_NUM = 10; 
  int articleFd = open(articleName, O_RDONLY);
  struct stat articleStat;
  fstat(articleFd, &articleStat);
  int articleSize = articleStat.st_size;
  char *article = (char *)mmap(0, articleSize, PROT_READ, MAP_PRIVATE, articleFd, 0);


  int startPos[FILE_SPLIT_NUM+1] = {0};
  const int splitSize = articleSize/FILE_SPLIT_NUM;
  // split the article into FILE_SPLIT_NUM parts
  for (int i = 1; i < FILE_SPLIT_NUM; ++i) {
    startPos[i] = startPos[i-1] + splitSize;
    while (article[ startPos[i] ] != ' ') startPos[i]++;
  }
  startPos[FILE_SPLIT_NUM] = articleSize;

  YY_BUFFER_STATE articleBuf[FILE_SPLIT_NUM];
  for (int i = 0; i < FILE_SPLIT_NUM; ++i)
    articleBuf[i] = yy_scan_bytes(article + startPos[i], startPos[i+1]-startPos[i]);

  pid_t cpid[FILE_SPLIT_NUM];
  int pfd[FILE_SPLIT_NUM][2];

  for (int i = 0; i < FILE_SPLIT_NUM; ++i) 
  {
    pipe(pfd[i]);

    if ((cpid[i] = fork()) == 0) { /* i-th child process */
      yy_switch_to_buffer(articleBuf[i]);

      close(pfd[i][FD_READ]);

      outbuf_i = 0;
      while (yylex() != 0) 
      {
        int wordlen = spelltextleng;
        string caselessWord(dicttext);
        string word(spelltext);

        if ( !wordlen || check(caselessWord)) continue;

        if (appeared.find(caselessWord) == appeared.end()) {

          appeared.insert(caselessWord);


          vector<string> candidates = basic_suggest(caselessWord);
          //candidates.resize(querySize);
          sort(candidates.begin(), candidates.end());

          printed = false;

          //fprintf(stdout, "%s:", spelltext);
          memcpy(outbuf+outbuf_i, spelltext, spelltextleng);
          outbuf_i += spelltextleng;
          outbuf[outbuf_i++] = ':';

          foreach(string pw, candidates) {

            //fprintf(stdout, " %s", pw.c_str());
            int len = pw.size();
            outbuf[outbuf_i++] = ' ';
            memcpy(outbuf+outbuf_i, pw.c_str(), len);
            outbuf_i += len;

            printed = true;
          }
          if (!printed) {
            //fputc(' ', stdout);
            outbuf[outbuf_i++] = ' ';
          }
        }
        else {
          //fprintf(stdout, "%s\n", spelltext);
          memcpy(outbuf+outbuf_i, spelltext, spelltextleng);
          outbuf_i += spelltextleng;
          outbuf[outbuf_i++] = ':';
        }
        //fputc('\n', stdout);
        outbuf[outbuf_i++] = '\n';
      }
      int nWritten;
      for (nWritten = 0; (nWritten += write(pfd[i][FD_WRITE], outbuf+nWritten, outbuf_i-nWritten)) < outbuf_i;);
      close(pfd[i][FD_WRITE]);
      _exit(EXIT_SUCCESS);
    } 
    else if (cpid[i] > 0) { /* main process */

      close(pfd[i][FD_WRITE]);

    }
    else {
      /* error */
    }
  }

  fd_set fdState;
  struct timeval TLE;
  FD_ZERO(&fdState);

  int now_i = 0;

  while (now_i < FILE_SPLIT_NUM) 
  {

    FD_SET(pfd[now_i][FD_READ], &fdState);
    TLE.tv_sec = 0;
    TLE.tv_usec = 500;

    int ret = select( pfd[now_i][FD_READ]+1, &fdState, NULL, NULL, &TLE );
    if (ret > 0) {
      if (FD_ISSET(pfd[now_i][FD_READ], &fdState)) {
        int nRead = 0; 
        while ((nRead = read(pfd[now_i][FD_READ], outbuf, OUTPUT_BUF_SIZE)) > 0) 
          for (int nWritten = 0; (nWritten += write(STDOUT_FILENO, outbuf+nWritten, nRead-nWritten)) < nRead;);
        now_i++;
      }
    }
    else if (ret == 0) {
    }
    else {
      /* error */
    }
  }


  //for (int nWritten = 0; (nWritten += write(STDOUT_FILENO, outbuf+nWritten, outbuf_i-nWritten)) < outbuf_i;);

  for (int i = 0; i < FILE_SPLIT_NUM; ++i)
    yy_delete_buffer(articleBuf[i]);
  munmap(article, articleSize);

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

  vector<string> candidateSet;

  unordered_set<string> appeared;
  
  // If the word is already in the dictionary, then its spelling may be correct
  if (check(word)) {
    candidateSet.push_back(word);
    appeared.insert(word);
  }


  // Obtain words that with edit distance equal to 1 and also in the dictionary
  vector<string> set1 = getKnownWords(getWordsWithEditDistance1(word));
  foreach(string w, set1) {
    if (appeared.find(w) == appeared.end()) {
      candidateSet.push_back(w);
      appeared.insert(w);
    }
  }


  // Obtain words that with edit distance equal to 2 and also in the dictionary
  /* TODO: Fix the problem caused by words with possibility greater than ED=1 */
  /*
  vector<string> set2 = getKnownWordsWithEditDistance2(word);
  foreach(string w, set2) {
    if (appeared.find(w) == appeared.end()) {
      candidateSet.push_back(w);
      appeared.insert(w);
    }
  }
  */

  // The input word must be the last choice of suggestion
  /* 20110617 Commented by Milnex: the spec doesn't require this
  if (appeared.find(word) == appeared.end()) {
    candidateSet.push_back(word);
    appeared.insert(word);
  }
  */

  //candidateSet = getMostPossibleWords(candidateSet, querySize);

  return candidateSet;
}


vector<string> SpellChecker::getWordsWithEditDistance1(const string &word) {

  const int EXPECT_SIZE = word.length()-1 + word.length()-2 + word.length()*26 + word.length()+2;
  vector<string> retSet;
  //retSet.reserve(EXPECT_SIZE);

  // Split word by characters
  typedef pair<string, string> str_pair_t;
  vector<str_pair_t> splitSet;
  for (int i = 0; i < word.size(); ++i)
    splitSet.push_back(make_pair<string, string>(word.substr(0, i), word.substr(i)));
  splitSet.push_back(make_pair<string, string>(word, string("")));



  // Obtain a set of words by deleting 1 character from original string
  foreach (str_pair_t ss, splitSet) {
    if (ss.second.size())
      retSet.push_back(ss.first + ss.second.substr(1));
  }

  // Obtain a set of words by transposing nearby characters from original string
  foreach (str_pair_t ss, splitSet) {
    if (ss.second.size() > 1)
      retSet.push_back(ss.first + ss.second[1] + ss.second[0] + ss.second.substr(2));
  }

  // Obtain a set of words by replacing 1 characters from original string
  foreach (str_pair_t ss, splitSet) {
    for (char alphabet = 'a'; alphabet <= 'z'; ++alphabet)
      if (ss.second.size() > 1)
        retSet.push_back(ss.first + alphabet + ss.second.substr(1));
      else
        retSet.push_back(ss.first + alphabet);
  }


  // Obtain a set of words by inserting 1 characters from original string
  foreach (str_pair_t ss, splitSet) {
    for (char alphabet = 'a'; alphabet <= 'z'; ++alphabet)
      retSet.push_back(ss.first + alphabet + ss.second);
  }


  return retSet;
}



vector<string> SpellChecker::getKnownWordsWithEditDistance2(const string &word) {
  vector<string> retSet;
  unordered_set<string> appeared;

  vector<string> set1 = getWordsWithEditDistance1(word);
  foreach(string w1, set1) {
    vector<string> set2 = getWordsWithEditDistance1(w1);
    foreach(string w2, set2) {
      if (appeared.find(w2) == appeared.end() && check(w2))
        retSet.push_back(w2);
        appeared.insert(w2);
    }
  }

  return retSet;
}

vector<string> SpellChecker::getKnownWords(const vector<string> &words) {
  vector<string> knowns;
  knowns.reserve(words.size());

  foreach (string w, words) {
    if (check(w))
      knowns.push_back(w);
  }

  knowns.resize(knowns.size());

  return knowns;
}



vector<string> SpellChecker::getMostPossibleWords(const vector<string> &words, const int &querySize){

  vector<string> retSet ;
  map<int, vector<string>, std::greater<int> > countWordsMap;

  foreach(string w, words) {
    countWordsMap[dict->getCount(w)].push_back(w);
  }

  typedef pair<int, vector<string> > istrs_pair_t;
  int count = 0;
  foreach(istrs_pair_t iws, countWordsMap) {
    if (count > querySize) break;
    retSet.insert(retSet.end(), iws.second.begin(), iws.second.end());
    count += iws.second.size();
  }

  return retSet;
}
