BINARY = glsl-preview

$(BINARY): obj/main.o obj/shader.o
	gcc $^ -lGLEW -lGL -lGLU -lglut -o $@

obj/%.o: src/%.c
	gcc -c $< -o $@

clean:
	rm -f obj/*.o $(BINARY)

.PHONY: clean
