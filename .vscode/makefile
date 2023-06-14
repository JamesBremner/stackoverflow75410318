#source file search paths
VPATH = ./src ../autocell/src ../raven-set

	# compiler include search paths
INCS = -I./include \
	-I../windex/include \
	-I../raven-set \
	-I../autocell/src \
	-I../../code_ext/boost/boost1_78 


LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs -lComctl32 \

	#-mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static

# folders
ODIR = ./.vscode/obj


OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_starter = \
	$(ODIR)/main.o \
	$(ODIR)/cSequenceHunter.o \
	$(ODIR)/autocell.o \
	$(ODIR)/cRunWatch.o \

OBJ_test = \
	$(ODIR)/test.o \
	$(ODIR)/cSequenceHunter.o \
	$(ODIR)/autocell.o \
	$(ODIR)/cutest.o \
	$(ODIR)/cRunWatch.o \


### COMPILE

$(ODIR)/%.o: %.cpp  cSequenceHunter.h
	g++  $(CCflags) \
	-c -o $@ $< $(INCS)

starter: $(OBJ_starter)
	g++ $(OPT) \
	-o bin/seqHunter.exe $^  \
	$(static_link_flags) $(LIBS)

test: $(OBJ_test)
	g++ -o bin/test.exe \
	$^ $(OPT) \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o


rebuild: clean test starter



