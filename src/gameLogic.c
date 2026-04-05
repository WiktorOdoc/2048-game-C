#include <time.h>
#define GRID_SIZE 4

void srandSet() //set seed so it doesn't reset with consecutive function calls
{
    srand(time(NULL));
}


void test(int grid[GRID_SIZE][GRID_SIZE])
{
    grid[0][0] = 8192;
    grid[0][1] = 16384*2;
    grid[0][2] = 8;
    grid[0][3] = 16;
    grid[1][0] = 32;
    grid[1][1] = 64;
    grid[1][2] = 128;
    grid[1][3] = 256;
    grid[2][0] = 512;
    grid[2][1] = 1024;
    grid[2][2] = 2048;
    grid[2][3] = 16384;
}

int countEmptySquares(int grid[GRID_SIZE][GRID_SIZE]) //count the number of empty squares
{
    int emptySquareCount = 0;
    for(int i = 0; i < GRID_SIZE; i++)
    {
        for(int j = 0; j < GRID_SIZE; j++)
        {
            if(grid[i][j]==0)
                emptySquareCount++;
        }
    }
    return emptySquareCount;
}

void spawnNewSquare(int grid[GRID_SIZE][GRID_SIZE]) //spawn new square, 2 or sometimes 4.
{

    int emptySquareCount = countEmptySquares(grid);
    if(emptySquareCount == 0)
        return;
    int c = rand()%emptySquareCount; //select random empty square
    for(int i = 0; i < GRID_SIZE; i++)
    {
        for(int j = 0; j < GRID_SIZE; j++)
        {
            if(grid[i][j]==0)
            {
                if(c==0)
                {
                    int toSpawn = 2;
                    if(rand()%10 == 0) //10% chance that generated square is 4
                        toSpawn = 4;
                    grid[i][j] = toSpawn;
                    return;
                }
                c--; //countdowns all the 0 cells until c gets to 0
            }
        }
    }
}

void rotateGrid(int grid[GRID_SIZE][GRID_SIZE]) //used to rotate grid so that the graviole_and_merge function can be reused without modifications for all directions
{
    for (int i = 0; i < GRID_SIZE / 2; i++)
    {
        for (int j = i; j < GRID_SIZE - i - 1; j++)
        {
            int t = grid[i][j];
            grid[i][j] = grid[GRID_SIZE - 1 - j][i];
            grid[GRID_SIZE - 1 - j][i] = grid[GRID_SIZE - 1 - i][GRID_SIZE - 1 - j];
            grid[GRID_SIZE - 1 - i][GRID_SIZE - 1 - j] = grid[j][GRID_SIZE - 1 - i];
            grid[j][GRID_SIZE - 1 - i] = t;
        }
    }
}

//push the blocks to one side and merge. Direction: 0 -> left, 1 -> down, 2 -> right, 3 -> up
//returns 0 if didn't move anything (only generate new square if the board changed with the input) returns 1 if something changed, if 2048 was achieved returns 2048
int graviole_and_merge(int grid[GRID_SIZE][GRID_SIZE], int direction, int *score)
{
    for(int i = 0; i < direction; i++) //rotate grid to the right direction
    {
        rotateGrid(grid);
    }
    int b, c; //b is for current column, c searches along the row for the next non-zero grid to push or merge.
    int changed = 0; //if something changed return 1, if 2048 was achieved returns 2048
    for(int i = 0; i < GRID_SIZE; i++) //for each row
    {
        b = 0;
        c = 1;
        while(c < GRID_SIZE)
        {
            while(c < GRID_SIZE && grid[i][c]==0)
                c++;
            if(c < GRID_SIZE)
            {
                if(grid[i][b] == 0)          //   b   c
                {                            //  | | |2| |
                    grid[i][b] = grid[i][c];
                    grid[i][c] = 0;          //  |2| | | |
                    if(changed < 1)
                        changed = 1;
                }
                else if(grid[i][b]==grid[i][c]) //   b   c
                {                               //  |2| |2| |
                    grid[i][b] *= 2;
                    *score += grid[i][b];       //  |4| | | |
                    grid[i][c] = 0;
                    if(changed < 1)
                        changed = 1;
                    if(grid[i][b]==2048)
                        changed = 2048;
                    b++;


                }
                else                             //   b   c
                {                                //  |4| |2| |
                    b++;
                    if(b!=c)                     //  |4|2| | |
                    {
                        grid[i][b] = grid[i][c];
                        grid[i][c] = 0;
                        if(changed < 1)
                            changed = 1;
                    }
                }
                c = b + 1;
            }
        }
    }
    for(int i = 0; direction!=0 && i < 4-direction; i++) //rerotate grid back to the right direction
    {
        rotateGrid(grid);
    }
    return changed;
}

int checkLoss(int grid[GRID_SIZE][GRID_SIZE]) //check if there are possible moves
{
    if(countEmptySquares(grid)) //if there are empty squares then it is not lost
        return 0;

    for(int i = 0; i < GRID_SIZE; i++) //checks for possible merge
    {
        for(int j = 0; j < GRID_SIZE-1; j++)
        {
            if(grid[i][j]==grid[i][j+1])
                return 0;
        }
    }

    for(int i = 0; i < GRID_SIZE-1; i++)
    {
        for(int j = 0; j < GRID_SIZE; j++)
        {
            if(grid[i][j]==grid[i+1][j])
                return 0;
        }
    }
    return 1; //if no merges possible then it is a loss
}
