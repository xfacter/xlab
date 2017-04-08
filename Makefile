TARGET = xLibTest
OBJS = main.o xlib.o xmem.o xlog.o xmath.o xtime.o xctrl.o xgraphics.o xdraw.o xtexture.o xmd2.o xtext.o xscreen.o xwav.o xobj.o xprim.o xcam.o xsprite.o #xlist.o

CFLAGS = -O2 -g -Wall
CXXFLAGS = -fno-exceptions -fno-rtti

LIBDIR =
LIBS = -lpspgum_vfpu -lpspvfpu -lpspgu -lpspaudiolib -lpspaudio -lpsprtc -lm -lstdc++
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = xLibTest

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

xlib.o: xlib.c xlib.h

xmem.o: xmem.c xmem.h

xlog.o: xlog.c xlog.h

xmath.o: xmath.c xmath.h

xtime.o: xtime.c xtime.h

xctrl.o: xctrl.c xctrl.h

xgraphics.o: xgraphics.c xgraphics.h

xdraw.o: xdraw.c xdraw.h

xtexture.O: xtexture.cpp xtexture.h

xmd2.o: xmd2.c xmd2.h

xtext.o: xtext.cpp xtext.h

xscreen.o: xscreen.c xscreen.h

xwav.o: xwav.c xwav.h

xobj.o: xobj.c xobj.h

xprim.o: xprim.c xprim.h

xcam.o: xcam.cpp xcam.h

#xlist.o: xlist.c xlist.h

xsprite.o: xsprite.cpp xsprite.h

main.o: main.cpp xconfig.h xlib.o xmem.o xlog.o xmath.o xtime.o xctrl.o xgraphics.o xdraw.o xtexture.o xmd2.o xtext.o xscreen.o xwav.o xobj.o xprim.o xcam.o xsprite.o #xlist.o

EBOOT.PBP: main.o
