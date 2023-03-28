#source file search paths
VPATH = ./src ./src/ext ../raven-set

	# compiler include search paths
INCS = -I./src/ext \
	-I../windex/include \
	-I../raven-set \
	-I../boost/boost1_78 

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs -lComctl32
	# -L../boost/boost1_78/lib \
	# 	-lboost_system-mgw11-mt-x64-1_78 \
	# 	-lboost_program_options-mgw11-mt-x64-1_78 \
	# 	# -lboost_thread-mgw11-mt-x64-1_7 \
	#-mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static

# folders
ODIR = ./.vscode/obj


OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_starter = \
	$(ODIR)/main.o \
	$(ODIR)/cCompound.o \
	$(ODIR)/chemgraph.o

# sources
OBJ_test = \
	$(ODIR)/test.o \
	$(ODIR)/cutest.o \
	$(ODIR)/chemgraph.o 

### COMPILE

$(ODIR)/%.o: %.cpp  cCompound.h
	g++  $(CCflags) \
	-c -o $@ $< $(INCS)

starter: $(OBJ_starter)
	g++ $(OPT) \
	-o bin/starter.exe $^  \
	$(static_link_flags) $(LIBS)

test: $(OBJ_test)
	g++ -o bin/test.exe \
	$^ $(OPT) \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

