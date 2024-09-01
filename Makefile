
SOURCES = $(CURDIR)/sources

.PHONY : all clean cleanall mrproper run

all:
	@make -sC $(SOURCES) all

clean:
	@make -sC $(SOURCES) clean

cleanall:
	@make -sC $(SOURCES) cleanall

mrproper:
	@make -sC $(SOURCES) mrproper

run:
	@make -sC $(SOURCES) run
