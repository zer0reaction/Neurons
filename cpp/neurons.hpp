#pragma once
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <string>

const double e = exp(1.0);
double sigmoid(double x) { return 1.0 / (1.0 + pow(e, -1 * x)); }

class Layer {
public:
  double* neuronValues;
  double* neuronDeltas;
  double** neuronWeights;
  double** neuronWeightChanges;
  int numOfNeurons;
  Layer* nextLayer;
  Layer* prevLayer;
  bool hasBias;

  void setRandomWeights() {
    for(int i = 0; i < numOfNeurons; i++) {
      for(int j = 0; j < nextLayer->numOfNeurons; j++) {
        double weight = 0.0;

        do {
          weight = double((rand() % 201) - 100) / 100.0;
        }
        while(weight == 0.0);

        neuronWeights[i][j] = weight;
      }
    }
  }

  Layer(int numOfNeurons, bool hasBias) {
    this->numOfNeurons = numOfNeurons;
    this->hasBias = hasBias;

    neuronValues = new double[this->numOfNeurons];
    for(int i = 0; i < this->numOfNeurons; i++) {
      neuronValues[i] = 0.0;
    }
    neuronDeltas = new double[this->numOfNeurons];
    for(int i = 0; i < this->numOfNeurons; i++) {
      neuronDeltas[i] = 0.0;
    }

    if(hasBias) {
      neuronValues[this->numOfNeurons - 1] = 1.0;
    }

    neuronWeights = new double* [this->numOfNeurons];
    neuronWeightChanges = new double* [this->numOfNeurons];
  }
};

class Network{
public:
  Layer** layers;
  int numOfLayers;

  void feedForward() {
    for(int i = 1; i < numOfLayers; i++) {
      Layer* thisLayer = layers[i];
      Layer* prevLayer = layers[i-1];

      for(int j = 0; j < thisLayer->numOfNeurons; j++) {
        double sum = 0.0;

        for(int k = 0; k < prevLayer->numOfNeurons; k++) {
          sum += prevLayer->neuronValues[k] * prevLayer->neuronWeights[k][j];
        }

        thisLayer->neuronValues[j] = sigmoid(sum);
      }
    }
  }

  void setInput(int neuronNum, double value) {
    layers[0]->neuronValues[neuronNum] = value;
  }

  void clearInputs() {
    for(int i = 0; i < layers[0]->numOfNeurons; i++) {
      layers[0]->neuronValues[i] = 0.0;
    }
  }

  double* returnAnswers() {
    return layers[numOfLayers-1]->neuronValues;
  }

  int saveNeuronConnections(std::string path) {
    std::ofstream fout(path);

    if(!fout.is_open())
      return -1;
    
    for(int i = 0; i < numOfLayers-1; i++) {
      Layer* thisLayer = layers[i];
      Layer* nextLayer = layers[i+1];

      for(int j = 0; j < thisLayer->numOfNeurons; j++) {
        for(int k = 0; k < nextLayer->numOfNeurons; k++) {
          double weight = thisLayer->neuronWeights[j][k];
          double weightChange = thisLayer->neuronWeightChanges[j][k];
          fout.write((char *) &weight, sizeof(double));
          fout.write((char *) &weightChange, sizeof(double));
        }
      }
    }

    return 0;
  }

  int loadNeuronConnections(std::string path) {
    std::ifstream fin(path);

    if(!fin.is_open())
      return -1;

    for(int i = 0; i < numOfLayers-1; i++) {
      Layer* thisLayer = layers[i];
      Layer* nextLayer = layers[i+1];

      for(int j = 0; j < thisLayer->numOfNeurons; j++) {
        for(int k = 0; k < nextLayer->numOfNeurons; k++) {
          double weight;
          double weightChange;
          fin.read((char *) &weight, sizeof(double));
          fin.read((char *) &weightChange, sizeof(double));

          thisLayer->neuronWeights[j][k] = weight;
          thisLayer->neuronWeightChanges[j][k] = weightChange;
        }
      }
    }
    
    return 0;
  }

  void backProp(double* rightAnswers, double learningRate, double acceleration) {
    // Calculating deltas for each neuron
    for(int i = numOfLayers-1; i > 0; i--) {
      // If it is the last layer
      if(i == numOfLayers-1) {
        Layer* thisLayer = layers[i];

        for(int j = 0; j < thisLayer->numOfNeurons; j++) {
          double error = rightAnswers[j] - thisLayer->neuronValues[j];
          double currentValue = thisLayer->neuronValues[j];
          thisLayer->neuronDeltas[j] = error * currentValue * (1.0 - currentValue);
        }
      }

      // If it is not the last layer
      else {
        Layer* thisLayer = layers[i];
        Layer* nextLayer = layers[i+1];

        for(int j = 0; j < thisLayer->numOfNeurons; j++) {
          double sum = 0.0;

          for(int k = 0; k < nextLayer->numOfNeurons; k++) {
            sum += thisLayer->neuronWeights[j][k] * nextLayer->neuronDeltas[k];
          }

          thisLayer->neuronDeltas[j] = sum * thisLayer->neuronValues[j] * (1.0 - thisLayer->neuronValues[j]);
        }
      }
    }

    // Calculating weights based on the deltas
    for(int i = numOfLayers-2; i >= 0; i--) {
      Layer* thisLayer = layers[i];
      Layer* nextLayer = layers[i+1];

      for(int j = 0; j < thisLayer->numOfNeurons; j++) {
        for(int k = 0; k < nextLayer->numOfNeurons; k++) {
          double weightGradient = thisLayer->neuronValues[j] * nextLayer->neuronDeltas[k];
          double deltaWeight = weightGradient * learningRate + acceleration * thisLayer->neuronWeightChanges[j][k];

          thisLayer->neuronWeights[j][k] = thisLayer->neuronWeights[j][k] + deltaWeight;
          thisLayer->neuronWeightChanges[j][k] = deltaWeight;
        }
      }
    }
  }

  Network(Layer** layers, int numOfLayers) {
    this->layers = layers;
    this->numOfLayers = numOfLayers;

    for(int i = 0; i < numOfLayers; i++) {
      if(i > 0) {
        layers[i]->prevLayer = layers[i-1];
      }
      if(i < numOfLayers-1) {
        layers[i]->nextLayer = layers[i+1];
      }
    }
    
    for(int i = 0; i < numOfLayers-1; i++) {
      Layer* thisLayer = this->layers[i];
      Layer* nextLayer = this->layers[i+1];

      for(int j = 0; j < thisLayer->numOfNeurons; j++) {
        thisLayer->neuronWeights[j] = new double[nextLayer->numOfNeurons];
        for(int k = 0; k < nextLayer->numOfNeurons; k++) {
          thisLayer->neuronWeights[j][k] = 0.0;
        }
        thisLayer->neuronWeightChanges[j] = new double[nextLayer->numOfNeurons];
        for(int k = 0; k < nextLayer->numOfNeurons; k++) {
          thisLayer->neuronWeightChanges[j][k] = 0.0;
        }
      }

      layers[i]->setRandomWeights();
    }
  }
};
