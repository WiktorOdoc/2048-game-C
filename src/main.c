#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "gameLogic.h"

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 600
#define GRID_SIZE 4
#define CELL_SIZE (WINDOW_WIDTH / GRID_SIZE) // Size of each grid box
#define MARGIN 10  // Margin between grid cells
#define BUTTON_HEIGHT 40
#define TEXT_BOX_HEIGHT 40
#define MAX_TEXT_LENGTH 21


SDL_Renderer *renderer;
SDL_Window *window;
TTF_Font *font;
TTF_Font *loadfont;
TTF_Font *gridfont;
int running = 1; //game runs while == 1. When 0 it exits.
SDL_Rect text_rect;
SDL_Rect button1, button2; // Button rects for Save and Load Game
SDL_Event event;
SDL_Surface *surface;
SDL_Texture *texture;
int grid[GRID_SIZE][GRID_SIZE] = {0};
int score = 0;
int highscore = 0;

int start_sdl();
void start_gui();
void events();
void on_button_clicks(SDL_Event* event);
void cleanup_sdl();
int is_point_in_rect(int, int, SDL_Rect*);
void refreshGrid();
void refreshScoreText();
void updateHighscore();
int readHighcore();
void losewin_game(char);
void save_game();
void load_game();

int main(int argc, char *argv[])
{
    if (start_sdl() == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    highscore = readHighcore();

    start_gui();

        //test(grid); //for testing colors
    //initialize game
    spawnNewSquare(grid);
    spawnNewSquare(grid);
    refreshGrid();
    refreshScoreText();

    while (running)
    {
        events();
    }

    cleanup_sdl();
    return EXIT_SUCCESS;
}

int start_sdl() //check for SDL errors and load font
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL Initialization Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if (TTF_Init() == -1)
    {
        fprintf(stderr, "TTF Initialization Error: %s\n", TTF_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow
    (
        "2048",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window)
    {
        fprintf(stderr, "Window Creation Error: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        fprintf(stderr, "Renderer Creation Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }
    loadfont = TTF_OpenFont("resources/Consolas.ttf", 20);
    font = TTF_OpenFont("resources/Consolas.ttf", 28);
    gridfont = TTF_OpenFont("resources/Consolas.ttf", 36);
    if (!font || !gridfont)
    {
        fprintf(stderr, "Font Loading Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void start_gui() //create and render the main gui window
{
    SDL_SetRenderDrawColor(renderer, 190, 190, 190, 255);
    SDL_RenderClear(renderer);


    button1 = (SDL_Rect) // Create the Save and Load Game buttons
    {
        .x = 30,
        .y = WINDOW_HEIGHT - BUTTON_HEIGHT - 10,
        .w = 150,
        .h = BUTTON_HEIGHT
    };
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &button1);

    const char *button1_text = "Save Game";
    surface = TTF_RenderText_Solid(font, button1_text, (SDL_Color){250, 230, 235, 255});
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_Rect button1_text_rect;
    SDL_QueryTexture(texture, NULL, NULL, &button1_text_rect.w, &button1_text_rect.h);
    button1_text_rect.x = button1.x + (button1.w - button1_text_rect.w) / 2;
    button1_text_rect.y = button1.y + (button1.h - button1_text_rect.h) / 2;

    SDL_RenderCopy(renderer, texture, NULL, &button1_text_rect);
    SDL_DestroyTexture(texture);

    button2 = (SDL_Rect)
    {
        .x = WINDOW_WIDTH - 180,
        .y = WINDOW_HEIGHT - BUTTON_HEIGHT - 10,
        .w = 150,
        .h = BUTTON_HEIGHT
    };
    SDL_RenderFillRect(renderer, &button2);

    const char *button2_text = "Load Game";
    surface = TTF_RenderText_Solid(font, button2_text, (SDL_Color){250, 230, 235, 255});
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_Rect button2_text_rect;
    SDL_QueryTexture(texture, NULL, NULL, &button2_text_rect.w, &button2_text_rect.h);
    button2_text_rect.x = button2.x + (button2.w - button2_text_rect.w) / 2;
    button2_text_rect.y = button2.y + (button2.h - button2_text_rect.h) / 2;

    SDL_RenderCopy(renderer, texture, NULL, &button2_text_rect);
    SDL_DestroyTexture(texture);

    SDL_RenderPresent(renderer);
}

void events() // handle button clicks and key presses
{
    while (SDL_WaitEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            running = 0;
            break;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
        {
            on_button_clicks(&event);
        }
        if(event.type == SDL_KEYDOWN)
        {
            on_key_press(&event);
        }
    }
}

void on_button_clicks(SDL_Event* event) //handles button clicks
{
    int mouse_x = event->button.x;
    int mouse_y = event->button.y;

    if (is_point_in_rect(mouse_x, mouse_y, &button1))
    {
        save_game();
    }

    if (is_point_in_rect(mouse_x, mouse_y, &button2))
    {
        load_game();
    }
}

void on_key_press(SDL_Event* event) //handles key presses
{
    int changed = 0;
    SDL_KeyCode *press = event->key.keysym.sym;
    if(press == SDLK_LEFT || press == SDLK_a)
    {
        changed = graviole_and_merge(grid, 0, &score);
    }
    if(press == SDLK_DOWN || press == SDLK_s)
    {
        changed = graviole_and_merge(grid, 1, &score);
    }
    if(press == SDLK_RIGHT || press == SDLK_d)
    {
        changed = graviole_and_merge(grid, 2, &score);
    }
    if(press == SDLK_UP || press == SDLK_w)
    {
        changed = graviole_and_merge(grid, 3, &score);
    }
    refreshGrid();
    if(changed)
    {
        updateHighscore();
        refreshScoreText();
        spawnNewSquare(grid);
        SDL_Delay(100);
        refreshGrid();
        if(changed==2048)
        {
            SDL_Delay(333);
            losewin_game('w');
        }
        if(checkLoss(grid))
        {
            SDL_Delay(666);
            losewin_game('l');
        }


    }



}

void cleanup_sdl() //delete all windows etc
{
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

int is_point_in_rect(int x, int y, SDL_Rect *rect) //check whether button was clicked by checking coords of click and button
{
    return x >= rect->x && x <= (rect->x + rect->w) && y >= rect->y && y <= (rect->y + rect->h);
}

void refreshGrid() //reprints the values of the grid to the display
{
    for (int row = 0; row < GRID_SIZE; row++)
    {
        for (int col = 0; col < GRID_SIZE; col++)
        {
            int x = col * CELL_SIZE + MARGIN;
            int y = TEXT_BOX_HEIGHT + row * CELL_SIZE + MARGIN;
            int w = CELL_SIZE - 2 * MARGIN;
            int h = CELL_SIZE - 2 * MARGIN;
            char text[8] = ""; // Render the value inside the box

            // Draw the light gray box for each cell
            if(grid[row][col]<=0)
            {
                SDL_SetRenderDrawColor(renderer, 225, 225, 225, 255);
            }
            else
            {
                int log2grid = log(grid[row][col])/log(2);
                SDL_SetRenderDrawColor(renderer, 255, 255-3*pow(log2grid,1.75), 330-90*pow(log2grid,0.5), 255);  //change color with value
                snprintf(text, sizeof(text), "%d", grid[row][col]);
            }


            SDL_Rect cell_rect = { x, y, w, h };
            SDL_RenderFillRect(renderer, &cell_rect);

            SDL_Surface *surface = TTF_RenderText_Solid(gridfont, text, (SDL_Color){0, 0, 0, 255});
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            SDL_Rect text_rect;
            SDL_QueryTexture(texture, NULL, NULL, &text_rect.w, &text_rect.h);
            text_rect.x = x + (w - text_rect.w) / 2; // Center horizontally
            text_rect.y = y + (h - text_rect.h) / 2; // Center vertically

            SDL_RenderCopy(renderer, texture, NULL, &text_rect);
            SDL_DestroyTexture(texture);
            SDL_RenderPresent(renderer);
        }
    }
}

void refreshScoreText() //refresh the score at top the screen
{
    SDL_Rect text_box = // Create score text box
    {
        .x = 0,
        .y = 0,
        .w = WINDOW_WIDTH,
        .h = TEXT_BOX_HEIGHT
    };
    SDL_SetRenderDrawColor(renderer, 245, 205, 200, 255);
    SDL_RenderFillRect(renderer, &text_box);

    char scoreText[36];
    snprintf(scoreText, sizeof(scoreText), "Score: %d | Highscore: %d",score, highscore);
    surface = TTF_RenderText_Solid(font, scoreText, (SDL_Color){0, 0, 0, 255});
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_QueryTexture(texture, NULL, NULL, &text_rect.w, &text_rect.h);
    text_rect.x = (WINDOW_WIDTH - text_rect.w) / 2; // Center horizontally
    text_rect.y = (TEXT_BOX_HEIGHT - text_rect.h) / 2; // Center vertically

    SDL_RenderCopy(renderer, texture, NULL, &text_rect);
    SDL_DestroyTexture(texture);
    SDL_RenderPresent(renderer);

}

void updateHighscore() //update highscore and write to file.
{
    if(score > highscore)
    {
        highscore = score;
        FILE *file;
        file = fopen("resources/highscore.txt", "wt");
        fprintf(file, "%d", highscore);
        fclose(file);
    }
}

int readHighcore() //read highscore from file
{
    int t;
    FILE *file;
    file = fopen("resources/highscore.txt", "rt");
    if(file == NULL)
        return 0;
    else
    {
        fscanf(file, "%d", &t);
        return t;
    }
}

void losewin_game(char x) //lose or win game. x: 'w' -> win 'l' -> loss
{
    const int window_width2 = 300;
    const int window_height2 = 150;
    const int button_width2 = 150;
    const int button_height2 = 40;

    char *text2; //pick text based on loss/win
    if(x == 'w')
    {
        text2 = "You win!";
    }
    else
    {
        text2 = "You lose!";
    }

    SDL_Window *window2 = SDL_CreateWindow //create window
    (
        text2,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width2,
        window_height2,
        SDL_WINDOW_SHOWN
    );

    if (!window2) //check errors
    {
        fprintf(stderr, "Window Creation Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_Renderer *renderer2 = SDL_CreateRenderer(window2, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer2)
    {
        fprintf(stderr, "Renderer Creation Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window2);
        SDL_Quit();
        return;
    }

    SDL_Event event2;
    int running2 = 1;

    SDL_Rect button_rect2 = //new game button
    {
        .x = (window_width2 - button_width2) / 2,
        .y = window_height2 - button_height2 - 20,
        .w = button_width2,
        .h = button_height2
    };

        SDL_SetRenderDrawColor(renderer2, 245, 245, 245, 255);
        SDL_RenderClear(renderer2);

        //render text
        SDL_Surface *surface2 = TTF_RenderText_Solid(font, text2, (SDL_Color){0, 0, 0, 255});
        SDL_Texture *texture2 = SDL_CreateTextureFromSurface(renderer2, surface2);
        SDL_FreeSurface(surface2);

        SDL_Rect text_rect2; //set text box
        SDL_QueryTexture(texture2, NULL, NULL, &text_rect2.w, &text_rect2.h);
        text_rect2.x = (window_width2 - text_rect2.w) / 2;
        text_rect2.y = 20;

        SDL_RenderCopy(renderer2, texture2, NULL, &text_rect2);
        SDL_DestroyTexture(texture2);


        SDL_SetRenderDrawColor(renderer2, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer2, &button_rect2);

        const char *button_text2 = "New Game";
        surface2 = TTF_RenderText_Solid(font, button_text2, (SDL_Color){255, 250, 250, 255});
        texture2 = SDL_CreateTextureFromSurface(renderer2, surface2);
        SDL_FreeSurface(surface2);

        SDL_Rect button_text_rect2;
        SDL_QueryTexture(texture2, NULL, NULL, &button_text_rect2.w, &button_text_rect2.h);
        button_text_rect2.x = button_rect2.x + (button_rect2.w - button_text_rect2.w) / 2;
        button_text_rect2.y = button_rect2.y + (button_rect2.h - button_text_rect2.h) / 2;

        SDL_RenderCopy(renderer2, texture2, NULL, &button_text_rect2);
        SDL_DestroyTexture(texture2);

        SDL_RenderPresent(renderer2);

    while (running2)
    {   //when window is exited through "X" button the game doesnt reset (this is intentional)
        while (SDL_PollEvent(&event2))
        {
            if (event2.type == SDL_WINDOWEVENT && event2.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                running2 = 0;
                break;
            }
            //on button click restart game
            if (event2.type == SDL_MOUSEBUTTONDOWN && event2.button.button == SDL_BUTTON_LEFT)
            {
                int mouse_x2 = event2.button.x;
                int mouse_y2 = event2.button.y;

                if (is_point_in_rect(mouse_x2, mouse_y2, &button_rect2))
                {
                    running2 = 0; // Close the window when the new game button is clicked
                    resetGame();
                }
            }
        }
    }

    // Cleanup and close the window
    SDL_DestroyRenderer(renderer2);
    SDL_DestroyWindow(window2);
}

void resetGame() //reset game to zero
{
    for(int i = 0; i < GRID_SIZE; i++)
    {
        for(int j = 0; j < GRID_SIZE; j++)
        {
            grid[i][j] = 0;
        }
    }
    score = 0;
    spawnNewSquare(grid);
    spawnNewSquare(grid);
    refreshGrid();
    refreshScoreText();
}

void handle_text_input(char *input_text, SDL_Event *event3) //modify input string
{
    if (event3->type == SDL_TEXTINPUT) {
        // Append the new text input, ensuring it doesn't exceed the maximum length
        strncat(input_text, event3->text.text, MAX_TEXT_LENGTH - strlen(input_text) - 1);
    }

    if (event3->type == SDL_KEYDOWN) {
        if (event3->key.keysym.sym == SDLK_BACKSPACE && strlen(input_text) > 0) {
            // Remove the last character for backspace
            input_text[strlen(input_text) - 1] = '\0';
        }
    }
}

void save_game_to_file(char* name) //save current game to the file Save format: name score grid0 ... gridN ::all separated by spaces. Spaces from string get changed into _
{
    FILE *file;
    if(file==NULL)
        return;
    file = fopen("resources/savedGames.txt", "at");
    for(int i = 0; i < MAX_TEXT_LENGTH; i++)
    {
        if(name[i]==' ')
            name[i]='_';
    }
    fprintf(file, "%s",name);
    fprintf(file, " %d", score);
    for(int i = 0; i < GRID_SIZE; i++)
    {
        for(int j = 0; j < GRID_SIZE; j++)
        {
            fprintf(file, " %d", grid[i][j]);
        }
    }
    fputc('\n',file);
    fclose(file);
}

void save_game() //make window to save the game. Allow text input to name the save.
{
    const int window_width3 = 400;
    const int window_height3 = 200;
    const int text_box_width3 = 320;
    const int text_box_height3 = 40;
    const int button_width3 = 150;
    const int button_height3 = 40;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL Initialization Error: %s\n", SDL_GetError());
        return;
    }

    SDL_Window *window3 = SDL_CreateWindow
    (
        "Save Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width3,
        window_height3,
        SDL_WINDOW_SHOWN
    );

    if (!window3)
    {
        fprintf(stderr, "Window Creation Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_Renderer *renderer3 = SDL_CreateRenderer(window3, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer3)
    {
        fprintf(stderr, "Renderer Creation Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window3);
        SDL_Quit();
        return;
    }

    SDL_StartTextInput(); // Enable text input events

    char name[MAX_TEXT_LENGTH] = ""; // Buffer for the text input

    SDL_Rect input_name_text_box3 =
    {
        .x = (window_width3 - text_box_width3) / 2,
        .y = 10, // Top margin
        .w = text_box_width3,
        .h = text_box_height3
    };

    SDL_Rect input_box_rect3 =
    {
        .x = input_name_text_box3.x, // Align with the label
        .y = input_name_text_box3.y + text_box_height3 + 10, // Under the label
        .w = text_box_width3,
        .h = text_box_height3
    };

    SDL_Rect button_rect3 =
    {
        .x = (window_width3 - button_width3) / 2,
        .y = window_height3 - button_height3 - 20, // Bottom margin
        .w = button_width3,
        .h = button_height3
    };

    SDL_Event event3;
    int running3 = 1;
    int quitted = 0;

    while (running3)
    {
        SDL_Delay(50); //to reduce cpu usage
        while (SDL_PollEvent(&event3))
        {
            if (event3.type == SDL_WINDOWEVENT && event3.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                running3 = 0; // Handle window close
                quitted = 1;
            }

            if (event3.type == SDL_MOUSEBUTTONDOWN && event3.button.button == SDL_BUTTON_LEFT)
            {
                int mouse_x3 = event3.button.x;
                int mouse_y3 = event3.button.y;

                if (is_point_in_rect(mouse_x3, mouse_y3, &button_rect3))
                {
                    running3 = 0; // Close the window when the "Save Game" button is clicked
                }
            }

            // Handle text input for the input box
            handle_text_input(name, &event3);
        }

        // Clear the renderer with white background
        SDL_SetRenderDrawColor(renderer3, 255, 255, 255, 255);
        SDL_RenderClear(renderer3);

        // Render the label text box ("Input name")
        const char *label_text3 = " Type in name:";
        SDL_Surface *label_surface3 = TTF_RenderText_Solid(font, label_text3, (SDL_Color){0, 0, 0, 255}); // Black text
        SDL_Texture *label_texture3 = SDL_CreateTextureFromSurface(renderer3, label_surface3);
        SDL_FreeSurface(label_surface3);

        SDL_Rect label_text_rect3;
        SDL_QueryTexture(label_texture3, NULL, NULL, &label_text_rect3.w, &label_text_rect3.h);
        label_text_rect3.x = input_name_text_box3.x + (input_name_text_box3.w - label_text_rect3.w) / 2;
        label_text_rect3.y = input_name_text_box3.y + (input_name_text_box3.h - label_text_rect3.h) / 2; // Centered vertically

        SDL_RenderCopy(renderer3, label_texture3, NULL, &label_text_rect3);
        SDL_DestroyTexture(label_texture3);

        // Render the input box background
        SDL_SetRenderDrawColor(renderer3, 200, 200, 200, 255); // Light gray
        SDL_RenderFillRect(renderer3, &input_box_rect3);

        // Render the current text in the input box
        SDL_Surface *input_surface3 = TTF_RenderText_Solid(font, name, (SDL_Color){0, 0, 0, 255});
        SDL_Texture *input_texture3 = SDL_CreateTextureFromSurface(renderer3, input_surface3);
        SDL_FreeSurface(input_surface3);

        SDL_Rect input_text_rect3;
        SDL_QueryTexture(input_texture3, NULL, NULL, &input_text_rect3.w, &input_text_rect3.h);
        input_text_rect3.x = input_box_rect3.x + 5; // Small margin
        input_text_rect3.y = input_box_rect3.y + (input_box_rect3.h - input_text_rect3.h) / 2; // Centered vertically

        SDL_RenderCopy(renderer3, input_texture3, NULL, &input_text_rect3);
        SDL_DestroyTexture(input_texture3);

        // Render the "Save Game" button
        SDL_SetRenderDrawColor(renderer3, 100, 100, 100, 255); // Dark gray
        SDL_RenderFillRect(renderer3, &button_rect3);

        const char *button_text3 = "Save Game";
        SDL_Surface *button_surface3 = TTF_RenderText_Solid(font, button_text3, (SDL_Color){255, 255, 255, 255}); // White text
        SDL_Texture *button_texture3 = SDL_CreateTextureFromSurface(renderer3, button_surface3);
        SDL_FreeSurface(button_surface3);

        SDL_Rect button_text_rect3;
        SDL_QueryTexture(button_texture3, NULL, NULL, &button_text_rect3.w, &button_text_rect3.h);
        button_text_rect3.x = button_rect3.x + (button_rect3.w - button_text_rect3.w) / 2; // Centered in the button
        button_text_rect3.y = button_rect3.y + (button_rect3.h - button_text_rect3.h) / 2;

        SDL_RenderCopy(renderer3, button_texture3, NULL, &button_text_rect3);
        SDL_DestroyTexture(button_texture3);

        SDL_RenderPresent(renderer3);
    }

    SDL_StopTextInput(); // Disable text input events
    if(quitted==0)
        save_game_to_file(name);

    SDL_DestroyRenderer(renderer3);
    SDL_DestroyWindow(window3);
}

int count_number_of_saves() //count how many saved games there were
{
    FILE *file;
    file = fopen("resources/savedGames.txt", "rt");
    if(file==NULL)
        return 0;
    char c = 'x';
    int counter = 0;
    while(c!=EOF)
    {
        c = fgetc(file);
        if(c=='\n')
            counter++;
    }
    fclose(file);
    return counter;

}

void load_game() //load the game
{
    int array_size4 = count_number_of_saves();
    char names[array_size4][MAX_TEXT_LENGTH];
    int scores[array_size4];
    int grids[array_size4][GRID_SIZE*GRID_SIZE];
    FILE *file;
    if(file==NULL)
        return;
    file = fopen("resources/savedGames.txt", "rt");
    for(int i = 0; i < array_size4; i++)
    {
        fscanf(file, "%s", &names[i]);
        fscanf(file, " %d", &scores[i]);
        for(int j = 0; j < GRID_SIZE*GRID_SIZE; j++)
        {
            fscanf(file, " %d", &grids[i][j]);
        }
    }


    const int window_width4 = 450;
    const int box_height4 = 28;
    const int margin4 = 8;


    int window_height4 = (array_size4 * (box_height4 + margin4)) + 2 * margin4; // Total height based on array size

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL Initialization Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *window4 = SDL_CreateWindow(
        "Load Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width4,
        window_height4,
        SDL_WINDOW_SHOWN
    );

    if (!window4)
    {
        fprintf(stderr, "Window Creation Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer *renderer4 = SDL_CreateRenderer(window4, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer4)
    {
        fprintf(stderr, "Renderer Creation Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window4);
        SDL_Quit();
        return -1;
    }

    SDL_Event event4;
    int running4 = 1;
    int clicked_index4 = -1;

    while (running4)
    {
        while (SDL_PollEvent(&event4))
        {
            if (event4.type == SDL_WINDOWEVENT && event4.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                running4 = 0;
                break;
            }

            if (event4.type == SDL_MOUSEBUTTONDOWN && event4.button.button == SDL_BUTTON_LEFT)
            {
                int mouse_x4 = event4.button.x;
                int mouse_y4 = event4.button.y;

                for (int i4 = 0; i4 < array_size4; i4++)
                {
                    SDL_Rect box_rect4 = {
                        .x = margin4,
                        .y = margin4 + i4 * (box_height4 + margin4),
                        .w = window_width4 - 2 * margin4,
                        .h = box_height4
                    };

                    if (is_point_in_rect(mouse_x4, mouse_y4, &box_rect4))
                    {
                        clicked_index4 = i4;
                        running4 = 0; // Close the window
                        break;
                    }
                }
            }
        }

        // Clear the renderer with white background
        SDL_SetRenderDrawColor(renderer4, 255, 255, 255, 255);
        SDL_RenderClear(renderer4);

        // Draw the boxes and display the array elements
        for (int i4 = 0; i4 < array_size4; i4++)
        {
            SDL_Rect box_rect4 = {
                .x = margin4,
                .y = margin4 + i4 * (box_height4 + margin4),
                .w = window_width4 - 2 * margin4,
                .h = box_height4
            };

            // Alternate the background color between white and light-gray
            if (i4 % 2 == 0)
            {
                SDL_SetRenderDrawColor(renderer4, 230, 230, 230, 255); // White
            }
            else
            {
                SDL_SetRenderDrawColor(renderer4, 200, 200, 200, 255); // Light-gray
            }

            SDL_RenderFillRect(renderer4, &box_rect4);

            char text4[40];
            snprintf(text4, sizeof(text4), "%s | Score: %d", names[i4],scores[i4]); // Display index and value

            SDL_Surface *text_surface4 = TTF_RenderText_Solid(loadfont, text4, (SDL_Color){0, 0, 0, 255}); // Black text
            SDL_Texture *text_texture4 = SDL_CreateTextureFromSurface(renderer4, text_surface4);
            SDL_FreeSurface(text_surface4);

            SDL_Rect text_rect4;
            SDL_QueryTexture(text_texture4, NULL, NULL, &text_rect4.w, &text_rect4.h);
            text_rect4.x = 20;
            text_rect4.y = box_rect4.y + (box_rect4.h - text_rect4.h) / 2;

            SDL_RenderCopy(renderer4, text_texture4, NULL, &text_rect4);
            SDL_DestroyTexture(text_texture4);
        }

        SDL_RenderPresent(renderer4);
    }

    if(clicked_index4>-1)
    {
        score = scores[clicked_index4];
        for(int i = 0; i < GRID_SIZE*GRID_SIZE; i++)
        {
            grid[i/4][i%4] = grids[clicked_index4][i];
        }
        refreshGrid();
        refreshScoreText();
    }


    // Cleanup
    SDL_DestroyRenderer(renderer4);
    SDL_DestroyWindow(window4);
}
