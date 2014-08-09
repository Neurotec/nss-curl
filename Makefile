CFLAGS := -fPIC -Wshadow `curl-config --libs`

all: libnss_curl.so.2

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

libnss_curl.so.2: nss_main.o http_client_curl.o config.o dohost.o dogroup.o
	$(CC) -shared -o $@ $^ -Wl,-soname,libnss_curl.so.2 -Wl,-as-needed  `curl-config --libs`

clean:
	-rm *.o
	-rm libnss_curl.so.2
