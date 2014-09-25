#include "Tic_Tac_Toe.h"

#include <iostream>
#include <cstdlib>

#include <time.h>
#include <string.h>

using namespace std;

/*********************************
 * Name:    Tic_Tac_Toe::constructor
 * Purpose: Create a game from a char array (for printing purpose)
 * Receive: in_board: a char* buffer from messages
 * Return:  None
 *********************************/
Tic_Tac_Toe::Tic_Tac_Toe(char* in_board) {
    memcpy(board, in_board, get_board_size());
};

/*********************************
 * Name:    Tic_Tac_Toe::start_game
 * Purpose: Start a new game by setting all spaces to unoccupied
 * Receive: None
 * Return:  None
 *********************************/
void Tic_Tac_Toe::start_game() {
    memset(board, unoccupied, sizeof(board));
}

/*********************************
 * Name:    Tic_Tac_Toe::mark
 * Purpose: mark a space at position position iwth mark mark
 * Recieve: position: the space position to be marked.
 *          mark: the mark, either nought or cross
 * Return:  None
 *********************************/
void Tic_Tac_Toe::mark(char position, char mark) {
    board[position - 'a'] = mark;
}

/*********************************
 * Name:    Tic_Tac_Toe::position_available
 * Purpose: check if a space at a position is available
 * Recieve: position: the space position to be checked
 * Return:  true if the position is available, false otherwise.
 *********************************/
bool Tic_Tac_Toe::position_available(char position) {
    if(board[position - 'a'] != unoccupied){
        return false;
    } else {
        return true;
    }
}

/*********************************
 * Name:    Tic_Tac_Toe::get_board()
 * Purpose: return current board as a char*, so that we can put it in a message
 * Recieve: None
 * Return:  char* for the board
 *********************************/
char* Tic_Tac_Toe::get_board() {
    return board;
};

/*********************************
 * Name:    Tic_Tac_Toe::has_won
 * Purpose: check if there is a winner
 * Recieve: None
 * Return:  true if there is a winner
 *********************************/
bool Tic_Tac_Toe::has_won() {
    if((board[0] == board[1]) && (board[1] == board[2]) 
       && (board[0] != unoccupied)) {
        return true;
    } else if((board[3] == board[4]) && (board[4] == board[5]) 
              && (board[3] != unoccupied)) {
        return true;
    } else if((board[6] == board[7]) && (board[7] == board[8]) 
              && (board[6] != unoccupied)) {
        return true;
    } else if((board[0] == board[3]) && (board[3] == board[6]) 
              && (board[0] != unoccupied)) {
        return true;
    } else if((board[1] == board[4]) && (board[4] == board[7]) 
              && (board[1] != unoccupied)) {
        return true;
    } else if((board[2] == board[5]) && (board[5] == board[8]) 
              && (board[2] != unoccupied)) {
        return true;
    } else if((board[0] == board[4]) && (board[4] == board[8]) 
              && (board[0] != unoccupied)) {
        return true;
    } else if((board[2] == board[4]) && (board[4] == board[6]) 
              && (board[2] != unoccupied)) {
        return true;
    } else {
        return false;
    }
}

/*********************************
 * Name:    Tic_Tac_Toe::is_tie
 * Purpose: check if the game ends with a tie
 * Recieve: None
 * Return:  true if the game ends with a tie
 *********************************/
bool Tic_Tac_Toe::is_tie() {
    if(has_won() == true) { return false;}
    for(int i = 0; i < get_board_size(); i++) {
        if(board[i] == unoccupied) {
            return false;
        }
    }
    // reaches here means all spaces are marked
    return true;
}

/*********************************
 * Name:    Tic_Tac_Toe::print_board
 * Purpose: print the Tic-Tac-Toe board
 * Recieve: None
 * Return:  None
 *********************************/
void Tic_Tac_Toe::print_board() {
    char pboard[9];
    for(int i = 0; i < 9; i++) {
        if(board[i] == unoccupied){
            pboard[i] = i + 'a';
        } else {
            pboard[i] = board[i];
        }    
    }

    cout << "|---|---|---|" << endl;
    cout << "| " << pboard[0] << " | " << pboard[1] << " | " 
         << pboard[2] << " |" << endl;
    cout << "|---+---+---|" << endl;
    cout << "| " << pboard[3] << " | " << pboard[4] << " | " 
         << pboard[5] << " |" << endl;
    cout << "|---+---+---|" << endl;
    cout << "| " << pboard[6] << " | " << pboard[7] << " | " 
         << pboard[8] << " |" << endl;
    cout << "|---|---|---|" << endl;
}
