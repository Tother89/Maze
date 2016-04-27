#include <Arduino.h>
#include "project.h"



extern int* option;
extern hor_wall hor_lines[128];
extern ver_wall ver_lines[160];
extern int num_blocks;
int iVer = 0;
int iHor = 0;
void setup()
{
    Serial.begin(9600);
    //Set up our random number generator
    randomSeed(analogRead(3));
    pinMode(JOYSTICK_SEL,INPUT_PULLUP);
    //Set the initial resting values of our joystick
    iVer = analogRead(JOYSTICK_VER);
    iHor = analogRead(JOYSTICK_HOR);
    //Initiate the screen
    tft.initR(INITR_BLACKTAB); 
    //Variable to toggle between menu and playing
    int answer =0;

    //In this do-while loop we'll continually run our 
    //program
        do{
            //Display the title screen
            tft.fillScreen(ST7735_BLACK);
            int input = 0;
            display_menu();
    
            while(!input){
                //Get user input for level difficulty
                display_cursor(option,iVer);
                input = user_input(option);
                delay(100);
            }
            //Set the number of blocks we will iterate through
            //when drawing the maze chosen by player
            num_blocks = difficulty(input);

            //Draw the maze and move your cursor with the joystick
            //Do this until player chooses a new level
            do{
                build_grid(hor_lines,ver_lines);
                draw_finish();
                //Keep using joystick until button is pressed
                while(digitalRead(JOYSTICK_SEL)){
                    //Returning a 3 means we've reach the end of a game
                    answer = move_cursor(analogRead(JOYSTICK_VER),
                                analogRead(JOYSTICK_HOR),iVer,iHor);
                    if(answer==3){
                        //Show the "Vicotry!" screen for a bit
                        //then display our pause menu
                        delay(4000);
                        break;
                    }
                }
                delay(100);
                //If button is pressed give user the option to quit
                //or continue playing with a pause menu
                answer = pause_menu(iVer);
                //Reset the cursor
                hit_edge();
                delay(100);
                //Answer 2 == play again
            }while(answer==2);
            //Answer 1 == New level
        }while(answer==1);


}


void loop()
{
    
    
}
                
