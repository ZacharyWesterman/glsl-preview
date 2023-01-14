BINARY = glsl-preview
INSTALL_DIR = ~/.local/bin/
DESKTOP_DIR = ~/.local/share/applications/

$(BINARY): obj/main.o obj/shader.o
	gcc $^ -lGLEW -lGL -lGLU -lglut -o $@

obj/%.o: src/%.c
	gcc -c $< -o $@

clean:
	rm -f obj/*.o $(BINARY)

install: glsl-preview
	cp $< $(INSTALL_DIR)
	< $<.desktop sed "s|___|${HOME}|" >$(DESKTOP_DIR)$<.desktop

.PHONY: clean install
