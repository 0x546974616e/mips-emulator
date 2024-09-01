
# mips-emulator

An emulator written in C for educational purposes.

First engineering school project (still amateur in 2021).

## To build

```sh
make
make all
```

## To run

```sh
./sources/mips ./tests/test5.txt
./sources/mips ./tests/test6.txt
./sources/mips ./tests/test6.txt -step
./sources/mips # Interactive mode, try:
               # addi $a0, $0, 0x1A
               # exit
```

## To clean

```sh
make clean    # .o
make cleanall # .o and .d
make mrproper # .o .d and binary
```
