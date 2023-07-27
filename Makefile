BREWPATH = $(shell brew --prefix)

CC = g++
CFLAGS = -g -std=c++11 -Wno-deprecated-register -Wno-deprecated-declarations -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
INCFLAGS = -I./include -I$(BREWPATH)/include
LDFLAGS = -framework GLUT -framework OpenGL -L$(BREWPATH)/lib -lfreeimage

RM = /bin/rm -f
all: MeshObj
MeshObj: main.o Shader.o Camera.o Mesh.o Obj.o shaders/normal.frag shaders/projective.vert
	$(CC) -o MeshObj main.o Shader.o Camera.o Mesh.o Obj.o $(LDFLAGS)
main.o: main.cpp 
	$(CC) $(CFLAGS) $(INCFLAGS) -c main.cpp 
Shader.o: src/Shader.cpp
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/Shader.cpp
Camera.o: src/Camera.cpp include/Camera.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/Camera.cpp
Mesh.o: src/Mesh.cpp include/Mesh.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/Mesh.cpp
Obj.o: src/Obj.cpp include/Obj.h 
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/Obj.cpp
clean: 
	$(RM) *.o *.png MeshObj

 
