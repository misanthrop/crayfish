CFLAGS += -fpic -std=c11

crayfish.so: xlib.o crayfish.o
	$(CC) -shared $^ -lX11 -o $@

crayfish.dll: winapi.o crayfish.o
	$(CC) -shared $^ -llua52 -luser32 -o $@

all: crayfish.dll

clean:
	$(RM) *.o crayfish.so crayfish.dll
