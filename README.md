# SnakeAI

## The game

The game is simply made with OpenGL and GLFW, with the game logic done in C++. It is pretty barebones, as it was really juste to be used to debug and visualise the AI's progress.

## The AI

I create a simple artificial neural network, with 24 inputs, corresponding to the view in 8 directions from the snake's head, 3 times to look for a wall, the snake's body, and a fruit. There are then 2 hidden layers, of 16 neurons each, using the sigmoid activation function. Four output neurons correspond to the four choices the player can make. 

Every frame, the AI is given the state of the board, with which it constructs it's 32 inputs, and choses a direction to go to. I initialise 1000 AI players with random weights and biases, and then select the two best players, to create the next generation, adding some randomness for mutations.

## Results

The AI players are able to get 25 fruits, after training for __ generations.
