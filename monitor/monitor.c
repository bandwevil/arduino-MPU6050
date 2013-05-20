#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <ctype.h>
#include <unistd.h>

#define ARR_SIZE 5
#define ARDUINO_LOCATION "/dev/ttyACM0"

void outputData(int* position, int in, double data);

int main()
{
   FILE* file;
   int in;
   int posData[ARR_SIZE] = {'x', 'y', 'z'};
   unsigned char upper, lower;
   int16_t data;

   file = fopen(ARDUINO_LOCATION, "r");

   if (file == NULL) {
      fprintf(stderr, "Could not open %s for reading\n", ARDUINO_LOCATION);
      exit(EXIT_FAILURE);
   }

   //Read success/failure from device
   while (fgetc(file) != 'r') {
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
   curs_set(0);
   noecho();
   cbreak();

   while ((in = fgetc(file)) != EOF) {
      if (isalpha(in)) {
         upper = fgetc(file);
         lower = fgetc(file);
         data = (upper << 8) | lower;
         outputData(posData, in, ((double)data)/16384);
      }
      if (getch() == 'q') {
         break;
      }
   }

   fclose(file);
   return EXIT_SUCCESS;
}

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

   if (data < 0) {
      mvprintw(i, 0, "%c: %fg ", in, data);
   } else {
      mvprintw(i, 0, "%c:  %fg", in, data);
   }

   refresh();
}
