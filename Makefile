TARGET = kur
CC = gcc

PREFIX_SRC = ./src/
PREFIX_OBJ = ./obj/
PREFIX_BIN = ./bin/
PREFIX_INC = ./include/
PREFIX_LIB = ./lib/

SRC = $(wildcard $(PREFIX_SRC)*.c)
OBJ = $(patsubst $(PREFIX_SRC)%.c, $(PREFIX_OBJ)%.o, $(SRC))
LIBS = $(wildcard $(PREFIX_LIB)*.a)
TAR = $(PREFIX_BIN)$(TARGET)


all : $(TAR)

clean : 
	del /q $(subst /,\, $(PREFIX_BIN)), $(subst /,\, $(PREFIX_OBJ))

exec : all
	start ".\" ".\bin\kur.exe" 

$(TAR) : $(OBJ) $(PREFIX_OBJ) $(PREFIX_BIN)
	$(CC) $(OBJ) -o $(TAR) -L $(PREFIX_LIB) $(LIBS) -pthread

$(PREFIX_OBJ):
	if not exist "$(PREFIX_OBJ)" mkdir "$(PREFIX_OBJ)"

$(PREFIX_BIN):
	if not exist "$(PREFIX_BIN)" mkdir "$(PREFIX_BIN)"

$(PREFIX_OBJ)%.o : $(PREFIX_SRC)%.c 
	$(CC) -c $< -o $@ -I $(PREFIX_INC)
