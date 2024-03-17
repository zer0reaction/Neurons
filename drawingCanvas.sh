g++ -c cpp/drawingCanvas.cpp -o executables/drawingCanvas.o
g++ executables/drawingCanvas.o -o executables/canvas -lsfml-graphics -lsfml-window -lsfml-system
./executables/canvas
