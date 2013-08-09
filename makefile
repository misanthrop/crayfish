CFLAGS += -fpic -std=c11

luada.so: luada-xlib.o
	$(CC) -shared $< -lX11 -o $@

luada.dll: luada-winapi.o
	$(CC) -shared $< -llua52 -luser32 -o $@

all: luada.dll

clean:
	$(RM) *.o luada.so luada.dll
