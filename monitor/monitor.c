#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <ctype.h>
#include <unistd.h>

#define ARR_SIZE 10
#define ARDUINO_LOCATION "/dev/ttyACM0"

void outputData(int* position, int in, double data);
void nextAccelRange();
void nextGyroRange();
void togglePower();

int accelSensitivity = 16384;
double gyroSensitivity = 131;
char powerState = 0;

int main()
{
   FILE* file;
   int in;
   int posData[ARR_SIZE] = {0, 'x', 'y', 'z', 0, 0, 'r', 'p', 'Y'};
   unsigned char upper, lower;
   int16_t data;

   file = fopen(ARDUINO_LOCATION, "rw");

   if (file == NULL) {
      fprintf(stderr, "Could not open %s for reading\n", ARDUINO_LOCATION);
      exit(EXIT_FAILURE);
   }

   sleep(1); //Wait for transmission to end
   fflush(file); //Clear any pending data from before reboot

   //Read success/failure from device
   while (fgetc(file) != 'R') {
   } //Scan through current input for the reset message
   if (fgetc(file) == 'F') {
      while (fgetc(file) != '\n') {
      } //Get through success message before continuing
   } else {
      fprintf(stderr, "Device did not initialize properly, got message:\n");
      while ((in = fgetc(file)) != 0) {
         fputc(in, stderr);
      }
      fclose(file);
      exit(EXIT_FAILURE);
   }

   //initialize ncurses
   initscr();
   curs_set(0);//No cursor
   noecho();   //Don't display user input
   cbreak();   //Don't wait for return to get input
   timeout(0); //input is non-blocking

   attron(A_UNDERLINE); //Print titles with underlines
   mvprintw(0, 0, "Acceleration");
   mvprintw(5, 0, "Rotation");
   mvprintw(0, 25, "Controls");
   attroff(A_UNDERLINE);

   //Print controls and their initial settings
   mvprintw(1, 25, "q: quit");
   mvprintw(2, 25, "a: accelerometer range ± 2g");
   mvprintw(3, 25, "g: gyroscope range     ± 250°/s");
   mvprintw(4, 25, "p: standard power");

   while ((in = fgetc(file)) != EOF) {
      if (isalpha(in)) { //Make sure we don't read the middle of a transmission
         upper = fgetc(file);
         lower = fgetc(file);
         data = (upper << 8) | lower;
         outputData(posData, in, ((double)data));
      }

      switch (getch()) { //Check input for controls, act as necessary
         case 'q': //Quit out of ncurses
            endwin();
            fclose(file);
            exit(EXIT_SUCCESS);
            break;
         case 'a': //Next accelerometer range setting
            fputc('a', file);
            nextAccelRange();
            break;
         case 'g': //Next gyroscope range setting
            fputc('g', file);
            nextGyroRange();
            break;
         case 't': //Run self test (not easily visible in display)
            fputc('t', file);
            accelSensitivity = 4096;
            mvprintw(2, 49, " 8g");
            gyroSensitivity = 131;
            mvprintw(3, 49, " 250°/s");
            break;
         case 'p':
            fputc('p', file);
            togglePower();
            break;
      }

      refresh();
   }

   fclose(file);
   endwin();
   return EXIT_SUCCESS;
}

/*
 * Toggle between low and standard power.
 */
void togglePower()
{
   if (powerState == 0) {
      mvprintw(4, 25, "p: low power     ");
      powerState = 1;
   } else {
      mvprintw(4, 25, "p: standard power");
      powerState = 0;
   }
}

/*
 * Switch between accelerometer range settings and modify the display as necessary
 */
void nextAccelRange()
{
   switch(accelSensitivity) {
      case 16384:
         accelSensitivity = 8192;
         mvprintw(2, 49, " 4g");
         break;
      case 8192:
         accelSensitivity = 4096;
         mvprintw(2, 49, " 8g");
         break;
      case 4096:
         accelSensitivity = 2048;
         mvprintw(2, 49, "16g");
         break;
      case 2048:
      default:
         accelSensitivity = 16384;
         mvprintw(2, 49, " 2g");
         break;
   }
}

/*
 * Switch between gyro range settings and modify the display as necessary
 */
void nextGyroRange()
{
   switch((int)gyroSensitivity) {
      case 131:
         gyroSensitivity = 65.5;
         mvprintw(3, 49, " 500°/s");
         break;
      case 65:
         gyroSensitivity = 32.8;
         mvprintw(3, 49, "1000°/s");
         break;
      case 32:
         gyroSensitivity = 16.4;
         mvprintw(3, 49, "2000°/s");
         break;
      case 16:
      default:
         gyroSensitivity = 131;
         mvprintw(3, 49, " 250°/s");
         break;
   }
}

/*
 * Take a 16 bit word and display it onscreen according to the message identifier
 * in and its location in the position array. If in is not in the position
 * array, nothing is changed
 */
void outputData(int* position, int in, double data)
{
   int i;

   for (i = 0; i < ARR_SIZE; i++) {
      if (position[i] == in) {
         break;
      }
   }

   if (i == ARR_SIZE) {
      return;
   }

   if (in == 'x' || in == 'y' || in == 'z') {
      mvprintw(i, 0, "%c: %7.3f g", in, data/accelSensitivity);
   } else {

      mvprintw(i, 0, "%c: %8.2f °/s", in, data/gyroSensitivity);
   }
}
