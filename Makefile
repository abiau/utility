
SRC= main.c \
	 sample.c \
	 vCommon.c \
	 vLog.c \
	 vData.c \
	 vNet.c


all:
	gcc -o exe -g -O0 $(SRC)
clean:
	rm -rf exe

