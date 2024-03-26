#include <SFML/Graphics.hpp>
#include "neurons.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <vector>

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

void showNumber(sf::RenderWindow &window,
                int** &number) {
  for (int y = 0; y < 28; y++) {
    for (int x = 0; x < 28; x++) {
      sf::RectangleShape r(sf::Vector2f(10, 10));
      r.setFillColor(sf::Color(number[y][x], number[y][x], number[y][x]));
      if(number[y][x] >= 255) {
        r.setFillColor(sf::Color(255, 255, 255));
      }
      r.setOutlineThickness(-0.5);
      r.setOutlineColor(sf::Color::White);
      r.setPosition(x * 10, y * 10);
      window.draw(r);
    }
  }
}

void showAnswers(sf::RenderWindow &window, double* &answers,
                 sf::Font font) {
  int xpos = 280 + 50;
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
    text.setPosition(300, i * 28);
    text.setCharacterSize(27);
    window.draw(text);
  }
}

void moveNumber(int** &number) {
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

int main() {
  using namespace std;

  Network network({28*28+1, 512+1, 256+1, 128+1, 64+1, 32+1, 10}, {true, true, true, true, true, true, false});
  network.loadNeuronConnections("n.data");
  

  sf::RenderWindow window(sf::VideoMode(495, 280), "View results",
                          sf::Style::Close);
  window.setFramerateLimit(120);
  sf::Font font;
  font.loadFromFile("fonts/Hack-Regular.ttf");
  sf::Mouse::setPosition(sf::Vector2i(0,0), window);

  int** number;
  initNumberArray(number);

  while (window.isOpen()) {
    network.clearInputs();

    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      sf::Vector2i localPos = sf::Mouse::getPosition(window);
      int xPos = localPos.x / 10;
      int yPos = localPos.y / 10;

      vector<int> offsetX = {0, -1, 1, 0};
      vector<int> offsetY = {1, 0, 0, -1};

      for(int i = 0; i < 4; i++) {
        if(yPos-offsetY[i] >= 0 && yPos-offsetY[i] < 28 && xPos-offsetX[i] >=0 && xPos-offsetX[i] < 28)
          if(number[yPos-offsetY[i]][xPos-offsetX[i]] <= 235)
            number[yPos-offsetY[i]][xPos-offsetX[i]] += 20;
      }

      if(xPos >= 0 && xPos < 28 && yPos >= 0 && yPos < 28)
        if(number[yPos][xPos] <= 225)
          number[yPos][xPos] += 30;
    }

    if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
      sf::Vector2i localPos = sf::Mouse::getPosition(window);
      int xPos = localPos.x / 10;
      int yPos = localPos.y / 10;

      vector<int> offsetX = {-1, 0, 1, -1, 1, -1, 0, 1};
      vector<int> offsetY = {1, 1, 1, 0, 0, -1, -1, -1};

      for(int i = 0; i < 8; i++) {
        if(yPos-offsetY[i] >= 0 && yPos-offsetY[i] < 28 && xPos-offsetX[i] >=0 && xPos-offsetX[i] < 28)
          number[yPos-offsetY[i]][xPos-offsetX[i]] = 0;
      }

      if(xPos >= 0 && xPos < 28 && yPos >= 0 && yPos < 28)
        number[yPos][xPos] = 0;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C)) {
      for(int y = 0; y < 28; y++) {
        for(int x = 0; x < 28; x++) {
          number[y][x] = 0;
        }
      }
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M)) {
      moveNumber(number);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
      stretchNumber(number);
    }

    int cnt = 0;
    for(int y = 0; y < 28; y++) {
      for(int x = 0; x < 28; x++) {
        network.setInput(cnt, double(number[y][x]));
        cnt++;
      }
    }

    network.feedForward();
    double* answers = network.returnAnswers();

    window.clear();
    showNumber(window, number);
    showAnswers(window, answers, font);
    window.display();

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
  }

}
