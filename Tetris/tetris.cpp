/*
 * ASCII Tetris
 * 
 * Author: Alan Chin
 * Github: https://github.com/aqchin/asciitetris
 *
 * This is a fairly straightforward program.
 * It is Tetris designed to work in Window's Command Prompt.
 * The display is handled by drawing characters on the screen.
 * It should compile on any Windows-based machine.
 * The scoring system is based on down-presses and rows completed.
 *
 * Basic Controls:
 * 'a' or LEFT_ARROW  -- Move left a column
 * 'd' or RIGHT_ARROW -- Move right a column
 * 's' or DOWN_ARROW  -- Move down a row
 * 'z' or '/'         -- Rotate clockwise
 * SPACE              -- Drop the shape down as many rows as possible
 *
 * Have fun!
 */

#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

#define DEBUG false                   // Debug toggle
#define HIDE_PIVOT true               // Display shape pivot

#define board_height 20               // Height of the board
#define board_width 10                // Width of the board
#define shape_blocks 5                // NxN of each shape
#define shape_radius shape_blocks/2   // N/2 of each shape
#define x_offset 8                    // X offset of the game in console
#define y_offset 3                    // Y offset of the game in console
#define wait_time 1.f                 // Time between shape drops

#define KEY_PRESSED 0x8000
#define VK_PERIOD 190
#define VK_SLASH 191

using namespace std;

/*
 * 2D bool vector for the board.
 */
static vector<vector<bool>> board(board_height,vector<bool>(board_width,false));

static bool to_update = false;

int curRow, curCol, curType, curRot;
int nextType, nextRot;

static int score = 0;
static vector<char> keys;

time_t mtime;

/*
 * Shapes and their corresponding rotations stored to save rotation calculation.
 * This is more efficient in terms of performance since the rotations are stored.
 * However, in-place rotations could be used instead to save disk space.
 * For the scale of this project, the difference is negligible.
 *
 * Type:
 * 0=Square, 1=Line, 2=L-Shape, 3=Reverse-L, 4=Z-Shape, 5=S-Shape, 6=T-Shape
 * 
 * Orientation:
 * 0=0deg, 1=90deg, 2=180deg, 3=270deg
 */
static char shapes[7 /*type*/][4 /*orientation*/][5 /*rows*/][5 /*columns*/] {
  { // Square
    {
      {0,0,0,0,0},
      {0,0,0,0,0},
      {0,0,2,1,0},
      {0,0,1,1,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,0,0,0},
      {0,0,2,1,0},
      {0,0,1,1,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,0,0,0},
      {0,0,2,1,0},
      {0,0,1,1,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,0,0,0},
      {0,0,2,1,0},
      {0,0,1,1,0},
      {0,0,0,0,0}
    }
  },{ // Line
    {
      {0,0,0,0,0},
      {0,0,0,0,0},
      {0,1,2,1,1},
      {0,0,0,0,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,1,0,0},
      {0,0,2,0,0},
      {0,0,1,0,0},
      {0,0,1,0,0}
    },{
      {0,0,0,0,0},
      {0,0,0,0,0},
      {1,1,2,1,0},
      {0,0,0,0,0},
      {0,0,0,0,0}
    },{
      {0,0,1,0,0},
      {0,0,1,0,0},
      {0,0,2,0,0},
      {0,0,1,0,0},
      {0,0,0,0,0}
    }
  },{ // L-shape
    {
      {0,0,0,0,0},
      {0,0,1,0,0},
      {0,0,2,0,0},
      {0,0,1,1,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,0,0,0},
      {0,1,2,1,0},
      {0,1,0,0,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,1,1,0,0},
      {0,0,2,0,0},
      {0,0,1,0,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,0,1,0},
      {0,1,2,1,0},
      {0,0,0,0,0},
      {0,0,0,0,0}
    }
  },{ // Reverse-L
    {
      {0,0,0,0,0},
      {0,0,1,0,0},
      {0,0,2,0,0},
      {0,1,1,0,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,1,0,0,0},
      {0,1,2,1,0},
      {0,0,0,0,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,1,1,0},
      {0,0,2,0,0},
      {0,0,1,0,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,0,0,0},
      {0,1,2,1,0},
      {0,0,0,1,0},
      {0,0,0,0,0}
    }
  },{ // Z-Shape
    {
      {0,0,0,0,0},
      {0,0,0,1,0},
      {0,0,2,1,0},
      {0,0,1,0,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,0,0,0},
      {0,1,2,0,0},
      {0,0,1,1,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,1,0,0},
      {0,1,2,0,0},
      {0,1,0,0,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,1,1,0,0},
      {0,0,2,1,0},
      {0,0,0,0,0},
      {0,0,0,0,0}
    }
  },{ // S-Shape
    {
      {0,0,0,0,0},
      {0,0,1,0,0},
      {0,0,2,1,0},
      {0,0,0,1,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,0,0,0},
      {0,0,2,1,0},
      {0,1,1,0,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,1,0,0,0},
      {0,1,2,0,0},
      {0,0,1,0,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,1,1,0},
      {0,1,2,0,0},
      {0,0,0,0,0},
      {0,0,0,0,0}
    }
  },{ // T-Shape
    {
      {0,0,0,0,0},
      {0,0,1,0,0},
      {0,0,2,1,0},
      {0,0,1,0,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,0,0,0},
      {0,1,2,1,0},
      {0,0,1,0,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,1,0,0},
      {0,1,2,0,0},
      {0,0,1,0,0},
      {0,0,0,0,0}
    },{
      {0,0,0,0,0},
      {0,0,1,0,0},
      {0,1,2,1,0},
      {0,0,0,0,0},
      {0,0,0,0,0}
    }
  }
};

/*
 * Translation offsets for the shapes when initialized at the top of the board.
 *
 * Type:
 * 0=Square, 1=Line, 2=L-Shape, 3=Reverse-L, 4=Z-Shape, 5=S-Shape, 6=T-Shape
 */
int shapeOffsets[7 /* type */][4 /* orientation */][2 /* translation */] {
  { // Square
    {-2,-3},
    {-2,-3},
    {-2,-3},
    {-2,-3}
  },{ // Line
    {-2,-2},
    {-2,-3},
    {-2,-2},
    {-2,-3}
  },{ // L-Shape
    {-2,-3},
    {-2,-3},
    {-2,-2},
    {-2,-3}
  },{ // Reverse-L
    {-2,-3},
    {-2,-2},
    {-2,-3},
    {-2,-3}
  },{ // Z-Shape
    {-2,-3},
    {-2,-3},
    {-2,-3},
    {-2,-2}
  },{ // S-Shape
    {-2,-3},
    {-2,-3},
    {-2,-3},
    {-2,-2}
  },{ // T-Shape
    {-2,-3},
    {-2,-3},
    {-2,-3},
    {-2,-2}
  }
};

struct Shape {
  int type;
  int orientation;
  int x, y;
};

/*
 * Get random int between m and n.
 */
int randInt(int m, int n) {
  return rand() % (n-m+1) + m;
}

/*
 * Get initial X translation of the shape.
 */
int getShapeInitialX(int type, int rot) {
  return shapeOffsets[type][rot][0];
}

/*
 * Get initial Y translation of the shape.
 */
int getShapeInitialY(int type, int rot) {
  return shapeOffsets[type][rot][1];
}

/*
 * Check if a piece has reached the top of the board.
 */
bool isGameOver() {
  for(int i=0; i<board_width; i++)
    if(board[0][i]) return true;

  return false;
}

/*
 * Remove a row and shift above blocks down a row.
 */
void deleteRow(int r) {
  for(int i=r; i>0; i--) {
    for(int j=0; j<board_width; j++) {
      board[i][j] = board[i-1][j];
    }
  }
}

/*
 * Remove rows that are filled.
 * Could be optimized. The deleteRow() call is a little expensive.
 */
void deleteFilledRows() {
  for(int r=board_height-1; r>=0; r--) {
    int c=0;
    while(c<board_width) {
      if(board[r][c]==0) break;
      c++;
    }
    if(c==board_width) {
      deleteRow(r++);
      score++;
    }
  }
}

/*
 * Check if a position on the board is available.
 */
inline bool isBlockFree(int x, int y) {
  if(x<0) 
    return true;
  return (board[x][y]==0);
}

inline void setBoard(int x, int y, bool b) {
  if(x>=0 && y>=0 && x<board_height && y<board_width)
    board[x][y] = b;
}

/*
 * Stores the shape at a given location indiscriminately.
 *
 * Pre-req: Check that shape will fit in the area.
 *          Use isValidMove().
 */
void storeAt(int row, int col, int type, int rot) {
  for(int r1=row, r2=0; r1<row+shape_blocks; r1++, r2++) {
    for(int c1=col, c2=0; c1<col+shape_blocks; c1++, c2++) {
      if(shapes[type][rot][r2][c2]!=0)
        setBoard(r1, c1, true);
    }
  }
}

/*
 * Check if shape will fit in a given area of the board.
 */
bool isValidMove(int row, int col, int type, int rot) {
  for(int r1=row, r2=0; r1<row+shape_blocks; r1++, r2++) {
    for(int c1=col, c2=0; c1<col+shape_blocks; c1++, c2++) {

      // Satisfy the condition that the shape does not go OOB
      if(c1<0 || r1>=board_height || c1>=board_width) {
        if(shapes[type][rot][r2][c2]!=0)
          return false;
      }

      // Satisfy the condition that the point is not already occupied
      if(c1>=0) {
        if(shapes[type][rot][r2][c2]!=0 && !(isBlockFree(r1, c1)))
          return false;
      }
    }
  }
  return true;
}

/*
 * Checks if position on the board is occupied by the current shape.
 *
 * Args: The X and Y coordinates of the shape on the board.
 */
int containsCurShape(int x, int y) {
  int d_x = x - curRow;
  int d_y = y - curCol;

  // Out of bounds
  if(d_x>shape_blocks-1 || d_x<0 || 
     d_y>shape_blocks-1 || d_y<0)
    return 0;

  return shapes[curType][curRot][d_x][d_y];
}

// Allocate variables globally since they are always re-used
int board_w2 = board_width * 2;
int d_width = (board_width+2) * 2;

/*
 * Draw the scene.
 */
void display_callback() {
  system("cls");

  for(int i=0; i<y_offset; i++)
    cout << endl;

  cout << string(x_offset, ' ');
  cout << string(d_width-2, '_') << endl;;

  for(int i=0; i<board_height; i++) {
    cout << string(x_offset, ' ');
    cout << "|";

    for(int j=0; j<board_width; j++) {
      int c = containsCurShape(i, j);
      if(c>0) {
        switch(c) {
          case 1:
            cout << "[]"; // Draw part of the shape
            break;

          case 2:
#if HIDE_PIVOT
              cout << "[]";
#else
              cout << "()"; // Draw a pivot point
#endif
            break;

          default: // This should never happen
            break;
        }

      } else {
        switch(board[i][j]) {
          case true:
            cout << "[]"; // Draw a filled unit
            break;

          default:
            cout << ".."; // Draw an empty unit
            break;
        }
      }
    }
    cout << "|" << endl;
  }
  cout << string(x_offset, ' ');
  cout << string(d_width-2, '^') << endl;

  cout << endl << string(x_offset, ' ');
  cout << "SCORE " << score << endl;

  to_update = false;
}

/*
 * Used for creating the next shape to display.
 */
void updateShapes() {
  curType = nextType;
  curRot = nextRot;
  curRow = getShapeInitialY(curType, curRot);
  curCol = (board_width / 2) + getShapeInitialX(curType, curRot);

  nextType = randInt(0, 6);
  nextRot = randInt(0, 3);
}

// Debug controls
#if DEBUG
bool p_up = false,
     p_ccw = false;   // anticlockwise
#endif

// Control keypress checks. This prevents unwanted key spam and finer control in the game.
bool p_down = false,
     p_left = false,
     p_right = false,
     p_cw = false,    // clockwise
     p_drop = false;

/*
* Read Windows key input.
*/
void keyboard_callback() {
#if DEBUG
  // DEBUG: Move up
  if((GetAsyncKeyState(VK_UP) | GetAsyncKeyState('W')) & KEY_PRESSED) {
    if(!p_up && isValidMove(curRow-1, curCol, curType, curRot)) {
      curRow--;
      mtime = time(NULL);
      to_update = p_up = true;
    }
  } else 
    p_up = false;

  // DEBUG: Rotate anticlockwise
  if((GetAsyncKeyState(VK_PERIOD) | GetAsyncKeyState('X')) & KEY_PRESSED) {
    if(!p_ccw) {
      int newRot = (curRot-1) % 4;
      if(isValidMove(curRow, curCol, curType, newRot))
        curRot = newRot;
      to_update = p_ccw = true;
    }
  } else
    p_ccw = false;
#endif

  // Move down
  if((GetAsyncKeyState(VK_DOWN) | GetAsyncKeyState('S')) & KEY_PRESSED) {
    if(!p_down) {
      if(isValidMove(curRow+1, curCol, curType, curRot)) {
        curRow++;
        score++;

      } else {
        storeAt(curRow, curCol, curType, curRot);
        deleteFilledRows();

        if (isGameOver())
          exit(0);

        updateShapes();
      }
      mtime = time(NULL);
      to_update = p_down = true;
    }
  } else
    p_down = false;

  // Move left
  if((GetAsyncKeyState(VK_LEFT) | GetAsyncKeyState('A')) & KEY_PRESSED) {
    if(!p_left && isValidMove(curRow, curCol-1, curType, curRot)) {
      curCol--;
      to_update = p_left = true;
    }
  } else
    p_left = false;

  // Move right
  if((GetAsyncKeyState(VK_RIGHT) | GetAsyncKeyState('D')) & KEY_PRESSED) {
    if(!p_right && isValidMove(curRow, curCol+1, curType, curRot)) {
      curCol++;
      to_update = p_right = true;
    }
  } else
    p_right = false;

  // Rotate clockwise
  if((GetAsyncKeyState(VK_SLASH) | GetAsyncKeyState('Z')) & KEY_PRESSED) {
    if(!p_cw) {
      int newRot = (curRot+1) % 4;
      if(isValidMove(curRow, curCol, curType, newRot))
        curRot = newRot;
      to_update = p_cw = true;
    }
  } else
    p_cw = false;

  // Drop the shape to the bottom-most space
  if(GetAsyncKeyState(VK_SPACE) & KEY_PRESSED) {
    if(!p_drop) {
      while(isValidMove(curRow+1, curCol, curType, curRot))
        curRow++;

      storeAt(curRow, curCol, curType, curRot);
      deleteFilledRows();

      if (isGameOver())
        exit(0);

      updateShapes();

      to_update = p_drop = true;
    }
  } else
    p_drop = false;
}

/*
 * Reset the keypress checks.
 */
inline void resetKeys() {
#if DEBUG
  p_up = p_ccw = false;
#endif
  p_left = p_right = p_down = p_drop = false;
}

/*
 * Idle callback.
 *
 * Updates the interface when:
 *    - Specified time is met
 *    - Key is pressed
 */
void update() {
  time_t cur_time = time(NULL);

  if(difftime(cur_time, mtime) > wait_time) {
    if(isValidMove(curRow+1, curCol, curType, curRot))
      curRow++;

    else {
      storeAt(curRow, curCol, curType, curRot);
      deleteFilledRows();

      if(isGameOver())
        exit(0);

      updateShapes();
    }
    mtime = cur_time;
    to_update = true;
  }

  if(!to_update)
    keyboard_callback();

  else {
    display_callback();
#if DEBUG
    cout << curType << " " << curRot << " " << curRow << " " << curCol << endl;
#endif
  }
}

/*
 * Main.
 *
 * Intialize first shapes and start loop routine.
 */
int main(int argc, char** argv) {

  srand(time(NULL));

  // Initialize the first piece
  curType = randInt(0, 6);
  curRot = randInt(0, 3);
  curRow = getShapeInitialY(curType, curRot);
  curCol = (board_width/2) + getShapeInitialX(curType, curRot);

  // Initialize the next piece
  nextType = randInt(0, 6);
  nextRot = randInt(0, 3);

  // Initialize time
  time(&mtime);

  // Start tetris loop
  display_callback();

  while(true)
    update();
}