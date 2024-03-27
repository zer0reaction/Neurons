# Neurons
Simple C++ library for creating neural networks. For graphical applications install SFML.

# Data
Put datasets from https://drive.google.com/drive/folders/1eMy__c89U7MCRbymvN68ff0lLkH5l1sh?usp=sharing into "dataset" folder

#### Creating network: 
```cpp
Network network({number of neurons in layer 1 including bias, ...}, {bool doesLayer1HaveBias, ...});
```
Network automatically sets random weights.
Clearing input neuron values and setting them:
```cpp
network.clearInputs();
network.setInput(neruon number starting from zero, value as double);
```

#### Learning and ourputting the result:
```cpp
network.feedForward();
network.backProp(pointer to the first element of answers (double), learning rate (double), acceleration (double));
```
To return pointer to result array of doubles:
```cpp
double* answers = network.returnAnswers();
```
