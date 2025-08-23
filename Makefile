TARGET = brain2mars
CC = gcc
CXX = g++
MKDIR = mkdir -p

CIMGUI_DIR = ./cimgui
SRC_PATH = ./src
OBJ_PATH = ./obj

SRCS = $(wildcard $(SRC_PATH)/*.c)
OBJS = $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRCS))

CIMGUI_LIB = $(CIMGUI_DIR)/libcimgui.a

CFLAGS += -I$(CIMGUI_DIR) -g -O2 --std=c99 -DCIMGUI_DEFINE_ENUMS_AND_STRUCTS -DCIMGUI_USE_SDL3 -DCIMGUI_USE_SDLRENDERER3
LDFLAGS += -static-libgcc -static-libstdc++ -Bstatic -lstdc++ $(CIMGUI_LIB) -Wl,-Bdynamic -lm -lSDL3 -lmsvg -lbox2d

all: makedirs $(TARGET)

makedirs:
	$(MKDIR) $(OBJ_PATH)

$(TARGET): $(CIMGUI_LIB) $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(CIMGUI_LIB):
	$(MAKE) -C $(@D) CXX=$(CXX) CFLAGS="-O2 -DIMGUI_DISABLE_OBSOLETE_KEYIO=1"

clean:
	$(MAKE) -C $(CIMGUI_DIR) clean
	rm -f $(OBJS) $(TARGET)
