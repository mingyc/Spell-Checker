.PHONY: all clean

TARGET=project

BZIP=pbzip2
MAIN=main.cpp
SRC=Compressor.cpp Dictionary.cpp SpellChecker.cpp 
SRC_PATH=src

MAIN:=$(addprefix $(SRC_PATH)/, $(MAIN))
OBJ=scanner.o $(SRC:.cpp=.o)

#LDFLAGS=-O3 -Wl,--enable-auto-import
LDFLAGS=-Isrc -Llib -lbz2
CFLAGS=-O3 -static -march=corei7

LEX=flex
CXX=g++
RM=rm -f


PBZ_LDFLAGS = -lbz2 -lpthread

PBZ_CFLAGS = -O3
PBZ_CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
PBZ_CFLAGS += -D_POSIX_PTHREAD_SEMANTICS
PBZ_CFLAGS += -DUSE_STACKSIZE_CUSTOMIZATION
# On some compilers -pthreads
PBZ_CFLAGS += -pthread



all: $(TARGET) $(OBJ)


$(TARGET): $(MAIN) $(OBJ) $(addprefix $(SRC_PATH)/, $(SRC))
	$(CXX) $(CFLAGS) $(MAIN) $(OBJ) $(LDFLAGS) -o $(TARGET)


$(BZIP): $(SRC_PATH)/pbzip2.cpp $(SRC_PATH)/BZ2StreamScanner.cpp
	$(CXX) $(PBZ_CFLAGS) $^ -Isrc -Llib -o $@ $(PBZ_LDFLAGS)


scanner.o: $(SRC_PATH)/scanner.l
	$(LEX) -oscanner.cpp $<
	$(CXX) -c scanner.cpp
	$(RM) scanner.cpp


SpellChecker.o: $(SRC_PATH)/SpellChecker.cpp $(SRC_PATH)/SpellChecker.h 
	$(CXX) -c $<

Dictionary.o: $(SRC_PATH)/Dictionary.cpp $(SRC_PATH)/Dictionary.h
	$(CXX) -c $<

Compressor.o: $(SRC_PATH)/Compressor.cpp $(SRC_PATH)/Compressor.h
	$(CXX) -c $<



clean:
	@$(RM) $(TARGET) $(BZIP) $(OBJ) $(TARGET).exe
