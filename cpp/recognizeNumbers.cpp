#include "scuffed_neurons.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void stretchNumber(std::vector<std::vector<int>> &number) {
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

  std::vector<std::vector<int>> stretched(28, std::vector<int>(28, 0));
  int dimensionX = 28 - offsetLeft - offsetRight;
  int dimensionY = 28 - offsetTop - offsetBottom;

  for(int y = 0; y < 28; y++) {
    for(int x = 0; x < 28; x++) {
      stretched[y][x] = number[(y * dimensionY / 28) + offsetTop][(x * dimensionX / 28) + offsetLeft];
    }
  }

  number = stretched;
}

void showNumber(sf::RenderWindow &window, std::vector<std::vector<int>> number) {
  for (int y = 0; y < 28; y++) {
    for (int x = 0; x < 28; x++) {
      sf::RectangleShape r(sf::Vector2f(10, 10));
      r.setFillColor(sf::Color(number[y][x], number[y][x], number[y][x]));
      r.setPosition(x * 10, y * 10);
      window.draw(r);
    }
  }
}

void showAnswers(sf::RenderWindow &window, std::vector<double> answers, sf::Font font) {
  int xpos = 280 + 30;
  for (int i = 0; i < 10; i++) {
    sf::RectangleShape r(sf::Vector2f(1000 * answers[i], 28));
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

  for(int i = 0; i < answers.size(); i++) {
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

  vector<vector<vector<int>>> test_data(
      10000, vector<vector<int>>(28, vector<int>(28, 0)));
  vector<double> labels_test(10000, 0);
  ifstream fin_test("dataset/test.txt");

  // Getting the data from file
  for (int i = 0; i < 10000; i++) {
    double n;
    fin_test >> n;
    labels_test[i] = n;
    for (int j = 0; j < 28; j++) {
      for (int k = 0; k < 28; k++) {
        fin_test >> n;
        test_data[i][j][k] = n;
      }
    }
  }
  fin_test.close();

  Layer fl(28 * 28, true);
  Layer il1(512, true);
  Layer il2(256, true);
  Layer il3(128, true);
  Layer il4(64, true);
  Layer il5(32, true);
  Layer ll(10, false);
  Network network({&fl, &il1, &il2, &il3, &il4, &il5, &ll});
  network.read_neuron_connections("n.data");

  sf::RenderWindow window(sf::VideoMode(1395, 280), "View results",
                          sf::Style::Close);
  window.setFramerateLimit(1);
  sf::Font font;
  font.loadFromFile("fonts/Hack-Regular.ttf");

  int num = 0;
  while (window.isOpen()) {

    network.clear_inputs();
    stretchNumber(test_data[num]);
    int cnt = 0;
    for (int y = 0; y < 28; y++) {
      for (int x = 0; x < 28; x++) {
        network.layers[0]->neurons[cnt]->value = test_data[num][y][x];
        cnt++;
      }
    }

    network.calculate_all_values();
    vector<double> answers = network.return_answers();
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
