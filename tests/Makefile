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

.PHONY: run-test
run-test: test
	./test

.PHONY: coverage
coverage:
	rm -f test
	make all CFLAGS_ADD="-fprofile-instr-generate -fcoverage-mapping"
	make run-test
	llvm-profdata merge default.profraw -o all.profdata
	llvm-cov show -instr-profile=all.profdata "./test" "../libsha"
	llvm-cov export -instr-profile=all.profdata -format=lcov "./test" "../libsha" > coverage.lcov

.PHONY: clean
clean:
	rm -f test munit.a **/*.o *.o
