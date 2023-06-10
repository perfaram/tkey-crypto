OBJCOPY ?= llvm-objcopy

IMAGE=ghcr.io/nowitis/tkey-deviceapp-builder:latest

P := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
LIBDIR ?= $(P)/../tkey-libs

CC = clang -std=gnu99

TKLIB_INCLUDE = $(LIBDIR)/include
LOCAL_INCLUDE = -I include/ -I .
CINCLUDES = -I $(TKLIB_INCLUDE) $(LOCAL_INCLUDE)

CFLAGS_W = -Wall -Werror=implicit-function-declaration
# If you want libcommon's qemu_puts() et cetera to output something on our QEMU
# debug port, remove -DNODEBUG below
CFLAGS = -target riscv32-unknown-none-elf -march=rv32iczmmul -mabi=ilp32 -mcmodel=medany \
   -static -O2 -ffast-math -fno-common -fno-builtin-printf \
   -fno-builtin-putchar -nostdlib -mno-relax -flto \
   $(CFLAGS_W) \
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

podman-lintfix:
	podman run --rm --mount type=bind,source=$(CURDIR),target=/src --mount type=bind,source=$(CURDIR)/../tkey-libs,target=/tkey-libs -w /src -it $(IMAGE) make -j lintfix

inpodman:
	podman run --rm --mount type=bind,source=$(CURDIR),target=/src --mount type=bind,source=$(CURDIR)/../tkey-libs,target=/tkey-libs -w /src -it $(IMAGE) make -j $(PODMAN_TARGET)

# Arithmetic lib
ARITHMOBJS=libarithmetic/div.o
libarithmetic/libarithmetic.a: $(ARITHMOBJS)
	llvm-ar -qc $@ $(ARITHMOBJS)
$(ARITHMOBJS): $(TKLIB_INCLUDE)/types.h

# OATH lib
SHAOBJS=libsha/sha1.o libsha/hmac_sha1.o
libsha/libsha.a: $(SHAOBJS)
	llvm-ar -qc $@ $(SHAOBJS)
$(SHAOBJS): $(TKLIB_INCLUDE)/types.h $(TKLIB_INCLUDE)/lib.h include/sha1.h include/hmac_sha1.h

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
.PHONY: lintfix
lintfix:
	clang-tidy -fix-errors $(FMTFILES) -- $(CINCLUDES)
