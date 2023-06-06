OBJCOPY ?= llvm-objcopy

IMAGE=ghcr.io/nowitis/tkey-deviceapp-builder:latest

CC = clang

P := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
LIBDIR ?= $(P)/../tkey-libs

CC = clang

INCLUDE=$(LIBDIR)/include
CINCLUDES=-I $(INCLUDE) -I include/ -I .

# If you want libcommon's qemu_puts() et cetera to output something on our QEMU
# debug port, remove -DNODEBUG below
CFLAGS = -target riscv32-unknown-none-elf -march=rv32iczmmul -mabi=ilp32 -mcmodel=medany \
   -static -std=gnu99 -O2 -ffast-math -fno-common -fno-builtin-printf \
   -fno-builtin-putchar -nostdlib -mno-relax -flto \
   -Wall -Werror=implicit-function-declaration \
   $(CINCLUDES) \
   -DNODEBUG

AS = clang
ASFLAGS = -target riscv32-unknown-none-elf -march=rv32iczmmul -mabi=ilp32 -mcmodel=medany -mno-relax

RM=/bin/rm

.PHONY: all
all: libarithmetic/libarithmetic.a libsha/libsha.a

podman:
	podman run --rm --mount type=bind,source=$(CURDIR),target=/src --mount type=bind,source=$(CURDIR)/../tkey-libs,target=/tkey-libs -w /src -it $(IMAGE) make -j

podman-lint:
	podman run --rm --mount type=bind,source=$(CURDIR),target=/src --mount type=bind,source=$(CURDIR)/../tkey-libs,target=/tkey-libs -w /src -it $(IMAGE) make -j lint

# Arithmetic lib
ARITHMOBJS=libarithmetic/div.o
libarithmetic/libarithmetic.a: $(ARITHMOBJS)
	llvm-ar -qc $@ $(ARITHMOBJS)
$(ARITHMOBJS): $(INCLUDE)/types.h

# OATH lib
SHAOBJS=libsha/sha1.o libsha/hmac_sha1.o
libsha/libsha.a: $(SHAOBJS)
	llvm-ar -qc $@ $(SHAOBJS)
$(SHAOBJS): $(INCLUDE)/types.h $(INCLUDE)/lib.h include/sha1.h include/hmac_sha1.h

CRYPTOLIBOBJS=$(ARITHMOBJS) $(SHAOBJS)
CRYPTOLIBS=libarithmetic/libarithmetic.a libsha/libsha.a

.PHONY: clean
clean:
	rm -f $(CRYPTOLIBS) $(CRYPTOLIBOBJS)

# Uses ../.clang-format
FMTFILES=include/*.h libsha/*.c
.PHONY: fmt
fmt:
	clang-format --dry-run --ferror-limit=0 $(FMTFILES)
	clang-format --verbose -i $(FMTFILES)
.PHONY: checkfmt
checkfmt:
	clang-format --dry-run --ferror-limit=0 --Werror $(FMTFILES)

# Uses ../.clang-tidy
.PHONY: lint
lint:
	clang-tidy $(FMTFILES) -- $(CINCLUDES)
