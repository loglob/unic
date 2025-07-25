.PHONY: all

all: kjv.txt gg.txt
	md5sum --check manifest.md5

# these files are too big to check into git
kjv.txt:
	wget https://openbible.com/textfiles/kjv.txt -O $@

gg.txt:
	wget https://gg.docpatch.org/grundgesetz-dev/out/brd_grundgesetz_63_2019-04-03.txt -O $@