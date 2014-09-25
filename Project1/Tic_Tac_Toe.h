#ifndef _BULLS_AND_COWS_
#define _BULLS_AND_COWS_

#include <string.h>
#include <iostream>

using namespace std;

class Tic_Tac_Toe
{
    public:
        Tic_Tac_Toe(){ start_game();};
        Tic_Tac_Toe(char* in_board);
        void mark(char position, char mark);
        bool position_available(char position);
        void start_game();
        char* get_board();
        int get_board_size() { return 9;}
        void print_board();
        static char get_mark(int i) {
            if(i == 0) {
                return nought;
            } else if(i == 1) {
                return cross;
            }
            return ' ';
        }
        bool has_won();
        bool is_tie();
    private:
        const static char unoccupied = '_';
        const static char nought = 'O';
        const static char cross = 'X';

        char board[9];
};

#endif
