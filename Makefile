
SRC= main.c \
	 vCommon.c \
	 vLog.c \
	 vNet.c


all:
	gcc -o exe -g -O0 $(SRC)
clean:
	rm -rf exe

