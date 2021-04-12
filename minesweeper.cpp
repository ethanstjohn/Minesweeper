#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>


#define BEGINNER 0
#define INTERMEDIATE 1
#define ADVANCED 2
#define MAXSIDE 13
#define MAXBOMB 35
#define MAXNEIGHBOR 8

using namespace std;

int numSides;
int numBombs;



class tile{
private:
    int flipped;
    int bomb;
    bool flagged;
    int numAdjBombs;
public:
    tile* neighbor[MAXNEIGHBOR];
    tile(){
        this->flipped = 0;
        this->bomb = 0;
        this->flagged = false;
        this->numAdjBombs = 0;
    };
    ~tile(){};
    void flip()      {this->flipped = 1;}
    int isFlipped() {return this->flipped;}
    void changeFlag(){this->flagged = !this->flagged;}
    int isFlagged() {return this->flagged;}
    void setBomb()   {this->bomb = 1;}
    int isBomb()    {return this->bomb;}
    void setAdjacentBombCount(int b){this->numAdjBombs = b;}
    int getNumAdjBombs() {return this->numAdjBombs;}

};

class board{
public:
    board(){}
    ~board(){}
    tile getTile(int row, int col);
    void printBoard();
};

class rules{
public:
    void setDifficulty();
    void printRules();
    void getMove();

};

class squareTile: public tile{
private:
public:
    squareTile* neighbor[8]; //only 8 possible neighbors of square tiles
};

class squareBoard: public board{
public:
    squareTile** p;
    squareBoard(int rows, int cols){
        //Create Matrix array of Tile Pointers
        p = new squareTile*[rows];

        for(int i = 0; i < rows; i++){
            p[i] = new squareTile[cols];
        }

        //Fill Neighbor arrays in Tiles
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                int north = i - 1;
                int south = i + 1;
                int east  = j + 1;
                int west  = j - 1;
                if(north >= 0 && north < rows){
                    p[i][j].neighbor[0] = &p[north][j];
                }
                if(south >= 0 && south < rows){
                    p[i][j].neighbor[1] = &p[south][j];
                }
                if(east >=0 && east < cols){
                    p[i][j].neighbor[2] = &p[i][east];
                }
                if(west >=0 && west < cols){
                    p[i][j].neighbor[3] = &p[i][west];
                }
                if((west >=0 && west < cols) && (north >= 0 && north < rows)){
                    p[i][j].neighbor[4] = &p[north][west];
                }
                if((east >=0 && east < cols) && (north >= 0 && north < rows)){
                    p[i][j].neighbor[5] = &p[north][east];
                }
                if((west >=0 && west < cols) && (south >= 0 && south < rows)){
                    p[i][j].neighbor[6] = &p[south][west];
                }
                if((east >=0 && east < cols) && (south >= 0 && south < rows)){
                    p[i][j].neighbor[7] = &p[south][east];
                }
            }
        }
        //Place Bombs randomly

        srand(time(0));
        int bombsToPlace = numBombs;
        while(bombsToPlace > 0){
            int X = rand() % numSides;
            int Y = rand() % numSides;
            //If no bomb on randomly selected tile OK to place.
            if (p[X][Y].isBomb() == 0){
                p[X][Y].setBomb();
                bombsToPlace--;
            }
        }
    };
   ~squareBoard(){
        for(int i = 0; i < 8; i++){
            delete[] p[i];
        }
        delete[] p;
    }
    squareTile* getTile(int row, int col) {return &p[row][col];}



    int countAdjacentBombs(squareTile* t){
        int bombCount = 0;
        for(int i = 0; i < 8; i++){
            if(t->neighbor[i] != 0 && t->neighbor[i]->isBomb()) {bombCount++;}
        }
        return bombCount;
    }



    void printBoard(){
        printf("  "); //Buffer to Align
        for(int colNums = 0; colNums < numSides; colNums++){
            if(colNums < 9){printf(" %d ", (colNums+1));}
            else if(colNums == 9){printf(" %d ", (colNums+1));}
            else{printf("%d ", (colNums+1));}
        }
        for(int row = 0; row < numSides; row++){
            printf("\n"); // New Row after each row ends
            if(row < 9){printf("%d ", (row+1));} //Row # ahead of Row
            else{printf("%d", (row+1));}
            for(int col = 0; col < numSides; col++){
                squareTile* currentTile = this->getTile(row, col);
                if(currentTile->isFlagged() == 1){
                    printf("[F]");
                }
                else if((currentTile->isFlipped()) == 0){
                    printf("[ ]");
                }
                else if(currentTile->isBomb() == 1){
                    printf("[*]");
                }
                else{
                    printf("[%d]", currentTile->getNumAdjBombs());
                }

            }
        }
        printf("\n"); //New line after board is printed.
    }
    //for debugging
    void cheatBoard(){
        for(int i = 0; i < numSides; i++){
            for(int j = 0; j < numSides; j++){
                p[i][j].flip();
            }
        }
    }
};




class squareRules: public rules{
private:
    int turnCounter;
public:
    squareBoard* currentBoard;
    squareRules(squareBoard* board){
        this->currentBoard = board;
        this->turnCounter = 0;
    }
    void incTurnCounter() {this->turnCounter++;}
    int getTurnCounter() {return this->turnCounter;}
    void autoMove(squareTile* checkTile){
        for(int i = 0; i < 8; i++){
            if(checkTile->neighbor[i] != 0 && checkTile->neighbor[i]->isFlipped() == 0){
                squareTile* neighborTile = checkTile->neighbor[i];
                neighborTile->flip();
                incTurnCounter();
                neighborTile->setAdjacentBombCount(currentBoard->countAdjacentBombs(neighborTile));
                if(neighborTile->getNumAdjBombs() == 0){autoMove(neighborTile);}
            }
        }
    }
    bool validMove(int command, int row, int col){
        return (command >= 1) && (command <= 3) &&
        ((row >0) && (row <= numSides) && (col >0) && (col <= numSides));
    }

    squareTile* storedTile;
    squareTile* storeTile(squareBoard* board, int row, int col){
        storedTile = board->getTile(row, col);
    }

    void makeMove(){
        int command;
        int row;
        int col;
        printf("ENTER INTEGERS ONLY \n");
        printf("Enter Command(1 to flip, 2 to flag, 3 to unflag): ");
        scanf("%d", &command);
        printf("\nEnter Row #: ");
        scanf("%d", &row);
        printf("\nEnter Column #: ");
        scanf("%d", &col);

        storeTile(currentBoard, row-1, col-1);

        if (validMove(command, row, col)){
            squareTile* currentTile = currentBoard->getTile(row-1, col-1); //Subtract 1 from user selected row/column to get corresponding tile in array.
            if (command == 1){
                if(currentTile->isFlipped() == 0){
                    currentTile->flip();
                    if(currentTile->isBomb() == 0){
                        currentTile->setAdjacentBombCount(currentBoard->countAdjacentBombs(currentTile));
                        if(currentTile->getNumAdjBombs() == 0){autoMove(currentTile);}
                    }
                }
                else{
                    //If tile is already flipped start a new Make Move.
                    printf("*****ERROR***** Cannot flip a tile that has already been flipped. \n");
                    printf("Please enter a new move \n");
                    makeMove();
                }
            }
            else if(command == 2){
                if(currentTile->isFlagged() == 0 && !currentTile->isFlipped()){
                    currentTile->changeFlag();
                }
                else{
                    printf("*****ERROR***** Cannot flag an already flagged or flipped tile \n");
                    printf("Please enter a new move \n");
                    makeMove();
                }
            }
            else if(command == 3){
                if(currentTile->isFlagged() == 1 && !currentTile->isFlipped()){
                    currentTile->changeFlag();
                }
                else{
                    printf("****ERROR***** Cannot unflag an unflagged or flipped tile \n");
                    printf("Please enter a new move \n");
                    makeMove();
                }

            }
        }
        else{
            printf("Please enter a valid Move \n");
            makeMove();
        }
    }


};

class game{
private:
    int gameOver;
    int maxTurns;
public:
    squareBoard* currentBoard;
    squareRules* currentRules;
    void setGameOver() {this->gameOver = 1;}
    int getGameOver() {return this->gameOver;}
    void setMaxTurns() {this->maxTurns = ((numSides*numSides) - numBombs);}
    int getMaxTurns() {return this->maxTurns;}
    void setDifficulty(){
        printf("Please select difficulty \n");
        printf("(0 for BEGINNER, 1 for INTERMEDIATE, 2 for ADVANCED): ");

        int difficulty;
        scanf("%d", &difficulty);
        if(difficulty == BEGINNER){
            numSides = 7;
            numBombs = 10;
        }
        else if(difficulty == INTERMEDIATE){
            numSides = 10;
            numBombs = 20;
        }
        else if(difficulty == ADVANCED){
            numSides = 13;
            numBombs = 35;
        }
        else {
            printf("please enter a valid level: ");
            setDifficulty();
        }

    }
    Game(){
        this->gameOver = 0;
    }


    void playGame(){
        this->setDifficulty();
        this->setMaxTurns();
        currentBoard = new squareBoard(numSides, numSides);
        currentRules = new squareRules(currentBoard);
        printf("HELLO AND WELCOME TO MINESWEEPER \n");
        while(this->getGameOver() != 1 && currentRules->getTurnCounter() < getMaxTurns()){
            currentBoard->printBoard();
            currentRules->makeMove();
            if(currentRules->storedTile->isFlipped() && currentRules->storedTile->isBomb()) {this->setGameOver();}
            else if(currentRules->storedTile->isFlipped()){currentRules->incTurnCounter();}
        }
        if(getGameOver() == 1) {
            printf("You hit a bomb better luck next time!\n");
            currentBoard->printBoard();
        }
        else{
            printf("Congratulations you win!\n");
            currentBoard->printBoard();
        }
    }

};

int main(){
    game* g = new game();
    g->playGame();
return 0;
};
