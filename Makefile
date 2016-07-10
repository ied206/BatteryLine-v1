WORKDIR = %cd%

CC = gcc.exe
CXX = g++.exe
AR = ar.exe
LD = g++.exe
WINDRES = windres.exe

INC = 
CFLAGS = -Wall -std=c99 
RESINC = 
LIBDIR = 
LIB = -lgdi32 -luuid -lshlwapi
LDFLAGS = 
RESFLAGS = 

OBJDIR = obj
OUTDIR = bin

INC_32 = $(INC)
CFLAGS_32 = $(CFLAGS) -O2 -m32
RESINC_32 = $(RESINC)
RCFLAGS_32 = $(RCFLAGS)
LIBDIR_32 = $(LIBDIR)
LIB_32 = $(LIB)
LDFLAGS_32 = $(LDFLAGS) -s -m32
RESFLAGS_32 = $(RESFLAGS) -F pe-i386
OBJDIR_32 = $(OBJDIR)\\x86
DEP_32 = 
OUT_32 = $(OUTDIR)\\BatteryLine32.exe

INC_64 = $(INC)
CFLAGS_64 = $(CFLAGS) -O2 -m64
RESINC_64 = $(RESINC)
RCFLAGS_64 = $(RCFLAGS)
LIBDIR_64 = $(LIBDIR)
LIB_64 = $(LIB)
LDFLAGS_64 = $(LDFLAGS) -s -m64
RESFLAGS_64 = $(RESFLAGS) -F pe-x86-64
OBJDIR_64 = $(OBJDIR)\\x64
DEP_64 = 
OUT_64 = $(OUTDIR)\\BatteryLine64.exe

OBJ_32 = $(OBJDIR_32)\\BasicIO.o $(OBJDIR_32)\\BatStat.o $(OBJDIR_32)\\DrawLine.o $(OBJDIR_32)\\ErrorHandle.o $(OBJDIR_32)\\Main.o $(OBJDIR_32)\\rc\\Resource.o
OBJ_64 = $(OBJDIR_64)\\BasicIO.o $(OBJDIR_64)\\BatStat.o $(OBJDIR_64)\\DrawLine.o $(OBJDIR_64)\\ErrorHandle.o $(OBJDIR_64)\\Main.o $(OBJDIR_64)\\rc\\Resource.o

all: release_32 release_64

clean:
	cmd /c rd /s /q $(OUTDIR)
	cmd /c rd /s /q $(OBJDIR)

ready_32: 
	cmd /c if not exist $(OUTDIR) mkdir $(OUTDIR)
	cmd /c if not exist $(OBJDIR_32) mkdir $(OBJDIR_32)
	cmd /c if not exist $(OBJDIR_32)\\rc mkdir $(OBJDIR_32)\\rc

release_32: ready_32 build_32
build_32: ready_32 $(OBJ_32) $(DEP_32)
	$(LD) $(LIBDIR_32) -o $(OUT_32) $(OBJ_32)  $(LDFLAGS_32) -mwindows $(LIB_32)

$(OBJDIR_32)\\BasicIO.o: BasicIO.c
	$(CC) $(CFLAGS_32) $(INC_32) -c BasicIO.c -o $(OBJDIR_32)\\BasicIO.o

$(OBJDIR_32)\\BatStat.o: BatStat.c
	$(CC) $(CFLAGS_32) $(INC_32) -c BatStat.c -o $(OBJDIR_32)\\BatStat.o

$(OBJDIR_32)\\DrawLine.o: DrawLine.c
	$(CC) $(CFLAGS_32) $(INC_32) -c DrawLine.c -o $(OBJDIR_32)\\DrawLine.o

$(OBJDIR_32)\\ErrorHandle.o: ErrorHandle.c
	$(CC) $(CFLAGS_32) $(INC_32) -c ErrorHandle.c -o $(OBJDIR_32)\\ErrorHandle.o

$(OBJDIR_32)\\Main.o: Main.c
	$(CC) $(CFLAGS_32) $(INC_32) -c Main.c -o $(OBJDIR_32)\\Main.o

$(OBJDIR_32)\\rc\\Resource.o: rc\\Resource.rc
	$(WINDRES) $(RESFLAGS_32) -i rc\\Resource.rc -J rc -o $(OBJDIR_32)\\rc\\Resource.o -O coff $(INC_32)


ready_64: 
	cmd /c if not exist $(OUTDIR) mkdir $(OUTDIR)
	cmd /c if not exist $(OBJDIR_64) mkdir $(OBJDIR_64)
	cmd /c if not exist $(OBJDIR_64)\\rc mkdir $(OBJDIR_64)\\rc

release_64: ready_64 build_64
build_64: ready_64 $(OBJ_64) $(DEP_64)
	$(LD) $(LIBDIR_64) -o $(OUT_64) $(OBJ_64)  $(LDFLAGS_64) -mwindows $(LIB_64)

$(OBJDIR_64)\\BasicIO.o: BasicIO.c
	$(CC) $(CFLAGS_64) $(INC_64) -c BasicIO.c -o $(OBJDIR_64)\\BasicIO.o

$(OBJDIR_64)\\BatStat.o: BatStat.c
	$(CC) $(CFLAGS_64) $(INC_64) -c BatStat.c -o $(OBJDIR_64)\\BatStat.o

$(OBJDIR_64)\\DrawLine.o: DrawLine.c
	$(CC) $(CFLAGS_64) $(INC_64) -c DrawLine.c -o $(OBJDIR_64)\\DrawLine.o

$(OBJDIR_64)\\ErrorHandle.o: ErrorHandle.c
	$(CC) $(CFLAGS_64) $(INC_64) -c ErrorHandle.c -o $(OBJDIR_64)\\ErrorHandle.o

$(OBJDIR_64)\\Main.o: Main.c
	$(CC) $(CFLAGS_64) $(INC_64) -c Main.c -o $(OBJDIR_64)\\Main.o

$(OBJDIR_64)\\rc\\Resource.o: rc\\Resource.rc
	$(WINDRES) $(RESFLAGS_64) -i rc\\Resource.rc -J rc -o $(OBJDIR_64)\\rc\\Resource.o -O coff $(INC_64)
	

	
.PHONY: ready_32 ready_64 build_32 build_64 clean

