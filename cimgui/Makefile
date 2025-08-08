CXX=g++
AR=ar -rc

OBJS = cimgui.o \
imgui/imgui.o \
imgui/imgui_demo.o \
imgui/imgui_draw.o \
imgui/imgui_impl_sdl3.o \
imgui/imgui_impl_sdlrenderer3.o \
imgui/imgui_tables.o \
imgui/imgui_widgets.o

libcimgui.a: $(OBJS)
	$(AR) $@ $^

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

imgui/%.o: imgui/%.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

clean:
	rm $(OBJS) libcimgui.a
