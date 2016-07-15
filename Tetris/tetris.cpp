#include <ctime>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

static int board_height = 20;
static int board_width = 10;
static int x_offset = 8;
static int y_offset = 2;
static vector<vector<bool>> board(board_height,vector<bool>(board_width,false));
static bool to_update = false;

int i,j;
int d_width = board_width+2;
void display_callback() {
  // refresh
  system("cls");

  for(int i=0; i<y_offset; i++) cout << endl;
  cout << string(x_offset, ' ') << endl;
  cout << string(x_offset, ' ');
  cout << string(d_width*2,'#') << endl;
  for(i=0; i<board_height; i++) {
    cout << string(x_offset, ' ');
    cout << "##";
    for(j=0; j<board_width; j++) {
      switch(board[i][j]) {
      case true:
        cout << "##";
      default:
        cout << "  ";
      }
    }
    cout << "##" << endl;
  }
  cout << string(x_offset, ' ');
  cout << string(d_width*2,'#') << endl;
}

void update() {
  if(to_update)
    display_callback();
}

int main(int argc, char** argv) {

  // start tetris loop
  display_callback();
  while(1) {
    update();
  }
}