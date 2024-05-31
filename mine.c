#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <ncurses.h>

#define MINES_COUNT 10
#define SIZE 9


void fill_mines();
void fill_mines_around(int height, int width);
void render();
void debug();
void fill_open(int height, int width);
int make_move();
int fill_flaged(int row, int col);
int contains_flag(int row, int col);
int check_win(int not_mined_cells);

typedef struct
{
    int mines_around;
    bool mined;
    bool flag;
    bool open;
} cell;

typedef struct
{
    int height;
    int width;
} coardinates;

cell field[SIZE][SIZE];

int main(void)
{   
    //Initialize the field
    bool win_conditon;
    int not_mined_cells = (SIZE*SIZE) - MINES_COUNT;
    srand(time(NULL));
    fill_mines();
    
    for(int i = 0; i < SIZE; i++)
    {
        for(int j = 0; j < SIZE; j++)
        {
            fill_mines_around(i, j);
        }
    }

    // Game cycle
    initscr();
    noecho();
    keypad(stdscr, 1);
    int row_buff, col_buff = 0;
    render();
    move(0, 0);

    while(true)
    {
        if(make_move())
        {
            clear();
            move(0, 0);
            printw("You lose!");
            getch();
            break;
        }

        if(check_win(not_mined_cells))
        {
            clear();
            move(0, 0);
            printw("You won!");
            getch();
            break;
        }
         
        getyx(stdscr, row_buff, col_buff);
        render();
        move(row_buff, col_buff);
    }
    endwin();


}

int check_win(int not_mined_cells)
{
    int count = 0;
    for(int i = 0; i < SIZE; i++)
    {
        for(int j = 0; j < SIZE; j++)
        {
            if(field[i][j].open)
            {
                count++;
            }
        }
    }
    if(count == not_mined_cells)
    {
        return 1;
    }
    return 0;
}

int make_move()
{
    int input = getch();
    int row, col;
    getyx(stdscr, row, col);
    
    if(input == KEY_UP)
    {
        move(row-1, col);
        return 0;
    }

    if(input == KEY_DOWN)
    {
        if(row+1 <= 8)
        {
            move(row+1, col);
        }
        return 0;
    }

    if(input == KEY_LEFT)
    {
        move(row, col-1);
        return 0;
    }

    if(input == KEY_RIGHT)
    {
        if(col+1 <= 8)
        {
            move(row, col + 1);
        }
        return 0;
    }

    if(input == 'z')
    {
        if(field[row][col].mined & field[row][col].flag == false)
        {
            return 1;
        }

        if(field[row][col].open == false & field[row][col].flag == false)
        {
            fill_open(row, col);
        }
        
        if(field[row][col].open == true)
        {
            int mined = fill_flaged(row, col);
            if(mined)
            {
                return 1;
            }
        }

        return 0;
    }

    if(input == 'x')
    {
        if(field[row][col].open == false & field[row][col].flag == false)
        {
            field[row][col].flag = true;
            return 0;
        }

        if(field[row][col].flag == true)
        {
            field[row][col].flag = false;
            return 0;
        }
    }

    return 0;
}

int fill_flaged(int row, int col)
{
    if(contains_flag(row, col) == false)
    {
        return 0;
    }
            
    for(int i = row-1; i < row+2; i++)
    {
        for(int j = col-1; j < col+2; j++)
        {
            if(i < 0 || j < 0 || i >= SIZE || j >= SIZE)
            {
                continue;
            }

            if(field[i][j].mined & field[i][j].flag == false)
            {
                return 1;
            }

            if(field[i][j].open == false)
            {
                fill_open(i, j);
            }
            

        }
    }
    return 0;

}

int contains_flag(int row, int col)
{

    int flag_count = 0;

    for(int i = row-1; i < row+2; i++)
    {
        for(int j = col-1; j < col+2; j++)
        {
            if(i < 0 || j < 0 || i >= SIZE || j >= SIZE)
            {
                continue;
            }

            if(field[i][j].flag)
            {
                flag_count++;
            }
        }
    }

    if(flag_count == field[row][col].mines_around)
    {
        return 1;
    }

    return 0;
}

void fill_mines()
{   
    int len = SIZE * SIZE;
    coardinates unused[len];
    int ind = 0;
    int mines = MINES_COUNT;

    for(int i = 0; i < SIZE; i++)
    {
        for(int j = 0; j < SIZE; j++)
        {
            unused[ind].height = i;
            unused[ind].width = j;
            ind++;
        }
    }

    //Put random mines
    while(mines > 0)
    {
        int rand_coard = rand() % len;
        field[unused[rand_coard].height][unused[rand_coard].width].mined = true;
        field[unused[rand_coard].height][unused[rand_coard].width].open = false;

        for(int i = rand_coard; i < len-1; i++)
        {
            unused[i].height = unused[i+1].height; 
            unused[i].width = unused[i+1].width;
        }

        mines--;
        len--;
    }

    //Fill the rest
    for(int i = 0; i < len; i++)
    {
        field[unused[i].height][unused[i].width].mined = false;
        field[unused[i].height][unused[i].width].open = false;
    }
   
}

void fill_mines_around(int height, int width)
{
    int count = 0;

    for(int i = height-1; i < height+2; i++)
    {
        for(int j = width-1; j < width+2; j++)
        {
            if(i < 0 || j < 0 || i >= SIZE || j >= SIZE)
            {
                continue;
            }

            if(field[i][j].mined)
            {
                count++;
            }
        }
    }

    field[height][width].mines_around = count;
}

void render()
{   
    for(int i = 0; i < SIZE; i++)
    {   
        for(int j = 0; j < SIZE; j++)
        {
            move(i, j);
            if (field[i][j].open)
            {
                printw("%i ",field[i][j].mines_around);
            }
            else
            {
                if(field[i][j].flag)
                {
                    printw("F");
                }
                else
                {
                    printw("# ");
                }
            }
        }
    }
}

void fill_open(int height, int width)
{
    for(int i = height-1; i < height+2; i++)
    {
        for(int j = width-1; j < width+2; j++)
        {
            if(i < 0 || j < 0 || i >= SIZE || j >= SIZE)
            {
                continue;
            }

            if(field[i][j].mined == false & field[i][j].open == false & field[i][j].flag == false) 
            {
                field[i][j].open = true;
                if(field[i][j].mines_around == 0)
                {
                    fill_open(i, j);
                }
            }
        }
    }
}