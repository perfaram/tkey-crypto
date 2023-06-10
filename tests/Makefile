CC = clang -std=gnu99

LOCAL_INCLUDE = -I ../include/ -I .

IMAGE=ghcr.io/nowitis/tkey-deviceapp-builder:latest

CFLAGS_W = -Wall -Werror=implicit-function-declaration
CFLAGS = $(CFLAGS_W) $(LOCAL_INCLUDE)
CFLAGS_ADD:= #-fsanitize=address,undefined,memory
PODMAN_TARGET:=

all: test

MUNITOBJS=munit/munit.o munit_vectors/munit_vectors.o
libmunit.a: $(MUNITOBJS)
	llvm-ar -qc $@ $(MUNITOBJS)
$(MUNITOBJS): munit_vectors/munit_vectors.h munit/munit.h

TESTOBJS=test.c t_hmac_sha1.c t_sha1.c ../libsha/*.c 
test: libmunit.a test.o $(TESTOBJS)
	$(CC) $(CFLAGS_W) $(CFLAGS_ADD) -o $@ $(TESTOBJS) -L. -lmunit $(LOCAL_INCLUDE) 

do-test: test
	./test

.PHONY: coverage
coverage:
	make all CFLAGS_ADD="-fprofile-instr-generate -fcoverage-mapping"
	./t_hmac_sha1
	llvm-profdata merge default.profraw -o all.profdata
	llvm-cov show -instr-profile=all.profdata "./t_hmac_sha1" "../libsha/hmac_sha1.c"

.PHONY: clean
clean:
	rm -f munit.a t_hmac_sha1 **/*.o *.o
