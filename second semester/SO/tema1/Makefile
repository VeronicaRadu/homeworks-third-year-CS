CC = cl
CFLAGS = /W3 /nologo /D_CRT_SECURE_NO_DEPRECATE
EXE = tema1.exe

build: $(EXE)

tema1.exe: main.obj hash.obj hashtable.obj list.obj nodes.obj utils.obj
	$(CC) /Fe$@ $**

main.obj: main.c
	$(CC) $(CFLAGS) /Fo$@ /c $**

hash.obj: hash.c
	$(CC) $(CFLAGS) /Fo$@ /c $**

hashtable.obj: hashtable.c
	$(CC) $(CFLAGS) /Fo$@ /c $**

list.obj: list.c
	$(CC) $(CFLAGS) /Fo$@ /c $**

nodes.obj: nodes.c
	$(CC) $(CFLAGS) /Fo$@ /c $**
	
utils.obj: utils.c
	$(CC) $(CFLAGS) /Fo$@ /c $**

.PHONY: clean
clean:
	del *.obj
	del $(EXE)
