#include "SpellChecker.h"
#include "Dictionary.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

void printUsage(const char *execName)
{
  fprintf(stderr, "Usage: %s option file...\n", execName);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "\t-b [text file] [dictionary file]  Use [text file] as a reference of dictionary for spell checking, and then output the internal dictionary data structure into [dictionary file].\n");
  fprintf(stderr, "\t-d [dictionary file] [article file]  Read a generated dictionary from [dictionary file] and then do the spell checking for [article file].\n");
}


int main(int argc, char **argv)
{

  if (argc < 4) {
    printUsage(argv[0]);
    return EXIT_FAILURE;
  }

  for (int i = 1; i < argc; i+=3)
    if (strcmp(argv[i], "-b") == 0)
    {
	  Dictionary dict(argv[i+1], argv[i+2]);
      // TODO: generate dictionary from argv[i+1] and output to argv[i+2]
      //SpellChecker->Create(argv[i+1], argv[i+2]);
    }
    else if (strcmp(argv[i], "-d") == 0)
    {
      // TODO: do spell checking for the ariticle argv[i+2] by the dicionary argv[i+1]
      //SpellChecker->Suggest(argv[i+2], argv[i+1]);
    }
    else
    {
      printUsage(argv[0]);
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
