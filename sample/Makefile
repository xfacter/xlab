### PROJECT SETTINGS ###
BIN_NAME = xlibtest
PBP_PATH = Bundle
SRC_PATH = ..
BIN_PATH = .
BUILD_PATH = .

# Basic configuration for PSPSDK. Refer to $(PSPSDK)/lib/build.mak
PSP_EBOOT_TITLE = xlibtest
#PSP_EBOOT_ICON = $(PBP_PATH)/icon0.png
#PSP_EBOOT_ICON1 = $(PBP_PATH)/icon1.pmf
#PSP_EBOOT_UNKPNG = $(PBP_PATH)/pic0.png
#PSP_EBOOT_PIC1 = $(PBP_PATH)/pic1.png
#PSP_EBOOT_SND0 = $(PBP_PATH)/snd0.at3
#PSP_EBOOT_PSAR = $(PBP_PATH)/data.psar
PSP_EBOOT_SFO = $(BUILD_PATH)/PARAM.SFO
PSP_EBOOT = $(BIN_PATH)/EBOOT.PBP
EXTRA_TARGETS = $(PSP_EBOOT)
TARGET = $(BUILD_PATH)/$(BIN_NAME)
#BUILD_PRX = 1

OBJS = main.o xlib.o xmem.o xlog.o xmath.o xtime.o xctrl.o xgraphics.o \
	xtext.o xtexture.o xmd2.o xsound.o xobj.o xprim.o xcam.o \
	xdraw.o #xsprite.o xscreen.o xlist.o

CFLAGS = -O2 -g -Wall
CXXFLAGS = -fno-exceptions -fno-rtti

INCDIR = $(SRC_PATH)
LIBDIR =
LIBS = -lpspgum_vfpu -lpspvfpu -lpspgu -lpspaudiolib -lpspaudio -lpsprtc -lm -lstdc++
LDFLAGS =
### END PROJECT SETTINGS ###

# Generally should not need to edit below this line

DEPS = $(OBJS:.o=.d)
-include $(DEPS)

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

clean: clean_more
clean_more:
	$(RM) $(DEPS) $(BIN_PATH)/xlog.txt

%.o: $(SRC_PATH)/%.c
	$(CC) -MP -MMD -c $(CFLAGS) -o $@ $<

%.o: $(SRC_PATH)/%.cpp
	$(CXX) -MP -MMD -c $(CXXFLAGS) -o $@ $<
