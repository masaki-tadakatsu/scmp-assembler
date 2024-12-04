#
#
#
.SUFFIXES:

.SUFFIXES:	.cpp .o

.cpp.o:
	g++ -c -Wall -O2 -o $*.o $*.cpp
#
#
files	= scmp2asm.o assembler.o parser.o error.o memory.o util.o
#
#
#
#
scmp2asm.exe : $(files)
	g++ -O2 -s $(files) -o $@
clean:
	-rm *.o
	-rm *.exe
#
#
