#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = %cd%

CC = gcc.exe
CXX = g++.exe
AR = ar.exe
LD = g++.exe
WINDRES = windres.exe

INC = 
CFLAGS = -Wall
RESINC = 
LIBDIR = 
LIB = -lgdi32 -luuid -lshlwapi
LDFLAGS = 

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g -D_DEBUG
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj\\Debug
DEP_DEBUG = 
OUT_DEBUG = bin\\Debug\\BatteryLine.exe

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj\\Release
DEP_RELEASE = 
OUT_RELEASE = bin\\Release\\BatteryLine.exe

OBJ_DEBUG = $(OBJDIR_DEBUG)\\BasicIO.o $(OBJDIR_DEBUG)\\BatStat.o $(OBJDIR_DEBUG)\\DrawLine.o $(OBJDIR_DEBUG)\\ErrorHandle.o $(OBJDIR_DEBUG)\\Main.o $(OBJDIR_DEBUG)\\rc\\BatteryLine.o

OBJ_RELEASE = $(OBJDIR_RELEASE)\\BasicIO.o $(OBJDIR_RELEASE)\\BatStat.o $(OBJDIR_RELEASE)\\DrawLine.o $(OBJDIR_RELEASE)\\ErrorHandle.o $(OBJDIR_RELEASE)\\Main.o $(OBJDIR_RELEASE)\\rc\\BatteryLine.o

all: release

clean: clean_debug clean_release

before_debug: 
	cmd /c if not exist bin\\Debug md bin\\Debug
	cmd /c if not exist $(OBJDIR_DEBUG) md $(OBJDIR_DEBUG)
	cmd /c if not exist $(OBJDIR_DEBUG)\\rc md $(OBJDIR_DEBUG)\\rc

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)\\BasicIO.o: BasicIO.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c BasicIO.c -o $(OBJDIR_DEBUG)\\BasicIO.o

$(OBJDIR_DEBUG)\\BatStat.o: BatStat.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c BatStat.c -o $(OBJDIR_DEBUG)\\BatStat.o

$(OBJDIR_DEBUG)\\DrawLine.o: DrawLine.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c DrawLine.c -o $(OBJDIR_DEBUG)\\DrawLine.o

$(OBJDIR_DEBUG)\\ErrorHandle.o: ErrorHandle.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ErrorHandle.c -o $(OBJDIR_DEBUG)\\ErrorHandle.o

$(OBJDIR_DEBUG)\\Main.o: Main.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c Main.c -o $(OBJDIR_DEBUG)\\Main.o

$(OBJDIR_DEBUG)\\rc\\BatteryLine.o: rc\\BatteryLine.rc
	$(WINDRES) -i rc\\BatteryLine.rc -J rc -o $(OBJDIR_DEBUG)\\rc\\BatteryLine.o -O coff $(INC_DEBUG)

clean_debug: 
	cmd /c del /f $(OBJ_DEBUG) $(OUT_DEBUG)
	cmd /c rd bin\\Debug
	cmd /c rd $(OBJDIR_DEBUG)
	cmd /c rd $(OBJDIR_DEBUG)\\rc

before_release: 
	cmd /c if not exist bin\\Release md bin\\Release
	cmd /c if not exist $(OBJDIR_RELEASE) md $(OBJDIR_RELEASE)
	cmd /c if not exist $(OBJDIR_RELEASE)\\rc md $(OBJDIR_RELEASE)\\rc

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) -mwindows $(LIB_RELEASE)

$(OBJDIR_RELEASE)\\BasicIO.o: BasicIO.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c BasicIO.c -o $(OBJDIR_RELEASE)\\BasicIO.o

$(OBJDIR_RELEASE)\\BatStat.o: BatStat.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c BatStat.c -o $(OBJDIR_RELEASE)\\BatStat.o

$(OBJDIR_RELEASE)\\DrawLine.o: DrawLine.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c DrawLine.c -o $(OBJDIR_RELEASE)\\DrawLine.o

$(OBJDIR_RELEASE)\\ErrorHandle.o: ErrorHandle.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ErrorHandle.c -o $(OBJDIR_RELEASE)\\ErrorHandle.o

$(OBJDIR_RELEASE)\\Main.o: Main.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c Main.c -o $(OBJDIR_RELEASE)\\Main.o

$(OBJDIR_RELEASE)\\rc\\BatteryLine.o: rc\\BatteryLine.rc
	$(WINDRES) -i rc\\BatteryLine.rc -J rc -o $(OBJDIR_RELEASE)\\rc\\BatteryLine.o -O coff $(INC_RELEASE)

clean_release: 
	cmd /c del /f $(OBJ_RELEASE) $(OUT_RELEASE)
	cmd /c rd bin\\Release
	cmd /c rd $(OBJDIR_RELEASE)
	cmd /c rd $(OBJDIR_RELEASE)\\rc

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

