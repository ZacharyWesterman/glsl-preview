BINARY = glsl-preview

$(BINARY): obj/main.o obj/window.o
	gcc $^ -lXrender -lX11 -lGLEW -lGL -lGLU -o $@

obj/%.o: src/%.c obj
	gcc -c $< -o $@

obj:
	mkdir obj

clean:
	rm -rf obj $(BINARY)

.PHONY: clean
