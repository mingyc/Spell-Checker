#ifndef __COMPRESSOR_H__
#define __COMPRESSOR_H__


class Compressor {
  public:
    static void Compress(int srcFd, const char *destFile);
    static void Decompress(const char *srcFile, int destFd);
  protected:
    Compressor();
    ~Compressor();
  private:
    const static int blockSize100k = 1;
    const static int verbosity = 0;
    const static int workFactor = 30;
};



#endif // __COMPRESSOR_H__
