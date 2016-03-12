CC = mingw32-g++ -std=c++14

BUILD_TARGET = ../bin/LittleVillage.exe	

LIBRARIES = -llua -lncurses

REBUILDABLES = *.o
	
SOURCE = 

all :
	$(CC) main.cpp $(LIBRARIES) -o $(BUILD_TARGET)
	@echo Complete!
	@echo 
	
clean:
	rm -f $(REBUILDABLES) $(BUILD_TARGET)
