.PHONY: all clean

TARGET=project

MAIN=main.cpp
SRC=Dictionary.cpp SpellChecker.cpp 
SRC_PATH=src

MAIN:=$(addprefix $(SRC_PATH)/, $(MAIN))
OBJ=scanner.o $(SRC:.cpp=.o)

#LDFLAGS=-O3 -Wl,--enable-auto-import
LDFLAGS=-O3
CFLAGS=

LEX=flex
CXX=g++
RM=rm



all: $(TARGET) $(OBJ)


$(TARGET): $(MAIN) $(OBJ) $(addprefix $(SRC_PATH)/, $(SRC))
	$(CXX) $(LDFLAGS) $(MAIN) $(OBJ) -o $(TARGET)
	@$(RM) $(OBJ)


scanner.o: $(SRC_PATH)/scanner.l
	$(LEX) -oscanner.cpp $<
	$(CXX) -c scanner.cpp
	@$(RM) scanner.cpp


SpellChecker.o: $(SRC_PATH)/SpellChecker.cpp $(SRC_PATH)/SpellChecker.h 
	$(CXX) -c $<

Dictionary.o: $(SRC_PATH)/Dictionary.cpp $(SRC_PATH)/Dictionary.h
	$(CXX) -c $<


clean:
	@$(RM) $(TARGET) $(TARGET).exe
