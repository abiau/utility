
# source files.
EXE_SRC = \
		  main.c \
		  sample.c

LIB_SRC = \
		  vCommon.c \
		  vLog.c \
		  vData.c \
		  vNet.c 
 

OBJ = $(LIB_SRC:.c=.o)
OUT = libvutil.a
EXE = exe
 
# include directories
INC = -I. 
# C compiler flags (-g -O2 -Wall)
CCFLAGS = -g -O0 -Wall -Wno-unused-variable
# compiler
CC = gcc
 
# library paths
LIB = -L. -lm -pthread
# compile flags
LDFLAGS = -g -O0
 
.SUFFIXES: .c
 
.c.o:
	$(CC) $(INC) $(CCFLAGS) -c $< -o $@
 
-$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)
	$(CC) -o $(EXE) $(CCFLAGS) $(EXE_SRC) $(OUT)
	rm -rf *.o
 
clean:
	rm -rf $(EXE) 
	rm -rf $(OUT) 
	rm -rf $(OBJ) 



