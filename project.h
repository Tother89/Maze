#ifndef PROJECT_H
#define PROJECT_H
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>
#include <lcd_image.h> //Noted to have this referenced last



// standard U of A library settings, assuming Atmel Mega SPI pins
#define SD_CS    5  // Chip select line for SD card
#define TFT_CS   6  // Chip select line for TFT display
#define TFT_DC   7  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)

// Joystick pins:
#define JOYSTICK_HOR 0   // Analog input A0 - horizontal
#define JOYSTICK_VER 1   // Analog input A1 - vertical
#define JOYSTICK_SEL 9  // Digital input pin 9 for the button

extern Adafruit_ST7735 tft;

typedef struct {
    uint16_t start;
    uint16_t end;
    uint16_t y_pos;
} hor_wall;

typedef struct {
    uint16_t start;
    uint16_t end;
    uint16_t x_pos;
} ver_wall;
int RNG();
void hit_edge();
int move_cursor(int, int, int,int);
int hit_wall(int cursor_x, int cursor_y);
void build_grid(hor_wall*,ver_wall*);
void display_menu();
int difficulty(int);
int user_input(int*);
void display_cursor(int*, int);
void display_box(int);
void draw_finish();
int end_game();
int pause_menu(int);
#endif
