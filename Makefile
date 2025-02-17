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

$(TAR) : $(OBJ)
	$(CC) $(OBJ) -o $(TAR) -L $(PREFIX_LIB) $(LIBS)

$(PREFIX_OBJ)%.o : $(PREFIX_SRC)%.c 
	$(CC) -c $< -o $@ -I $(PREFIX_INC)

clean : 
	del /q $(subst /,\, $(TAR)).exe, $(subst /,\, $(PREFIX_OBJ))*