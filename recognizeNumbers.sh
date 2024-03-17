g++ -c cpp/recognizeNumbers.cpp -o executables/recognizeNumbers.o
g++ executables/recognizeNumbers.o -o executables/sfml-app -lsfml-graphics -lsfml-window -lsfml-system
./executables/sfml-app
