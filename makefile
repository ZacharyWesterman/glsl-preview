BINARY = glsl-preview

$(BINARY): obj/main.o obj/shader.o
	gcc $^ -lGLEW -lGL -lGLU -lglut -o $@

obj/%.o: src/%.c obj
	gcc -c $< -o $@

obj:
	mkdir obj

clean:
	rm -rf obj $(BINARY)

.PHONY: clean
