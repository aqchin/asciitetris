# asciitetris

Overview
--------
This is a terminal-based Tetris game.
The current build has only been tested on a Windows machine and is likely not going to work on other machines due implementation.
The build is optimizes performance by pre-allocating most of the information to avoid excessive computation like rotations.

Controls
--------
'a' or LEFT_ARROW   Move left a column
'd' or RIGHT_ARROW  Move right a column
's' or DOWN_ARROW   Move down a row
'z' or '/'          Rotate clockwise
SPACE               Drop the shape down as many rows as possible
