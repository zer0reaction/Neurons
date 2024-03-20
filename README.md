# Neurons
Simple C++ library for creating neural networks. For graphical applications install SFML.

# Usage
To create a network you shoud firstly create layer objects.
Layer layer(number of neurons on the layer including bias, is there a bias or not (bool));

#### Creating network: 
```cpp
Network network(pointer to the first element of pointers to layer objects, number of layers including first and last ones)
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
