CFLAGS += -fpic -std=c11

libluada.so: luada.o
	$(CC) -shared $< -lX11 -o $@

all: libluada.so

clean:
	$(RM) luada.o libluada.so
