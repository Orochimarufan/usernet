CC=clang
SUDO=sudo

usernet: usernet.c
	$(CC) -o usernet usernet.c
	$(SUDO) chown root usernet
	$(SUDO) chmod u+s usernet

printuids: printuids.c
	$(CC) -o printuids printuids.c

all: usernet printuids

