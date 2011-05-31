#include <cstdio>



void printUsage(char *execName)
{
  fprintf(stderr, "Usage: %s option file...\n", execName);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "\t-b [text file] [dictionary file]  Use [text file] as a reference of dictionary for spell checking, and then output the internal dictionary data structure into [dictionary file].\n");
  fprintf(stderr, "\t-d [dictionary file] [article file]  Read a generated dictionary from [dictionary file] and then do the spell checking for [article file].\n");
}


int main(int argc, char **argv)
{

  if (argc < 2) {
    printUsage(argv[0]);
    return EXIT_FAILURE;
  }

  for (int i = 1; i < argc; i+=2)
    if (strcmp(argv[i], "-b") == 0)
    {
      // TODO: generate dictionary from argv[i+1], argv[i+2]
      break;
    }
    else if (strcmp(argv[i], "-d") == 0)
    {
      // TODO: do spell checking for the ariticle argv[i+2] by the dicionary argv[i+1]
      break;
    }
    else
    {
      printUage(argv[0]);
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
