# :set noexpandtab

BIN = mips

CSRC = $(shell find . -name "*.c")
COBJ = $(CSRC:.c=.o)
CDEP = $(CSRC:.c=.d)

CC = gcc

# -Werror
CFLAGS = \
	-Wall \
	-Wextra \
	-Wconversion \
	-Wpedantic \
	-ansi

CDEPS = -MMD -MP -MT $@ -MF $(@:.o=.d)

CINCLUDE = -I./

LDFLAGS =

.PHONY : all clean cleanall mrproper run

all : $(BIN)
	@echo "-->" ./$(notdir $<)

$(BIN) : $(COBJ)
	@echo Generating Code...
	@$(CC) $^ -o ../$@ $(LDFLAGS)

%.o : %.c
	@echo $(notdir $<)
	@$(CC) -c $< -o $@ $(CINCLUDE) $(CFLAGS) $(CDEPS)

-include $(CDEP)

clean :
	@rm -f $(COBJ)

cleanall : clean
	@rm -f $(CDEP)

mrproper : cleanall
	@rm -f ../$(BIN)

run :
	@../$(BIN) "../tests/test6.txt" -step
