#include "neurons.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void getData(int*** &data, int* &labels, std::string path, int numberOfnumbers) { 
  using namespace std;

  ifstream fin(path);

  int n;
  for(int i = 0; i < numberOfnumbers; i++) {
    fin >> n;
    labels[i] = n;

    for(int y = 0; y < 28; y++) {
      for(int x = 0; x < 28; x++) {
        fin >> n;
        data[i][y][x] = n;
      } 
    }
  }
  fin.close();
}

void initNumberArray(int** &number) {
  number = new int* [28];

  for(int i = 0; i < 28; i++) {
    number[i] = new int[28];
    for(int j = 0; j < 28; j++)
      number[i][j] = 0;
  }
}

void initDataArray(int*** &data, int numberOfnumbers) {
  data = new int** [numberOfnumbers];

  for(int i = 0; i < numberOfnumbers; i++) {
    initNumberArray(data[i]);
  }
}

void stretchNumber(int** &number) {
  int offsetTop = 0;
  int offsetLeft = 0;
  int offsetRight = 0;
  int offsetBottom = 0;

  // Top
  for(int y = 0; y < 28; y++) {
    bool f = false;
    for(int x = 0; x < 28; x++)
      if(number[y][x] > 0) f = true;

    if(!f) offsetTop++;
    else break;
  }
  // Bottom
  for(int y = 27; y >= 0; y--) {
    bool f = false;
    for(int x = 0; x < 28; x++)
      if(number[y][x] > 0) f = true;

    if(!f) offsetBottom++;
    else break;
  }
  // Left
  for(int x = 0; x < 28; x++) {
    bool f = false;
    for(int y = 0; y < 28; y++) 
      if(number[y][x] > 0)
        f = true;

    if(!f) offsetLeft++;
    else break;
  }
  // Right
  for(int x = 27; x >= 0; x--) {
    bool f = false;
    for(int y = 0; y < 28; y++) 
      if(number[y][x] > 0)
        f = true;

    if(!f) offsetRight++;
    else break;
  }

  int** stretched;
  initNumberArray(stretched);

  int dimensionX = 28 - offsetLeft - offsetRight;
  int dimensionY = 28 - offsetTop - offsetBottom;

  for(int y = 0; y < 28; y++) {
    for(int x = 0; x < 28; x++) {
      stretched[y][x] = number[(y * dimensionY / 28) + offsetTop][(x * dimensionX / 28) + offsetLeft];
    }
  }

  delete[] number;
  number = stretched;
}

void showNumber(sf::RenderWindow &window, int** &number) {
  for (int y = 0; y < 28; y++) {
    for (int x = 0; x < 28; x++) {
      sf::RectangleShape r(sf::Vector2f(10, 10));
      r.setFillColor(sf::Color(number[y][x], number[y][x], number[y][x]));
      r.setPosition(x * 10, y * 10);
      window.draw(r);
    }
  }
}

void showAnswers(sf::RenderWindow &window, double* &answers, sf::Font font) {
  int xpos = 280 + 30;
  for (int i = 0; i < 10; i++) {
    sf::RectangleShape r(sf::Vector2f(100 * answers[i], 28));
    r.setFillColor(sf::Color::White);
    r.setOutlineColor(sf::Color::Black);
    r.setOutlineThickness(-1);
    r.setPosition(xpos, i * 28);
    window.draw(r);

    sf::Text text;
    text.setString(std::to_string(i));
    text.setFont(font);
    text.setPosition(270, i * 28);
    text.setCharacterSize(27);
    window.draw(text);
  }

  double maxAnswer = 0.0;
  int maxNum = 0;

  for(int i = 0; i < 10; i++) {
    if(answers[i] > maxAnswer) {
      maxAnswer = answers[i];
      maxNum = i;
    }
  }

  sf::Text text;
  text.setString(std::to_string(maxNum));
  text.setFont(font);
  text.setPosition(5, 248);
  text.setCharacterSize(27);
  window.draw(text);
}

void moveNumber(std::vector<std::vector<int>> &number) {
  int offsetY = 0;
  int offsetX = 0;

  for(int y = 0; y < 28; y++) {
    bool f = false;
    for(int x = 0; x < 28; x++) {
      if(number[y][x] > 0) 
        f = true;
    }

    if(f)  { 
      break;
    }
    offsetY++; 
  }

  for(int x = 0; x < 28; x++) {
    bool f = false;
    for(int y = 0; y < 28; y++) {
      if(number[y][x] > 0) 
        f = true;
    }
    
    if(f)  { 
      break;
    }
    offsetX++; 
  }

  for(int i = 0; i < offsetY; i++) {
    for(int y = 0; y < 27; y++) {
      number[y] = number[y+1];
    }
    for(int x = 0; x < 28; x++) {
      number[27][x] = 0;
    }
  }

  for(int i = 0; i < offsetX; i++) {
    for(int y = 0; y < 28; y++) {
      for(int x = 0; x < 27; x++) {
        number[y][x] = number[y][x+1];
      }
      for(int y = 0; y < 28; y++) {
        number[y][27] = 0;
      }
    }
  }
}

int main() {
  using namespace std;

  int*** test_data;
  int* labels_test = new int[10000];

  initDataArray(test_data, 10000);
  getData(test_data, labels_test, "dataset/test.txt", 10000);

  Layer* fl = new Layer (28 * 28 + 1, true);
  Layer* il1 = new Layer (512 + 1, true);
  Layer* il2 = new Layer (256 + 1, true);
  Layer* il3 = new Layer (128 + 1, true);
  Layer* il4 = new Layer (64 + 1, true);
  Layer* il5 = new Layer (32 + 1, true);
  Layer* ll = new Layer (10, false);

  Layer** layers = new Layer*[7];
  layers[0] = fl;
  layers[1] = il1;
  layers[2] = il2;
  layers[3] = il3;
  layers[4] = il4;
  layers[5] = il5;
  layers[6] = ll;

  Network network(layers, 7);
  network.loadNeuronConnections("/home/zer0/Neurons/n.data");

  sf::RenderWindow window(sf::VideoMode(495, 280), "View results",
                          sf::Style::Close);
  window.setFramerateLimit(1);
  sf::Font font;
  font.loadFromFile("fonts/Hack-Regular.ttf");

  int num = 0;
  while (window.isOpen()) {

    network.clearInputs();
    stretchNumber(test_data[num]);
    int cnt = 0;
    for (int y = 0; y < 28; y++) {
      for (int x = 0; x < 28; x++) {
        network.setInput(cnt, test_data[num][y][x]);
        cnt++;
      }
    }

    network.feedForward();
    double* answers = network.returnAnswers();

    window.clear();
    showNumber(window, test_data[num]);
    showAnswers(window, answers, font);
    window.display();

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
    num++;
  }
}
