build:
	@mkdir -p bin; g++ opengl-labs-cpp/**.cpp -o bin/solar-system -lGL -lGLEW -lglfw
.PHONY: build

run:
	@cd bin; ./solar-system
.PHONY: run

clean:
	@rm -rf ./bin
.PHONY: clean
