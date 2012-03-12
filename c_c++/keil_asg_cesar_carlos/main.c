#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <reg51.h>      /*Used for serial interface*/

#define MAXLINE 72
#define PROMPT  prtline("> ")

typedef enum{TRUE=1, FALSE=0} bool;


//
//Gets a line from the user. Includes a newline.
//
void getline (char *line)  {
   while ((*line++ = getchar()) != '\n');
}

//
//Prints a line of output in a given char *line
//
void prtline (char *line) {
   while (putchar(*line++));   
}

//
//Reverses the characters in a char *string.
//The beginning position and ending position
//must be passed in to rev_word, of a 
//char *string.
//
void rev_word( char *beg_pos, char *end_pos){
   char *start = beg_pos;
   char *end = end_pos;
   char temp;

   while (start < end){
      temp = *start;
      *start = *end;
      *end = temp;
      ++start;
      --end;
   }
}

//
//Sorts the characters in a word in 
//alphabetical order. The beginning position and ending
//position must be passed in to alph_sort_w, of a
//char *string.
//
void alph_sort_w( char *beg_pos, char *end_pos) {
   char *start = beg_pos;
   char *end = end_pos;
   char *itor = start;
   char temp;
   
   //According to the assignment, we will not sort
   //the letter in lexicographic order. So we use
   //the tolower(3) comparison to swap elements
   for (; start != end; ++start)
      for (itor = start; itor <= end; ++itor) 
         if( tolower(*start) > tolower(*itor) ) {
            temp = *start;
            *start = *itor;
            *itor = temp;
         }
}

//
//Finds words in the char *string. When a word is found,
//alph_sort_words() will keep track of the starting and ending
//position. We apply alph_sort_w(), which sorts a word alphabetically.
//
void alph_sort_words(char *str) {
   char *itor = str;
   char *start;
   bool start_set = FALSE;  /*Has char *start been set?*/
   bool prev = FALSE;
   
   //Loop entire char *str, until we find the null plug.
   //When a word is found apply alph_sort_w()
   while (*itor ) {
      /*New word found*/
      if (!isspace(*itor)) {
         prev = TRUE;
         if(start_set == FALSE){
            start = itor;
            start_set = TRUE;
         }
         ++itor;
      /*Skip whitespace */
      }else if (isspace(*itor) && prev == FALSE) {
         ++itor;
         continue;
      /*We are at the end of a word. Apply operation*/
      }else if (isspace(*itor) && prev == TRUE) {
         alph_sort_w(start, --itor);
         ++itor;
         prev = FALSE;
         start_set = FALSE;
      }
   }
}

//
//Finds words in the char *string. When a word is found,
//reverse_words() will keep track of the starting and ending
//position. We apply rev_word(), which
//reverses a word
//
void reverse_words(char *str) {
   char *itor = str;
   char *start;
   bool start_set = FALSE;   /*Has char *start been set*/
   bool prev = FALSE;
   
   //Loop entire char *str, until we find the null plug.
   //When a word is found apply rev_word()
   while (*itor ) {
      /*New word found*/
      if (!isspace(*itor)) {
         prev = TRUE;
         if(start_set == FALSE){
            start = itor;
            start_set = TRUE;
         }
         ++itor;
      /*Skip whitespace */
      }else if (isspace(*itor) && prev == FALSE) {
         ++itor;
         continue;
      /*We are at the end of a word. Apply operation*/
      }else if (isspace(*itor) && prev == TRUE) {
         rev_word(start, --itor);
         ++itor;
         prev = FALSE;
         start_set = FALSE;
      }
   }
}

void main (void) {
   pdata char buff[MAXLINE];

                      /*Required for Serial Interface*/
   SCON = 0x52;       /*Serial port configuration*/
   TMOD = 0x20;
   TCON = 0x40;
   TH1  = 0xf3;       /*2403 baudrate @12mhz*/

   while(TRUE){
      memset(buff, 0, sizeof(buff));
      prtline("Enter Line: ");
      getline(buff);
      PROMPT;
      puts(buff);

      /*Reverse the characters in the line*/
      /*Reverse the words in the line*/
      rev_word(buff, buff + strlen(buff) - 2); 
      reverse_words(buff); 
      PROMPT;
      puts(buff);

      /*Convert the line to its original form*/
      rev_word(buff, buff + strlen(buff) - 2);
      reverse_words(buff);
   
      /*Sort the characters in the words in alphabetical order*/
      alph_sort_words(buff);
      PROMPT;
      puts(buff);
   }
}
