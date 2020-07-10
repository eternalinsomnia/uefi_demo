ARCH = x86_64
CC = clang
TARGET = tetris.efi
OBJS = main.o
EFIINC = /usr/include/efi
EFIINCS = -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol
EFILIB = /usr/lib
EFI_CRT_OBJS = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS = $(EFILIB)/elf_$(ARCH)_efi.lds

CFLAGS = $(EFIINCS) -fno-stack-protector -fpic \
	-fshort-wchar -mno-red-zone -DHAVE_USE_MS_ABI -Wall

LDFLAGS = -nostdlib -znocombreloc -T $(EFI_LDS) -shared \
	-Bsymbolic -L $(EFILIB) $(EFI_CRT_OBJS)

all: $(TARGET)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c	

tetris.so: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

%.efi: %.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
 		-j .dynsym -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@
