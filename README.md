# Chess Engine Project

This is a personal project by Geya Wang over the summer of 2026.

## Building
The project is tested for python 3.12 with python-chess 1.11.2 and C++20 on Ubuntu 24.04. Other versions may not work.
Run gui/setup.sh to automatically create a python venv environment and install python dependencies.
Run build_engine.sh to automatically build the engine as a .exe file located in engine/build.

## Playing
Play singleplayer mode by running play_singleplayer.sh or by running the command: "python3 gui/src/main.py"
Play against the engine by running play_singleplayer.sh or by running the command: "python3 gui/src/main.py --fp={PATH_TO_ENGINE} -col={PLAYER_COLOUR}"
