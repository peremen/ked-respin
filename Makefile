# Korean Editor Makefile
#
# written by Song Jaekyung

BIN= /usr/local/bin
INC= /usr/local/lib/g++-include
#LIB= -lg++ /usr/local/lib/gcc-gnulib libc.a

# Change the following line to point to the directory
# where the application default resource files are kept.

APPDEFAULTSDIR = /usr/local/lib/X11/app-defaults

DEFINES = -DAPPDEFAULTSDIR=\"$(APPDEFAULTSDIR)\"

C_FLAG= -Wall -g $(DEFINES)

SRC= main.c basic.c Kfont.c container.c base.c menu.c \
     editor.c dialog.c scroll.c scrollbar.c button.c input_dialog.c \
     load.c save.c cut.c print.c
OBJ= main.o basic.o Kfont.o container.o base.o menu.o \
     editor.o dialog.o scroll.o scrollbar.o button.o input_dialog.o \
     load.o save.o cut.o print.o

.c.o:
	g++ $(C_FLAG) -c $<

ked: $(OBJ)
	g++ $(C_FLAG) -o $@ $(OBJ) -lX11 $(LIB)

install: ked
	cp ked $(BIN)/ked

depend:
	makedepend -I$(INC) $(SRC)

clean:
	-rm -f *.o ked *~ *bak core *BAK *CKP .*BAK .*CKP

# DO NOT DELETE THIS LINE -- make depend depends on it.

main.o: /usr/local/lib/g++-include/std.h /usr/local/lib/g++-include/stddef.h
main.o: /usr/local/lib/g++-include/stdio.h button.h basic.h Xlib.h
main.o: /usr/local/lib/g++-include/sys/types.h //usr/include/sys/types.h
main.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h Kfont.h menu.h
main.o: editor.h aline.h ked.h dialog.h container.h scroll.h scrollbar.h
main.o: base.h input_dialog.h
basic.o: /usr/local/lib/g++-include/std.h /usr/local/lib/g++-include/stddef.h
basic.o: /usr/local/lib/g++-include/stdio.h basic.h Xlib.h
basic.o: /usr/local/lib/g++-include/sys/types.h //usr/include/sys/types.h
basic.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h Kfont.h macro.h
basic.o: container.h
Kfont.o: /usr/local/lib/g++-include/std.h /usr/local/lib/g++-include/stddef.h
Kfont.o: /usr/local/lib/g++-include/stdio.h Kfont.h Xlib.h
Kfont.o: /usr/local/lib/g++-include/sys/types.h //usr/include/sys/types.h
Kfont.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h macro.h basic.h
Kfont.o: editor.h aline.h
container.o: container.h basic.h Xlib.h
container.o: /usr/local/lib/g++-include/sys/types.h //usr/include/sys/types.h
container.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h Kfont.h
base.o: /usr/local/lib/g++-include/std.h /usr/local/lib/g++-include/stddef.h
base.o: /usr/local/lib/g++-include/stdio.h base.h container.h basic.h Xlib.h
base.o: /usr/local/lib/g++-include/sys/types.h //usr/include/sys/types.h
base.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h Kfont.h Xutil.h
menu.o: menu.h basic.h Xlib.h /usr/local/lib/g++-include/sys/types.h
menu.o: //usr/include/sys/types.h /usr/include/sys/sysmacros.h
menu.o: /usr/include/X11/X.h Kfont.h
editor.o: /usr/local/lib/g++-include/std.h
editor.o: /usr/local/lib/g++-include/stddef.h
editor.o: /usr/local/lib/g++-include/stdio.h /usr/include/X11/keysym.h
editor.o: /usr/include/X11/keysymdef.h editor.h basic.h Xlib.h
editor.o: /usr/local/lib/g++-include/sys/types.h //usr/include/sys/types.h
editor.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h Kfont.h aline.h
editor.o: Xutil.h
dialog.o: /usr/local/lib/g++-include/std.h
dialog.o: /usr/local/lib/g++-include/stddef.h
dialog.o: /usr/local/lib/g++-include/stdio.h
dialog.o: /usr/local/lib/g++-include/sys/types.h //usr/include/sys/types.h
dialog.o: /usr/include/sys/sysmacros.h /usr/local/lib/g++-include/sys/stat.h
dialog.o: //usr/include/sys/stat.h dialog.h container.h basic.h Xlib.h
dialog.o: /usr/include/X11/X.h Kfont.h button.h editor.h aline.h scroll.h
dialog.o: scrollbar.h margin.h Xutil.h
scroll.o: /usr/local/lib/g++-include/std.h
scroll.o: /usr/local/lib/g++-include/stddef.h
scroll.o: /usr/local/lib/g++-include/stdio.h scroll.h basic.h Xlib.h
scroll.o: /usr/local/lib/g++-include/sys/types.h //usr/include/sys/types.h
scroll.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h Kfont.h aline.h
scroll.o: margin.h
scrollbar.o: /usr/local/lib/g++-include/std.h
scrollbar.o: /usr/local/lib/g++-include/stddef.h
scrollbar.o: /usr/local/lib/g++-include/stdio.h
scrollbar.o: /usr/local/lib/g++-include/signal.h /usr/include/sys/signal.h
scrollbar.o: /usr/include/vm/faultcode.h /usr/local/lib/g++-include/time.h
scrollbar.o: /usr/local/lib/g++-include/sys/types.h //usr/include/sys/types.h
scrollbar.o: /usr/include/sys/sysmacros.h //usr/include/sys/time.h
scrollbar.o: //usr/include/sys/times.h //usr/include/time.h scrollbar.h
scrollbar.o: basic.h Xlib.h /usr/include/X11/X.h Kfont.h scroll.h aline.h
scrollbar.o: margin.h up down up_fill down_fill stipple macro.h
button.o: button.h basic.h Xlib.h /usr/local/lib/g++-include/sys/types.h
button.o: //usr/include/sys/types.h /usr/include/sys/sysmacros.h
button.o: /usr/include/X11/X.h Kfont.h
input_dialog.o: input_dialog.h container.h basic.h Xlib.h
input_dialog.o: /usr/local/lib/g++-include/sys/types.h
input_dialog.o: //usr/include/sys/types.h /usr/include/sys/sysmacros.h
input_dialog.o: /usr/include/X11/X.h Kfont.h button.h editor.h aline.h
input_dialog.o: scroll.h scrollbar.h margin.h Xutil.h
load.o: /usr/local/lib/g++-include/std.h /usr/local/lib/g++-include/stddef.h
load.o: /usr/local/lib/g++-include/stdio.h dialog.h container.h basic.h
load.o: Xlib.h /usr/local/lib/g++-include/sys/types.h
load.o: //usr/include/sys/types.h /usr/include/sys/sysmacros.h
load.o: /usr/include/X11/X.h Kfont.h button.h editor.h aline.h scroll.h
load.o: scrollbar.h ked.h
save.o: /usr/local/lib/g++-include/std.h /usr/local/lib/g++-include/stddef.h
save.o: /usr/local/lib/g++-include/stdio.h ked.h dialog.h container.h basic.h
save.o: Xlib.h /usr/local/lib/g++-include/sys/types.h
save.o: //usr/include/sys/types.h /usr/include/sys/sysmacros.h
save.o: /usr/include/X11/X.h Kfont.h button.h editor.h aline.h scroll.h
save.o: scrollbar.h input_dialog.h
cut.o: ked.h dialog.h container.h basic.h Xlib.h
cut.o: /usr/local/lib/g++-include/sys/types.h //usr/include/sys/types.h
cut.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h Kfont.h button.h
cut.o: editor.h aline.h scroll.h scrollbar.h
print.o: /usr/local/lib/g++-include/std.h /usr/local/lib/g++-include/stddef.h
print.o: /usr/local/lib/g++-include/stdio.h editor.h basic.h Xlib.h
print.o: /usr/local/lib/g++-include/sys/types.h //usr/include/sys/types.h
print.o: /usr/include/sys/sysmacros.h /usr/include/X11/X.h Kfont.h aline.h
