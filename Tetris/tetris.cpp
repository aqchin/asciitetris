#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#define board_height 20
#define board_width 10
#define x_offset 8
#define y_offset 2

using namespace std;

/*
 * 2D bool vector for the board
 */
static vector<vector<bool>> board(board_height,vector<bool>(board_width,false));

static bool to_update = false;

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

// Intialize variables globally since they are always reused
int i,j;
int board_w2 = board_width * 2;
int d_width = (board_width+2)*2;

void display_callback() {
  system("cls");

  for(i=0; i<y_offset; i++) cout << endl;
  cout << string(x_offset, ' ');
  cout << string(d_width, '_') << endl;;

  for(i=0; i<board_height; i++) {
    cout << string(x_offset, ' ');
    cout << "<|";

    for(j=0; j<board_width; j++) {
      switch(board[i][j]) {
      case true:
        cout << "[]";
      default:
        cout << "..";
      }
    }
    cout << "|>" << endl;
  }
  cout << string(x_offset, ' ');
  cout << string(d_width,'~') << endl;
}

void update() {
  if(to_update) {
    display_callback();
    to_update = false;
  }
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

int main(int argc, char** argv) {

  // Start tetris loop
  display_callback();
  while(1) {
    update();
  }
}