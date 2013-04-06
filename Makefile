CC = g++
CFLAGS = -g 
LIBS = -lm -lX11 -lXi -lXmu -lglut -lGL -lGLU -lGLEW
DEPS = shaderSetup.h bitmap.h
OBJ = shaderSetup.o shader_test.o bitmap.o
OUT = shader

%.o: %.cpp $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)


$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) 

clean:
	rm *.o $(OUT)
