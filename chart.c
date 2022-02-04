#include <stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <sys/select.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>


#define MAX_FLAG_COUNT 16
#define FONT_FILE "unispace/unispace_bold.ttf"
#define SMALL_FONT_FILE "unispace/unispace_italic.ttf"

/**
 * @brief check if stdin can be read
 * 
 * @return int 1 - available 0 - not available
 */
int inputAvailable(void);


enum halign {HLEFT, HCENTER, HRIGHT};
enum valign {VUP, VCENTER, VDOWN};

/**
 * @brief render text blender
 * 
 * @param rend render
 * @param text text to render
 * @param font used font
 * @param x x position on the window
 * @param y y position on the window
 * @param ha horizontal align
 * @param va vertical align
 * @param r red
 * @param g green
 * @param b blue
 */
void textBlendedRGB (SDL_Renderer* rend, char* text, TTF_Font* font, 
        Uint16 x, Uint16 y, int ha, int va, Uint8 r, Uint8 g, Uint8 b);

/**
 * @brief add value to the end of the values array and remove the first
 * 
 * @param values values array
 * @param size array length
 * @param value new added value
 */
void push_value (double* values, unsigned size, double value);

/**
 * @brief render signal plot
 * 
 * @param rend renderer
 * @param values signal values
 * @param size signal size
 * @param min minimal signal value
 * @param max maximal signal value
 * @param x x position on the window
 * @param y y position on the window
 * @param w width of the plot
 * @param h heigth of the plot
 * @param r red
 * @param g green
 * @param b blue
 */
void singnalRGB(SDL_Renderer* rend, double* values, unsigned size, double min, 
        double max, Uint16 x, Uint16 y,  Uint16 w, Uint16 h, Uint8 r, Uint8 g, 
        Uint8 b);

/**
 * @brief render guides to the signal
 * 
 * @param rend renderer
 * @param val_i signal value index
 * @param value signal value
 * @param min minimal signal value
 * @param max maximal signel value
 * @param size length of the signal
 * @param x x position on the window
 * @param y y position on the window
 * @param w width of the window
 * @param h heigth of the window
 * @param r red
 * @param g green
 * @param b blue
 */
void signalGuidesRGB(SDL_Renderer* rend, int val_i, double value, double min, 
        double max, double size, Uint16 x, Uint16 y,  Uint16 w, Uint16 h, 
        Uint8 r, Uint8 g, Uint8 b);

/**
 * @brief print help text
 * 
 */
void print_help(void);

int main(int argc, char const *argv[])
{
    enum {RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN, WHITE, ORANGE, GRAY};
    unsigned color = RED;
    unsigned length = 100;
    bool output = false;
    bool file_mode = false;
    FILE* file = NULL;
    double wait_time = 0; // s

    // arguments
    for (int i = 1; i < argc;++i) {
        // print help
        if (strcmp(argv[i], "-h") == 0) {
            print_help();
            return 1;
        }

        if (strcmp(argv[i], "-c") == 0) {
            // check for option
            if (i + 1 == argc) {
                fprintf(stderr, "-c needs additional option\n");
                return -1;
            }

            // set color
            if (strcmp(argv[i + 1], "red") == 0) {
                color = RED;
            } else if (strcmp(argv[i + 1], "green") == 0) {
                color = GREEN;
            } else if (strcmp(argv[i + 1], "blue") == 0) {
                color = BLUE;
            } else if (strcmp(argv[i + 1], "yellow") == 0) {
                color = YELLOW;
            } else if (strcmp(argv[i + 1], "magenta") == 0) {
                color = MAGENTA;
            } else if (strcmp(argv[i + 1], "cyan") == 0) {
                color = CYAN;
            } else if (strcmp(argv[i + 1], "white") == 0) {
                color = WHITE;
            } else if (strcmp(argv[i + 1], "orange") == 0) {
                color = ORANGE;
            } else if (strcmp(argv[i + 1], "gray") == 0) {
                color = GRAY;
            } else {
                fprintf(stderr, "color %s is not implemented\n", argv[i + 1]);
                return -1;
            }

            ++i;
        }

        // l - length
        if (strcmp(argv[i], "-l") == 0) {
            // check for option
            if (i + 1 == argc) {
                fprintf(stderr, "-l needs additional numeric option\n");
                return -1;
            } 

            length = atoi(argv[i + 1]);
            ++i;
        }

        // T - wait time
        if (strcmp(argv[i], "-T") == 0) {
            // check for option
            if (i + 1 == argc) {
                fprintf(stderr, "-T needs additional numeric option\n");
                return -1;
            } 

            wait_time = atof(argv[i + 1]);
            ++i;
        }

        // o - output
        if (strcmp(argv[i], "-o") == 0) {
            // check for option
            if (i + 1 == argc) {
                fprintf(stderr, "-o needs additional boolean option\n");
                return -1;
            } 

            if (strcmp(argv[i + 1], "true") == 0) {
                output = true;
            } else if (strcmp(argv[i + 1], "false") == 0) {
                output = false;
            } else {
                fprintf(stderr, "bool needs to be true or false\n");
            }

            ++i;
        }

        // f - file input mode
        if (strcmp(argv[i], "-f") == 0) {
            // check for option
            if (i + 1 == argc) {
                fprintf(stderr, "-f needs additional filename option\n");
                return -1;
            } 


            file = fopen(argv[i + 1], "r");
            if (file == NULL) {
                fprintf(stderr, "file is not readable %s\n", argv[i + 1]);
                return -1;
            }
            file_mode = true;

            ++i;
        }

    }

    Uint8 lr, lg, lb;
    switch (color) {
    case RED:
        lr = 255; 
        lg = 0; 
        lb = 0;
        break;

    case GREEN:
        lr = 0;
        lg = 255;
        lb = 0;
        break;
    
    case BLUE:
        lr = 0;
        lg = 0;
        lb = 255;
        break;

    case YELLOW:
        lr = 255;
        lg = 255;
        lb = 0;
        break;

    case MAGENTA:
        lr = 255;
        lg = 0;
        lb = 255;
        break;

    case CYAN:
        lr = 0;
        lg = 255;
        lb = 255;
        break;

    case WHITE:
        lr = 255;
        lg = 255;
        lb = 255;
        break;

    case ORANGE:
        lr = 255;
        lg = 128;
        lb = 0;
        break;

    case GRAY:
        lr = 128;
        lg = 128;
        lb = 128;
        break;

    default:
        lr = 255;
        lg = 255;
        lb = 255;
        break;
    }


    double min = INFINITY;
    double max = -INFINITY;

    double values[length];
    for (int i = 0; i < length; ++i) {
        values[i] = 0;
    }

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    TTF_Font* font = TTF_OpenFont("unispace/unispace_bold.ttf", 12);
    if (font == NULL) {
        font = TTF_OpenFont("/usr/local/lib/chart/unispace/unispace_bold.ttf", 12);
    }

    TTF_Font* font_small = TTF_OpenFont("unispace/unispace_italic.ttf", 12);
    if (font_small == NULL) {
        font_small = TTF_OpenFont("/usr/local/lib/chart/unispace/unispace_italic.ttf", 12);
    }
    

    int fh;
    TTF_SizeText(font, "TEXT", NULL, &fh);

    char title[64];
    sprintf(title, "Graph %d samples", length);
    SDL_Window* win = SDL_CreateWindow(title, 0, 0, 800, 800, 0);
    SDL_Renderer* rend = SDL_CreateRenderer(win, 0, SDL_RENDERER_ACCELERATED);
    SDL_SetWindowResizable(win, SDL_TRUE);

    bool update = false;

    SDL_Event e;
    bool quit = false;
    bool pause = false;

    clock_t t_prev = 0;
    clock_t t_now = 0;

    while (!quit) {
        t_now = clock();

        // check scan
        if (!file_mode) {    
            if (inputAvailable() && !pause && t_now > t_prev + wait_time 
                    * CLOCKS_PER_SEC) {
                double new_value;
                int f = scanf("%lf", &new_value);
                if (f == 1) {
                    if (output) {
                        printf("%lf\n", new_value);
                        fflush(stdout);
                    }

                    push_value(values, length, new_value);

                    // update min max
                    if(new_value < min) {min = new_value;}
                    if(new_value > max) {max = new_value;}

                    update = true;
                }

                t_prev = t_now;
            }
        } else {
            double new_value;
            if (fscanf(file, "%lf", &new_value) == 1) {
                if (output) {
                        printf("%lf\n", new_value);
                        fflush(stdout);
                    }
                    push_value(values, length, new_value);

                    // update min max
                    if(new_value < min) {min = new_value;}
                    if(new_value > max) {max = new_value;}

                    update = true;
            }
        }

        // check for events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } 

            if (e.type == SDL_KEYDOWN && 
                    e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                pause = !pause;
            }

            if (e.type == SDL_MOUSEMOTION) {
                update = true;
            }
        }

        if (update) {
            // plot
            int w, h;
            SDL_GetRendererOutputSize(rend, &w, &h);

            SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
            SDL_RenderClear(rend);

            singnalRGB(rend, values, length, min, max, 0, 0, w, h - fh, 
                    lr, lg, lb);

            // separator
            SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

            SDL_RenderDrawLineF(rend, 0, h - fh, w, h - fh);

            // info text
            char info1[64];
            sprintf(info1, "max: %lf, min: %lf", max, min);
            textBlendedRGB(rend, info1, font, 10, h - fh, HLEFT, VUP,
                    255, 255, 255);

            char info2[64];
            sprintf(info2, "last: %lf", values[length - 1]);
            textBlendedRGB(rend, info2, font, w - 10, h - fh, HRIGHT, VUP,
                    255, 255, 255);

            if (SDL_GetMouseFocus() == win) {
                int mx, my;
                SDL_GetMouseState(&mx, &my);

                char mouse_info[64];
                int x = (double)mx / (double)w * length;
                double y = values[x];

                signalGuidesRGB(rend, x, y, min, max, length, 0, 0, w, 
                        h - fh, 64, 64, 64);
                sprintf(mouse_info, "%4d : %lf", x, y);

                int tw, th;
                TTF_SizeText(font_small, mouse_info, &th, &tw);
                int spcx = 5; 
                int spcy = 5;

                if (my > w - fh) {
                    my = w - fh;
                }

                int ha = HRIGHT;
                if (mx - 5 < th) {
                    ha = HLEFT;
                    spcx = -5;
                }

                int va = VDOWN;
                if (my - 5 < tw) {
                    va = VUP;
                    spcy = -5;
                }

                textBlendedRGB(rend, mouse_info, font_small, mx - spcx, 
                        my - spcy, ha, va, 64, 64, 64);
            }

            SDL_RenderPresent(rend);
        }
            

        usleep(100);
    }


    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();

    TTF_CloseFont(font);
    TTF_CloseFont(font_small);
    TTF_Quit();

    if (file_mode) {
        fclose(file);
    }


    return 0;
}


int inputAvailable() {
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return (FD_ISSET(0, &fds));
}


void textBlendedRGB (SDL_Renderer* rend, char* text, TTF_Font* font, 
        Uint16 x, Uint16 y, int ha, int va, Uint8 r, Uint8 g, Uint8 b) {
    SDL_Color color = {r, g, b};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, surface);

    SDL_Rect rect;

    rect.w = surface->w;
    rect.h = surface->h;

    switch (ha) {
    case HLEFT:
        rect.x = x;
        break;
    
    case HRIGHT:
        rect.x = x - surface->w;
        break;

    case HCENTER:
        rect.x = x - surface->w / 2;
        break;
    
    default:
        break;
    }

    switch (va) {
    case VUP:
        rect.y = y;
        break;
    
    case VDOWN:
        rect.y = y - surface->h;
        break;

    case VCENTER:
        rect.y = y - surface->h / 2;
        break;
    
    default:
        break;
    }
    
    
    SDL_RenderCopy(rend, texture, NULL, &rect); 
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


void push_value (double* values, unsigned size, double value) {
    for (int i = 1; i < size; ++i) {
        values[i - 1] = values[i];
    }
    values[size - 1] = value;
}


void singnalRGB(SDL_Renderer* rend, double* values, unsigned size, double min, 
        double max, Uint16 x, Uint16 y,  Uint16 w, Uint16 h, Uint8 r, Uint8 g, 
        Uint8 b) {
    double t_max = max < 1 ? 1 : max;
    double t_min = min > -1 ? -1 : min;

    SDL_SetRenderDrawColor(rend, r, g, b, 255);
    double x1, y1, x2, y2;
    for (int i = 1; i < size; ++i) {
        x1 = (double)(i - 1) * (double)w / ((double)size - 1);
        y1 = (values[i - 1] - t_min) * h / (t_max - t_min);
        x2 = i * (double)w / ((double)size - 1);
        y2 = (values[i] - t_min) * h / (t_max - t_min);
        thickLineRGBA(rend, x1, h - y1, x2, h - y2, 2, r, g, b, 255);
    }

    // render zero line
    if (t_min < 0 && 0 < t_max) {
        double y = -t_min * h / (t_max - t_min);

        SDL_SetRenderDrawColor(rend, 128, 128, 128, 255);

        SDL_RenderDrawLineF(rend, 0, h - y, w, h - y);
    }
}


void signalGuidesRGB(SDL_Renderer* rend, int val_i, double value, double min, 
        double max, double size, Uint16 x, Uint16 y,  Uint16 w, Uint16 h, 
        Uint8 r, Uint8 g, Uint8 b) {
    double t_max = max < 1 ? 1 : max;
    double t_min = min > -1 ? -1 : min;
    int w_x = x + w * ((double)val_i / (double)size);
    double w_y = (value - t_min) * h / (t_max - t_min);
    SDL_SetRenderDrawColor(rend, r, g, b, 255);
    SDL_RenderDrawLineF(rend, w_x, y, w_x, y + h);
    SDL_RenderDrawLineF(rend, x, h - w_y, x + w, h - w_y);
}


void print_help(void) {
    printf(
        "┌───────────────────────────────────────────────────────────────┐\n"
        "│                 CHART PLOTTING PROGRAM                        │\n"
        "├───────────────────────────────────────────────────────────────┤\n"
        "│  Program takes input from stdin and plots data in window.     │\n"
        "│                                                               │\n"
        "│  Usage:                                                       │\n"  
        "│    [some signal cmd] | chart [options]                        │\n"
        "│    chart -f [some file] [options]                             │\n"
        "│                                                               │\n"
        "│  Options:                                                     │\n"
        "│    -h print this help                                         │\n"
        "│    -c [color] change color                                    │\n"
        "│        red, green, blue, yellow, magenta, cyan, white, orange,│\n"
        "│        gray                                                   │\n"
        "│    -l [int] length of the buffer                              │\n"
        "│    -o [bool] repeat output to stdout                          │\n"
        "│    -T [double] read wait time [s]                             │\n"
        "│    -f [filename] read input from file                         │\n"
        "│                                                               │\n"
        "└───────────────────────────────────────────────────────────────┘\n"
    );
}
