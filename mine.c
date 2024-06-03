#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

// Define colors;
#define COLOR_0 0
#define COLOR_1 4
#define COLOR_2 2
#define COLOR_3 1
#define COLOR_4 8 
#define COLOR_5 9
#define COLOR_6 10
#define COLOR_7 5
#define COLOR_8 7
#define COLOR_CELL 3
#define COLOR_FLAG 5
int colors[] = {
   COLOR_0,
   COLOR_1,
   COLOR_2,
   COLOR_3,
   COLOR_4,
   COLOR_5,
   COLOR_6,
   COLOR_7,
   COLOR_8,
   COLOR_CELL,
   COLOR_FLAG,
};

//Prototypes
void fill_mines();
void fill_mines_around(int row, int col);
void render();
void fill_open(int row, int col);
int make_move();
int fill_flaged(int row, int col);
int contains_flag(int row, int col);
int check_win(int not_mined_cells);
void render_end();
void malloc_field();
void free_field();

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

//Global variables
cell **field;
int height, width, size, mines_count;

int main(int argc, char const *argv[])
{   
    //Choose difficulty
    if(argc != 2)
    {
        printf("Usage: ./mine -[difficulty] (easy, normal, hard)");
        return 1;
    }
    if(strcmp(argv[1], "-easy") == 0)
    {
        height = 9;
        width = 9;
        mines_count = 10;
    }
    else if(strcmp(argv[1], "-normal") == 0)
    {
        height = 16;
        width = 16;
        mines_count = 40;
    }
    else if(strcmp(argv[1], "-hard") == 0)
    {
        height = 16;
        width = 30;
        mines_count = 99;
    }
    else
    {
        printf("Usage: ./mine -[difficulty] (easy, normal, hard)");
        return 1;
    }

    //Initialize the field
    size = height*width;
    int not_mined_cells = (size) - mines_count;
    malloc_field();
    srand(time(NULL));
    fill_mines();
    
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            fill_mines_around(i, j);
        }
    }

    initscr();
    if(has_colors() == FALSE)
	{	endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}
    start_color();

    // Color pairs
    init_color(8, 0, 0, 500);
    init_color(9, 300, 0, 0);
    init_color(10, 0, 1000, 1000);

    for (int i = 1; i <= 11; i++)
    {
        init_pair(i, colors[i-1], COLOR_BLACK);
    }

    noecho();
    keypad(stdscr, 1);
    int row_buff, col_buff = 0;
    render();
    move(0, 0);

    //Game cycle
    while(true)
    {
        if(make_move())
        {
            clear();
            render_end();
            printw("You lost!");
            getch();
            break;
        }

        if(check_win(not_mined_cells))
        {
            clear();
            render_end();
            printw("You won!");
            getch();
            break;
        }
         
        getyx(stdscr, row_buff, col_buff);
        render();
        move(row_buff, col_buff);
    }
    endwin();
    free_field();
}

void malloc_field()
{
    field = (cell**)malloc(height * sizeof(cell*));
    for(int i = 0; i < height; i++)
    {
        field[i] = (cell*)malloc(width * sizeof(cell));
    }
}

void free_field()
{
    for(int i = 0; i < height; i++)
    {
        free(field[i]);
    }

    free(field);
}


int check_win(int not_mined_cells)
{
    int count = 0;
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
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
        if(row+1 < height)
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
        if(col+1 < width)
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
            if(fill_flaged(row, col))
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
            if(i < 0 || j < 0 || i >= height || j >= width)
            {
                continue;
            }

            if(field[i][j].mined & field[i][j].flag == false)
            {
                return 1;
            }

            if(field[i][j].open == false & field[i][j].flag == false)
            {
                field[i][j].open = true;
                if(field[i][j].mines_around == 0)
                {
                    fill_open(i, j);
                }
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
            if(i < 0 || j < 0 || i >= height || j >= width)
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
    int len = size;
    coardinates unused[len];
    int ind = 0;
    int mines = mines_count;

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
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

void fill_mines_around(int row, int col)
{
    int count = 0;

    for(int i = row-1; i < row+2; i++)
    {
        for(int j = col-1; j < col+2; j++)
        {
            if(i < 0 || j < 0 || i >= height || j >= width)
            {
                continue;
            }

            if(field[i][j].mined)
            {
                count++;
            }
        }
    }

    field[row][col].mines_around = count;
}

void render()
{   
    for(int i = 0; i < height; i++)
    {   
        for(int j = 0; j < width; j++)
        {
            move(i, j);
            if (field[i][j].open)
            {
                attron(COLOR_PAIR(field[i][j].mines_around+1));
                printw((field[i][j].mines_around == 0) ? " " : "%i",field[i][j].mines_around);
                attroff(COLOR_PAIR(field[i][j].mines_around+1));
                continue;
            }
            else
            {
                if(field[i][j].flag)
                {
                    attron(COLOR_PAIR(11));
                    printw("F");
                    attroff(COLOR_PAIR(11));
                    continue;
                }
                else
                {
                    attron(COLOR_PAIR(10));
                    printw("#");
                    attroff(COLOR_PAIR(10));
                }
            }
        }
    }
}

void render_end()
{
    for(int i = 0; i < height; i++)
    {   
        for(int j = 0; j < width; j++)
        {
            move(i, j);
            if(field[i][j].mined)
            {
                attron(COLOR_PAIR(11));
                printw("*");
                attroff(COLOR_PAIR(11));
                continue;
            }
            else
            {
                attron(COLOR_PAIR(field[i][j].mines_around+1));
                printw((field[i][j].mines_around == 0) ? " " : "%i",field[i][j].mines_around);
                printw((field[i][j].mines_around == 0) ? " " : "%i",field[i][j].mines_around);
                attroff(COLOR_PAIR(field[i][j].mines_around+1));
                continue;
            }
        }
    }

    int row, col;
    getyx(stdscr, row, col);
    row++;
    move(row, 0);
}

void fill_open(int row, int col)
{
    for(int i = row-1; i < row+2; i++)
    {
        for(int j = col-1; j < col+2; j++)
        {
            if(i < 0 || j < 0 || i >= height || j >= width)
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