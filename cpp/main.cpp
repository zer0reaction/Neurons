#include "neurons.hpp"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

void getTrainingData(std::vector<std::vector<std::vector<int>>> &data, std::vector<int> &labels) {
  using namespace std;
  ifstream fin("dataset/train.txt");

  for(int i = 0; i < 60000; i++) {
    int n;
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

void getTestData(std::vector<std::vector<std::vector<int>>> &data, std::vector<int> &labels) {
  using namespace std;
  ifstream fin("dataset/train.txt");

  for(int i = 0; i < 10000; i++) {
    int n;
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

double calculateError(Network &n, std::vector<std::vector<std::vector<int>>> &training_data, std::vector<int> &labels_train, double batch_size) {
  double error = 0.0;

  for(int i = 0; i < int(batch_size); i++) {
    n.clearInputs();
    stretchNumber(training_data[i]);

    int cnt = 0;
    for(int y = 0; y < 28; y++) {
      for(int x = 0; x < 28; x++) {
        n.setInput(cnt, training_data[i][y][x]);
        cnt++;
      }
    }

    n.feedForward();
    double* answers = n.returnAnswers();

    for(int j = 0; j < 10; j++) {
      if(j == labels_train[i]) {
        error += pow(1.0 - answers[j], 2.0);
      } else {
        error += pow(0.0 - answers[j], 2.0);
      }
    }
    //delete[] answers;
  }

  error /= batch_size;
  return error;
}

double calculateGuess(Network &n, std::vector<std::vector<std::vector<int>>> &training_data, std::vector<int> &labels_train, double batch_size) {
  double right = 0.0;

  for(int i = 0; i < int(batch_size); i++) {
    n.clearInputs();
    stretchNumber(training_data[i]);
    int cnt = 0;
    for(int y = 0; y < 28; y++) {
      for(int x = 0; x < 28; x++) {
        n.setInput(cnt, training_data[i][y][x]);
        cnt++;
      }
    }
    n.feedForward();
    double* answers = n.returnAnswers();

    double max = 0.0;
    int ans = 0;
    for(int j = 0; j < 10; j++) {
      if(answers[j] > max) {
        max = answers[j];
        ans = j;
      }
    }
    //delete[] answers;

    if(ans == labels_train[i]) {
      right += 1.0;
    }

  }
  right /= batch_size;
  return right;
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


void learnIteration(Network &n, std::vector<std::vector<std::vector<int>>> &training_data, std::vector<int> &labels_train, int num,
                    double learning_rate, double acceleration) {
  n.clearInputs();

  //moveNumber(training_data[num]);
  stretchNumber(training_data[num]);
  int cnt = 0;
  for(int y = 0; y < 28; y++) {
    for(int x = 0; x < 28; x++) {
      n.setInput(cnt, double(training_data[num][y][x]));
      cnt++;
    }
  }

  n.feedForward();
  double* rightAnswers = new double[10];
  for(int i = 0; i < 10; i++) {
    if(i == labels_train[num])
      rightAnswers[i] = 1.0;
    else
      rightAnswers[i] = 0.0;
  }
  n.backProp(rightAnswers, learning_rate, acceleration);
  //delete [] rightAnswers;
}

int main() {
  using namespace std;
	srand(time(NULL));

  // Setting the neural network, how did we get here?
  Layer* fl = new Layer (28 * 28 + 1, true);
  Layer* il1 = new Layer (512 + 1, true);
  Layer* il2 = new Layer (128 + 1, true);
  Layer* il3 = new Layer (32 + 1, true);
  Layer* ll = new Layer (10, false);

  Layer** layers = new Layer*[5];
  layers[0] = fl;
  layers[1] = il1;
  layers[2] = il2;
  layers[3] = il3;
  layers[4] = ll;

  Network network(layers, 5);

  double learning_rate;
	double acceleration;
  cout << "Input learning rate and acceleration: ";
  cin >> learning_rate >> acceleration;
  cout << '\n';

  cout << "Do you want to load from existing file? y/n ";
  string s;
  cin >> s;

  if (s == "y")
    network.loadNeuronConnections("n.data");

  // Setting stuff to work with training data
  // Too bad
  vector<vector<vector<int>>> training_data(60000, vector<vector<int>>(28, vector<int>(28, 0)));
  vector<int> labels_train(60000, 0);
  vector<vector<vector<int>>> test_data(10000, vector<vector<int>>(28, vector<int>(28, 0)));
  vector<int> labels_test(10000, 0);

  // Getting the data from file
  cout << "Getting data from dataset...\n";
  getTrainingData(training_data, labels_train);
  getTestData(test_data, labels_test);
  cout << "Done!\n";

  // Preparing the network for learning process
  int jcount = 1;
	for(int epoch = 1; epoch <= 1000000; epoch++) {
		// Starting the learning process
		for (int j = 1; j <= 1000; j++) {
      learnIteration(network, training_data, labels_train, j, learning_rate, acceleration);

      if(jcount % 10000 == 0) {
        double error = calculateError(network, test_data, labels_train, 100.0);
        double guess = calculateGuess(network, test_data, labels_train, 100.0);
        error *= 100;
        cout << jcount << "\t error: " << error << "%    \tguess: " << guess << "   \t Epoch: \t" << epoch << "\t" << learning_rate << " " << acceleration<< endl;
      }

      if(jcount % 10000 == 0)
        network.saveNeuronConnections("n.data");

      jcount++;
		}
	}
}
