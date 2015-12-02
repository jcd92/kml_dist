/* A simple XML parser */
#include <stdio.h>
#include <strings.h>
#include <ctype.h>


struct character {
  char cargo;
  char type[8];
};

struct token {
  char cargo[256];
  char type[8];
};

struct character scanner(FILE *source);
struct token tokenizer(FILE *source);

main (int argc, char *argv[])
{
  FILE *input;
  struct token t;
  if ((input = fopen(argv[1], "r")) == NULL)
    printf("File could not be opened\n");
  else {
    while (1) {
    t = tokenizer(input);
    if (strcmp(t.type, "EOF") == 0) {
      printf("End of file\n");
      return 0;
    }  else {
      printf("Token : %8s %s\n", t.type, t.cargo);
    }
    }
  }
}

struct character scanner(FILE *source) 
{
  struct character c;
  if ((c.cargo = fgetc(source)) == EOF) {
    strncpy(c.type, "EOF", 8);
    //printf("scanner: EOF\n");
  } else { 
    strncpy(c.type, "ASCII", 8);
//    *c.type = "ASCII";
  }
  return c;
}

struct token tokenizer(FILE *source)
{
  struct token t;
  struct character c;
  int n;
  
  while (1) {
    c = scanner(source);
    //printf("TOK: c.type %s\n", c.type);
    if (strcmp(c.type, "EOF") == 0) {
      //printf("Tokeniser: EOF\n");
      strncpy(t.type, "EOF", 8);
      return t;
    } else {
      //printf("Character : %c\n", c.cargo);

      while (c.cargo == ' ' || c.cargo == '\t' || c.cargo =='\n') {
        //printf("Whitespace : %c\n", c.cargo);
        c = scanner(source);    
      }
      //printf("Non w/s character: %c\n", c.cargo);

      if (c.cargo == '<') {
        n = 0;
        c = scanner(source); // Check if this is a node start or a node end.
        if (c.cargo == '/'){
          strncpy(t.type, "End", 8);      
        } else {
          strncpy(t.type, "Start", 8);
          t.cargo[n] = c.cargo;
          n++;      
        }
        while (c.cargo != '>') {
          c = scanner(source);    /*Keep scanning characters until we hit the end of the token*/
          //printf("Character in token : %c\n", c.cargo);
          t.cargo[n] = c.cargo;
          n++;
        }
        t.cargo[n-1] = '\0';  // Overwrite the > character with the array terminator.
        //printf("End token\n");
        return t;
      } else if (isdigit(c.cargo) || c.cargo == '-') {
        n = 0;
        strncpy(t.type, "Number", 8);
        //printf("Number\n");
        while (isdigit(c.cargo) || c.cargo =='-' || c.cargo =='.') {
          //printf("Digit in number : %c\n", c.cargo);
          t.cargo[n] = c.cargo;
          n++;
          c = scanner(source);    /*Keep scanning characters until we hit a non text */
        }
        ungetc(c.cargo, source);  // Push the next character back for processing next time
        t.cargo[n] = '\0';  // Overwrite the > character with the array terminator.        
        return t;
      } else if (isprint(c.cargo)){
        n = 0;
        strncpy(t.type, "Text", 8);
        //printf("Text\n");
        //t.cargo[0] = c.cargo;
        while (c.cargo !='<') {
          //printf("Character in text : %c\n", c.cargo);
          t.cargo[n] = c.cargo;
          n++;
          c = scanner(source);    /*Keep scanning characters until we hit a non text */
        }
        ungetc(c.cargo, source);  // Push the next character back for processing next time
        t.cargo[n] = '\0';  // Overwrite the > character with the array terminator.        
        return t;
      } else {
        printf("Illegal token start character %c\n", c.cargo);
      }
    }
  }
}

    