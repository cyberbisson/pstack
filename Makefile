
# Makefile
# This will generate a Win32 executable.  Drive everything from this one file?
# Matt Bisson     9 July, 2008

all:
	vcbuild /nologo /logcommands pstack.sln "release|Win32"

docs:
	doxygen

clean_docs:
	cmd /c rmdir /s /q doc\html

distclean: clean clean_docs

clean:
	vcbuild /nologo /logcommands /c pstack.sln
	rm -rf release debug
	rm -rf addr2ln/obj
	rm -rf proclib/obj
	rm -rf pstack/obj
	rm -rf psystem/obj
