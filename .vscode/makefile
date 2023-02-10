#source file search paths
VPATH = ./src ../autocell/src

	# compiler include search paths
INCS = -I./include \
	-I../windex/include \
	-I../raven-set \
	-I../autocell/src


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
	$(ODIR)/autocell.o

### COMPILE

$(ODIR)/%.o: %.cpp  
	g++  $(CCflags) \
	-c -o $@ $< $(INCS)

starter: $(OBJ_starter)
	g++ $(OPT) \
	-o bin/starter.exe $^  \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

