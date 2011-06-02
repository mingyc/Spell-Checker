.PHONY: all clean

TARGET=project

MAIN=main.cpp
SRC=SpellChecker.cpp
SRC_PATH=src

MAIN:=$(addprefix $(SRC_PATH)/, $(MAIN))
OBJ=$(SRC:.cpp=.o)

LDFLAGS=-O3
CFLAGS=
CXX=g++
RM=rm



all: $(TARGET) $(OBJ)


$(TARGET): $(MAIN) $(addprefix $(SRC_PATH)/, $(SRC))  $(OBJ)
	$(CXX) $(LDFLAGS) $(MAIN) $(OBJ) -o $(TARGET)
	$(RM) $(OBJ)


SpellChecker.o: $(SRC_PATH)/SpellChecker.cpp $(SRC_PATH)/SpellChecker.h 
	$(CXX) -c $<


clean:
	@$(RM) $(TARGET) $(TARGET).exe
