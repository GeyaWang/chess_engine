# Chess Engine Project

This is a personal project by Geya Wang over the summer of 2026.

<figure>
  <img width="400" height="400" alt="image" src="https://github.com/user-attachments/assets/a18ae7ac-4053-46dc-a6b3-bd6a5b6e8342" />
  <figcaption>Example chess game against the engine</figcaption>
</figure>


## Building
The project is tested for python 3.12 with python-chess 1.11.2 and C++20 on Ubuntu 24.04. Other versions may not work.\
Run gui/setup.sh to automatically create a python venv environment and install python dependencies.\
Run build_engine.sh to automatically build the engine as a .exe file located in engine/build.

## Playing
Play singleplayer mode by running play_singleplayer.sh or by running the command:\
"python3 gui/src/main.py"\
Play against the engine by running play_singleplayer.sh or by running the command:\
"python3 gui/src/main.py --fp={PATH_TO_ENGINE} -c={PLAYER_COLOUR}"

## Info About Project

### Backend (engine)
C++20 was chosen for its speed and ability to manage memory at a low level while having significant safety benefits over C.
The engine is based on a simple negamax algorithm with alpha-beta pruning. A large focus of the project was on optimising move generation.
To achieve this, multiple tachniques were used such as compile-time move board generation, bitboards to take advantage of fast bit operations, and avoiding heap allocations when generating moves.
Furthermore, a lot of effort was involved in implementing good OOP design techniques. The roles of each class was carefully chosen and composited together, making development and debugging effortless.

### Frontend (GUI)
The frontend uses Python due to ease of development and access to a large variety of libraries. Pygame was chosen for the display and python-chess was chosen for game logic.
Initially, I programmed both the game logic for the frontend and backend. However, due to the inherent proneness to bugs, I decided to switch to python-chess.
Some thought was put into how the frontend and backend would communicate. I decided to have the backend as a subprocess of the frontend as this mimicks the popular universal chess interface (UCI) which states the engine and GUI should communicate through stdin.
I used a context manager to manage the Engine class to ensure the subprocess is correctly terminated.
Additionally, I carefully considered every failure case and implemented error handling which was crucial in the development process.

### Areas of Improvement
The engine algorithm is quite rudimentary as it only takes into account the piece value of the positions as well as checkmates and stalemates. The evaluation function can be improved and perhaps a neural network could be used.
Furthermore, move generation could be further optimised using techniques such as magic bitboards, quiescence search, and transposition tables.
Basic unit testing was used but implementing proper techniques would have sped up development.

### Conclusion
Developing this project was a great experience. It familiarised me more with Python and C++20 and I gained experience in compile-time optimisations, memory management, bit operations as well as general programming and project devlopment techniques.
