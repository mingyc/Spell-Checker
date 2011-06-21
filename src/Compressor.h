#ifndef __COMPRESSOR_H__
#define __COMPRESSOR_H__


class Compressor {
  public:
    static void Compress(int srcFd, const char *destFile);
    static void Decompress(const char *srcFile, int destFd);
    static void ParallelCompress(const char *srcFileName, const char *destFileName) ;
    static void ParallelDecompress(const char *srcFileName, int destFd) ;
  protected:
    Compressor();
    ~Compressor();
  private:
    const static int blockSize100k = 9;
    const static int verbosity = 0;
    const static int workFactor = 30;
};



#endif // __COMPRESSOR_H__
