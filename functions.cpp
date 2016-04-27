#include <Arduino.h>
#include "project.h"


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
int srow = 1, scol = 1;
int* option;
hor_wall hor_lines[128];
ver_wall ver_lines[160];
int num_blocks;
uint16_t num_hor = 0; 
uint16_t num_ver = 0;
int x = 5;
/*
  Similar to my other joystick movement functions
  Takes in the initial vertical and horizontal position
  as references for the joystick movement
  It also keeps track of the vertical and horizontal
  position by incrementing or decrementing whenever the 
  joystick is moved accordingly
*/
int move_cursor(int ver, int hor, int iVer, int iHor){
    //The integer x will define my cursor size
    
    //These guys are to keep my cursor from going over
    //the edge of the screen
    int bot_wall=160, right_wall=128;
    //Redraw or just draw the old spot my cursor passed over
    tft.fillRect(scol, srow, x, x, ST7735_BLACK);    
    const int deadZone = 20;

    if(ver < iVer - deadZone) {
        //Go down a row when joystick is less than
        //initial position due to orientation of joystick
        //to LCD screen
        srow = srow-1;
        if( hit_wall(srow,scol) || srow==0) {
            //Once we hit an edge we have failed and 
            //must go back to the start;
            hit_edge();
        }
    }
    if(ver > iVer + deadZone) {
        srow= srow+1;
        if(hit_wall( srow,scol) || srow==bot_wall-x) {
            hit_edge();
        }
    }
    if(hor < iHor - deadZone) {
        scol = scol-1;
        if(hit_wall(srow,scol) || scol==0) {
            hit_edge();
        }
    }
    if(hor > iHor + deadZone) {
        scol = scol+1;
        if(hit_wall(srow,scol) || scol==right_wall-x) {
            hit_edge();
        }
    }
    //Once movement is done internally redraw the 
    //cursor in its new position
    tft.fillRect(scol, srow,x,x, ST7735_GREEN);
    delay(40);//Wait so as to not move too quickly
    //Check to see if player got to the end
    if((scol > 65 && scol < 72) && (srow > 78 && srow < 88))
       return end_game();
    return 0;
}
/*
  Randomly generates a number using our randomSeed
  initialization from the setup
*/
int RNG(){
    //We only need values that are either 0 or 1
    //Must be as random as possible or else
    //We will see patterns in the maze design
    int r = random(2) %2;
    return r;
}
/*
  Send the cursor back to the start
*/
void hit_edge(){
    srow = 1;
    scol = 1;
}
/*This function returns 1 when a wall has been hit
  and 0 when its not hitting a wall
  1) Check if the cursor is overlapping a wall
  2) If it is return 1
  3) Else return 0
*/
int hit_wall(int cursor_y, int cursor_x){
   
    int x = tft.width()/num_blocks;
    int y = tft.height()/num_blocks;
    int x_edge = cursor_x + 5;
    int y_edge = cursor_y + 5;

    for(int i=0; i<num_hor; i++){
        for(int j=0; j<num_ver; j++){
            //Here we want to return true if the cursor lies between the start
            //and end of the horizontal line and is on the correct y-axis position
            if((((cursor_x >= hor_lines[i].start) && (cursor_x <= hor_lines[i].end))
                && (cursor_y == hor_lines[i].y_pos))){
                return 1;
            }
            //Check for walls hitting the far x_edge of the cursor
            if((((x_edge >= hor_lines[i].start) && (x_edge <= hor_lines[i].end)) &&
                ((cursor_x >= hor_lines[i].start) && (cursor_x <= hor_lines[i].end))
                && (y_edge >= hor_lines[i].y_pos && hor_lines[i].y_pos >= cursor_y))){
                
                return 1;
            }
            //check the same as before but this time for vertical lines
            if(((cursor_y   >= ver_lines[j].start) && (cursor_y <= ver_lines[j].end))
               && (cursor_x == ver_lines[j].x_pos)) {
                return 1;
            }
            //Check for walls hitting the far y_edge of the cursor
            if((y_edge   >= ver_lines[j].start) && (y_edge <= ver_lines[j].end) &&
               ((cursor_y   >= ver_lines[j].start) && (cursor_y <= ver_lines[j].end))
               && (x_edge >= ver_lines[j].x_pos && ver_lines[j].x_pos >= cursor_x)) {
               
                return 1;
            }
        }
    }
    //Return  0 when cursor is touching nothing
    return 0;

}
/*
  Draw out our grid lines
  When drawing them decide to leave up either
  the East wall or the South wall
  Then iterate through each "block" 
  choosing the same thing
*/
void build_grid(hor_wall* hor_lines, ver_wall* ver_lines){
    
    uint16_t x = tft.width()/num_blocks;
    uint16_t y = tft.height()/num_blocks;
    uint16_t color = ST7735_RED;
    num_hor = 0;
    num_ver = 0;
    //Build the outer border 
    tft.fillScreen(ST7735_BLACK);
    tft.drawLine(0,0,128,0,color);
    tft.drawLine(0,0,0,160,color);
    tft.drawLine(0,159,127,159,color);
    tft.drawLine(127,0,127,159,color);
    
    //This is the main algorithm for randomly creating a maze
    //First generate a random number either 1 or 0
    //Next decide whether to draw the south wall
    //or the east wall in that block
    //Afterwards move onto next block
    
    //These for loops iterate through only where we
    //want to draw a line
    for(uint16_t i=0; i<tft.width(); i+=x){
        for(uint16_t j=0; j<tft.height(); j+=y){
            int rand = RNG();            
                        
            //When the random number is 1 and not the first/last column
            //Draw the south line
            //and store its location
            if(rand && i!=0 && i!=tft.width() && i!=tft.width()-x){
                //South line
                //Draw a line from x start position i 
                //to x end position i+x along the y+j y-axis
                tft.drawLine(i,y+j,x+i,y+j, color);
                
                //Here is where we store the line locations in
                //our array
                hor_lines[num_hor].start = i;
                hor_lines[num_hor].end = x+i;
                hor_lines[num_hor].y_pos = y+j;
                
                num_hor++;
               
               
            }
            
            //When the random is 0 and not the first/last row
            //Draw the east line
            //and store its location
           
            if((!rand && (j != 0)) && (j!=tft.height()) && (j!=tft.height()-y)){
                //East line
                //Draw a line from y to y+j along the
                //x+i x-axis
                tft.drawLine(x+i,j,x+i,y+j, color);
                //Store the verticle lines
                ver_lines[num_ver].start = j;
                ver_lines[num_ver].end = y+j;
                ver_lines[num_ver].x_pos = x+i;
                num_ver++;
            }
        }
    }
}
/*
  Menu function which allows players to
  1. See the name of the game
  2. See the difficulty setting
  3. Selection a difficulty
*/
void display_menu(){
    //Give our player a warm welcome
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(38,5);
    tft.setTextColor(ST7735_BLUE);
    tft.setTextSize(1);
    tft.println("WELCOME\n");
    tft.setCursor(54,20);
    tft.println("TO \n");
    tft.setCursor(5,35);
    tft.setTextSize(2);
    tft.println("MAZ-O-RAMA");

    //Ask them to select a difficulty
    tft.setCursor(15,70);
    tft.setTextSize(0);
    tft.setTextColor(ST7735_BLUE);
    tft.print("Choose difficulty");

    //Then we can list our options
    //Green for easy, yellow for medium
    //and of course red for hard!
    tft.setCursor(50, 100);
    tft.setTextColor(ST7735_GREEN);
    tft.print("EASY");
    tft.setCursor(50, 120);
    tft.setTextColor(ST7735_YELLOW);
    tft.print("MEDIUM");
    tft.setCursor(50, 140);
    tft.setTextColor(ST7735_RED);
    tft.print("HARD");
}
/*
  These are our difficulty settings which 
  ultimately decide the number of blocks in our
  grid that will be build in our build_grid
  function
  Depending on user input we will select something
  that is divisible by both our screen width and height
*/
int difficulty(int input){

    int hard = 16, medium = 8, easy = 4;
    
    if(1==input)
        return easy;
    if(2==input)
        return medium;
    if(3==input)
        return hard;
   
}
/*
  Lets us know what difficulty level
  the player has selected
*/
int user_input(int* option){
    //Whichever value option is currently
    //set to will be the one we return
    if(option[0]==2)
        return 1;
    if(option[0]==1)
        return 2;
    if(option[0]==0)
        return 3;
    return 0;
}
/*
  Read our input from the joystick and decide
  whether to show the box over a certain option
  whenever you move the joystick go up or down
  depending on movement
  when its not being moved stay at current box
  Also change our "option" array to the currently
  selected difficulty
*/
void display_cursor(int* option, int iVer){
    int time = 220;
    uint16_t i = 1;
    //Serial.print("am i here");
    while(digitalRead(JOYSTICK_SEL)){
        //If joystick moves up then move the cursor up
        if(analogRead(JOYSTICK_VER) < iVer-50){
            if(i==2)
                i=0;
            else
                i++;
            option[0] = i;
            display_box(i);
            delay(time);

           
        }
        //If joystick is not moving keep present selection
        if(analogRead(JOYSTICK_VER) == iVer){
            display_box(i);
            option[0] = i;
            delay(time);
        }
        //If joystick read down, move the cursor down
        if(analogRead(JOYSTICK_VER) > iVer+50){
            if(i==0)
                i=2;
            else
                i--;
            option[0] = i;
            display_box(i);
            delay(time);
        }
    }
}
//Put a cursor around each option
//So the user will know which difficulty 
//he is selecting
void display_box(int option){
    
    //Mapped out the size of each box according to
    //the length of the word its covering
    int box_x = 28, box_y = 12;
    int med_x = 40, med_y = 12;
    int hard_x = 30, hard_y = 12;
    if(option==2){
        //Draw a box over the "easy" option
        tft.drawRect(48,98 , box_x, box_y, ST7735_BLUE);
        //Erase the rest and repeat this in the other steps
        tft.drawRect(48, 118, med_x, med_y, ST7735_BLACK);
        tft.drawRect(48, 138,hard_x ,hard_y , ST7735_BLACK);
    }
    if(option==1){
        //Draw a box over the "medium" option
        tft.drawRect(48, 118, med_x, med_y, ST7735_BLUE);
        tft.drawRect(48,98 , box_x, box_y, ST7735_BLACK);
        tft.drawRect(48, 138,hard_x ,hard_y , ST7735_BLACK);
    }
    if(option==0){
        //Draw a box over the "hard" option
        tft.drawRect(48, 138,hard_x ,hard_y , ST7735_BLUE);
        tft.drawRect(48,98 , box_x, box_y, ST7735_BLACK);
        tft.drawRect(48, 118, med_x, med_y, ST7735_BLACK);
    }
}
/*
  Draw an "F" for finish to
  mark where the player should
  try to navigate towards
*/
void draw_finish(){
    tft.setTextColor(ST7735_MAGENTA);
    tft.setTextSize(1);
    tft.setCursor(68,82);
    tft.print("F");
}
/*
  End the game once the player has arrived at the
  finish line 
*/
int end_game(){
    tft.fillScreen(ST7735_WHITE);
    tft.setCursor(22, 80);
    tft.setTextColor(ST7735_MAGENTA);
    tft.setTextSize(2);
    tft.print("YOU WIN!");
    return 3;
}

/*
  Give the user an option to restart the game
  and choose another difficulty level or continue
  playing the same level
  Note: the level will not be exactly the same since
  we cannot store the old maze. Once we're able to
  store it then we'll be able to replay it after
  going to this menu
*/
int pause_menu(int iVer){
    int result = 0;
    tft.fillScreen(ST7735_BLACK);
    tft.setTextSize(2);
    tft.setCursor(40,20);
    tft.print("PAUSE");
    tft.setTextSize(1);
    tft.setCursor(8,60);
    tft.print("Would you like to... ");
    tft.setCursor(8,100);
    tft.print("New level");
    tft.setCursor(8,120);
    tft.print("Play again");

    tft.drawRect(5,98,68,12,ST7735_BLUE);
    tft.drawRect(5,118,68,12,ST7735_BLACK);
    result = 1;
    while(digitalRead(JOYSTICK_SEL)){
       
        if(analogRead(JOYSTICK_VER) < iVer-50){
            tft.drawRect(5,98,68,12,ST7735_BLUE);
            tft.drawRect(5,118,68,12,ST7735_BLACK);
            result = 1;
        }
        if(analogRead(JOYSTICK_VER) > iVer+50){
            tft.drawRect(5,118,68,12,ST7735_BLUE);
            tft.drawRect(5,98,68,12,ST7735_BLACK);
            result = 2;
        }
        delay(200);

    }
    return result;
}
