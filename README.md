# uefi_tetris
![](out.gif)
# Build
```
$ git clone https://github.com/eternalinsomnia/uefi_tetris  
$ cd uefi_tetris  
$ make
```
# Launch
## QEMU
```
$ apt-get install qemu ovmf
$ qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -net none -drive format=raw,file=fat:rw:~/uefi_tetris/  
Shell> fs0:tetris.efi
```
## VirtualBox/VMWare Workstation
### Create ISO
```
$ fallocate -l 64M image.iso
$ mkfs.vfat -F32 image.iso
$ mkdir -p mnt
$ mount image.iso mnt/
$ mkdir -p mnt/EFI/BOOT
$ cp tetris.efi mnt/EFI/BOOT/BOOTx64.EFI
$ umount image.iso
```
