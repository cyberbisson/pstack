# This is for Windows' NMAKE!

w32sopth.lib: w32sopth.obj
	lib -out:w32sopth.lib -machine:i386 -def:w32sopth.def w32sopth.obj

w32sopth.obj: w32sopth.c
	cl /c w32sopth.c