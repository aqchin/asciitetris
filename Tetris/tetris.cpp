#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

#define board_height 20               // Height of the board
#define board_width 10                // Width of the board
#define shape_blocks 5                // NxN of each shape
#define shape_radius shape_blocks/2   // N/2 of each shape
#define x_offset 8                    // Y offset of the game in console
#define y_offset 2                    // X offset of the game in console
#define wait_time 1.f                 // Time between shape drops

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_PRESSED 0x8000

using namespace std;

/*
 * 2D bool vector for the board
 */
static vector<vector<bool>> board(board_height,vector<bool>(board_width,false));

static bool to_update = false;

int curRow, curCol, curType, curRot;
int nextType, nextRot;

static int score = 0;

time_t mtime;

/*
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
 *Get random int between m and n
 */
int randInt(int m, int n) {
  return rand() % (n-m+1) + m;
}

int getShapeInitialRow(int type, int rot) {
  return shapeOffsets[type][rot][0];
}

int getShapeInitialCol(int type, int rot) {
  return shapeOffsets[type][rot][1];
}

bool isGameOver() {
  for(int i=0; i<board_width; i++)
    if(board[0][i]) return true;

  return false;
}

void deleteRow(int r) {
  for(int i=r; i>0; i--) {
    for(int j=0; j<board_width; j++) {
      board[i][j] = board[i-1][j];
    }
  }
}

// Could make this more efficient
void deleteFilledRows() {
  for(int r=0; r<board_height; r++) {
    int c=0;
    while(c<board_width) {
      if(board[r][c]==1) break;
      c++;
    }
    if(c==board_width) {
      deleteRow(r);
      score++;
    }
  }
}

bool isBlockFree(int x, int y) {
  return (board[x][y]==0);
}

/*
 * Pre-req: check that shape will fit in the area
 */
void storeAt(int x, int y, int type, int rot) {
  for(int r1=x, r2=0; r1<x+shape_blocks; r1++, r2++) {
    for(int c1=y, c2=0; c1<y+shape_blocks; c1++, c2++) {
      if(shapes[type][rot][r2][c2]!=0)
        board[r1][c1] = true;
    }
  }
}

bool isValidMove(int x, int y, int type, int rot) {
  for (int r1=x, r2=0; r1<x + shape_blocks; r1++, r2++) {
    for (int c1=y, c2=0; c1<y + shape_blocks; c1++, c2++) {
      if(r1<0 || r1 > board_width-1 || c1 > board_height-1) {
        if(shapes[type][rot][x][y]!=0)
          return false;
      }
      if(c1>=0) {
        if(shapes[type][rot][x][y]!=0 && !(isBlockFree(r1, c1)))
          return false;
      }
    }
  }
  return true;
}

/*
 * Checks if position on the board is occupied by the current shape
 * Args: location on the board
 */
bool containsCurShape(int x, int y) {
  int d_x = x - curRow;
  int d_y = y - curCol;

  // Out of bounds
  if(d_x>shape_radius || d_x<-shape_radius || 
     d_y>shape_radius || d_y<-shape_radius)
    return false;

  if(shapes[curType][curRot][shape_radius+d_x][shape_radius+d_y]>0)
    return true;

  return false;
}

void updateShapes() {
  curType = nextType;
  curRot = nextRot;
  curRow = (board_width/2) + getShapeInitialRow(curType, curRot);
  curCol = getShapeInitialCol(curType, curRot);

  nextType = randInt(0, 6);
  nextRot = randInt(0, 3);
}

// Allocate variables globally since they are always reused
int i, j;
int board_w2 = board_width * 2;
int d_width = (board_width+2) * 2;

/*
 * Draws the scene
 */
void display_callback() {
  system("cls");

  for (i=0; i<y_offset; i++)
    cout << endl;

  cout << string(x_offset, ' ');
  cout << string(d_width, '_') << endl;;

  for(i=0; i<board_height; i++) {
    cout << string(x_offset, ' ');
    cout << "<|";

    for(j=0; j<board_width; j++) {
      if(i==curRow && j==curCol) {
        cout << "()"; // Draw a pivot point

      } else if(containsCurShape(i, j)) {
        cout << "[]"; // Draw part of the shape

      } else {
        switch(board[i][j]) {
        case true:
          cout << "[]"; // Draw a filled unit
          break;

        default:
          //cout << ".."; // Draw an empty unit
          cout << i%10 << j%10;
          break;
        }
      }
    }
    cout << "|>" << endl;
  }
  cout << string(x_offset, ' ');
  cout << string(d_width, '~') << endl;

  to_update = false;
}

/*
 * Reads Windows key input
 */
void keyboard_callback() {

  if(KEY_PRESSED & (GetKeyState('s') | GetKeyState(KEY_DOWN))) {
    if (isValidMove(curRow, curCol + 1, curType, curRot)) {
      curCol++;
      to_update = true;
    }
  }
  /*
  switch(_getch()) {
    case KEY_UP:
    case 'w':
    case 'W':
      break;

    case KEY_DOWN:
    case 's':
    case 'S':
      if(isValidMove(curRow, curCol+1, curType, curRot)) {
        curCol++;
        to_update = true;
      }
      break;

    case KEY_LEFT:
    case 'a':
    case 'A':
      if(isValidMove(curRow-1, curCol, curType, curRot)) {
        curRow--;
        to_update = true;
      }
      break;

    case KEY_RIGHT:
    case 'd':
    case 'D':
      if(isValidMove(curRow+1, curCol, curType, curRot)) {
        curRow++;
        to_update = true;
      }
      break;

    case 'z':
    case 'Z':
    case '/':
    case '?':
      // Clockwise rotation
      if(isValidMove(curRow, curCol, curType, (curRot+1)%4)) {
        curRot = (curRot+1) %4;
        to_update = true;
      }
      break;

    case 'x':
    case 'X':
    case '.':
    case '>':
      // Counter-clockwise rotation
      if(isValidMove(curRow, curCol, curType, (curRot-1)%4)) {
        curRot = (curRot-1) %4;
        to_update = true;
      }
      break;

    case ' ':
      while(isValidMove(curRow, curCol, curType, curRot))
        curCol++;
      storeAt(curRow, curCol-1, curType, curRot);
      deleteFilledRows();
      
      if(isGameOver())
        exit(0);

      updateShapes();
      to_update = true;
      break;

    default:
      break;
  }
  */
}

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
    }
    mtime = cur_time;
    to_update = true;
  }

  keyboard_callback();

  if(to_update) {
    display_callback();
    cout << curType << " " << curRot << " " << curRow << " " << curCol << endl;
  }
}

int main(int argc, char** argv) {

  srand(time(NULL));

  // Initialize the first piece
  curType = randInt(0, 6);
  curRot = randInt(0, 3);
  curRow = (board_width/2) + shape_radius + getShapeInitialRow(curType, curRot);
  curCol = shape_radius + getShapeInitialCol(curType, curRot);


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