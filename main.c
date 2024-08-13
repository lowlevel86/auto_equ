#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
int divide(int a, int b);
int unused(int a, int b);
int (*operations[])(int, int) = {multiply, add, unused, subtract, unused, divide};

int unused(int a, int b)
{
   return 0;
}

int add(int a, int b)
{
   return a + b;
}

int subtract(int a, int b)
{
   return a - b;
}

int multiply(int a, int b)
{
   return a * b;
}

int divide(int a, int b)
{
   if (b)
   return a / b;
   
   return 0; // ignore incorrect operation | check later
}

int exeEquation(char *equ, int size, int *vals)
{
   int i;
   
   for (i=0; i < size-4; i+=6)
   vals[equ[i+4]-65] = operations[equ[i+1]-42](vals[equ[i]-65], vals[equ[i+2]-65]);
   
   return operations[equ[i+1]-42](vals[equ[i]-65], vals[equ[i+2]-65]);
}

void genEquation(char *equ, int size, int valCnt)
{
   int i, emptyValLoc;
   int A, B, op, extra;
   
   extra = 0;
   emptyValLoc = valCnt;
   for (i=0; i < size-4; i+=6)
   {
      A = (rand() % (valCnt+extra));
      B = (rand() % (valCnt+extra));
      op = (rand() % 4);
      
      equ[i] = 65 + A;
      equ[i+1] = 42 + ((op > 1) ? op+(op-1) : op);
      equ[i+2] = 65 + B;
      equ[i+4] = 65 + emptyValLoc;
      emptyValLoc++;
      extra++;
   }

   A = (rand() % (valCnt+extra));
   B = (rand() % (valCnt+extra));
   op = (rand() % 4);
   
   equ[i] = 65 + A;
   equ[i+1] = 42 + ((op > 1) ? op+(op-1) : op);
   equ[i+2] = 65 + B;
}

int *get_value_list(char *filename, int opCnt, char **equ, int *equ_size, int *valueCnt, int *list_size, int *var_space_size)
{
   char ch;
   int cnt;
   char *token;
   char line[1000];
   static int *vars_array;
   static char eq[] = "_-_=_;_-_=_;_-_=_;_-_=_;_-_=_;_-_=_;_-_=_;_-_=_;\
_-_=_;_-_=_;_-_=_;_-_=_;_-_=_;_-_=_;_-_=_;_-_=_;\
_-_=_;_-_=_;_-_=_;_-_=_;_-_=_;_-_=_;_-_=_;_-_=_;\
_-_=_;_-_=_;";
   eq[6*opCnt-3] = 0;
   *equ_size = strlen(eq);
   *equ = eq;
   
   
   // read in values list
   FILE *fp = fopen(filename, "r");if (fp == NULL) {printf("Error opening file.\n");exit(0);}
   
   // get the value count
   *valueCnt = 0;
   while ((ch = fgetc(fp)) != '\n')
   if (ch == ',')
   (*valueCnt)++;
   
   // get the list size
   *list_size = 1;
   while ((ch = fgetc(fp)) != EOF)
   if (ch == '\n')
   (*list_size)++;
   
   // variable space size
   *var_space_size = *valueCnt + opCnt; // space for variables and to perform operations
   
   
   // allocate memory for value list
   vars_array = (int *)malloc(*list_size * *var_space_size * sizeof(int *));
   
   rewind(fp);
   
   cnt = 0;
   while (fgets(line, 1000, fp))
   {
      token = strtok(line, ",\n"); // split the line into tokens
      
      while (token != NULL)
      {
         vars_array[cnt++] = atoi(token);
         token = strtok(NULL, ",\n");
      }
      
      vars_array[cnt-1+opCnt-1] = vars_array[cnt-1]; // copy result value to end of the variable space
      vars_array[cnt-1] = 0;
      cnt += opCnt-1;
   }
   
   fclose(fp);
   
   return vars_array;
}

void free_value_list(int *vars_array)
{
   if (vars_array)
   free(vars_array);
}

int main()
{
   #define OPERATION_CNT 9
   int i;
   char *equ;
   int equ_size;
   int valueCnt;
   int list_size;
   int *vars_array;
   int var_space_size;
   
   unsigned int t, t_prior;
   int vars_index;
   int result_index;
   int matchCnt;
   int matchCnt_prior = 0;
   
   // the values in each line of valList.txt are put into generated equations to find a result
   // last value in each line of valList.txt is the result
   vars_array = get_value_list("valList.txt", OPERATION_CNT, &equ, &equ_size, &valueCnt, &list_size, &var_space_size);
   
   // check for keypress
   struct termios old_tio, new_tio; // save current terminal settings
   int flags;
   tcgetattr(STDIN_FILENO, &old_tio);
   flags = fcntl(STDIN_FILENO, F_GETFL, 0); // set the terminal to non-blocking mode
   fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
   new_tio = old_tio; // set the terminal to raw mode
   new_tio.c_lflag &= ~(ICANON | ECHO);
   tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
   
   t = time(NULL);
   printf("time seed:%u\n", t);
   srand(t);

   while (getchar() == EOF) // exit after keypress
   {
      genEquation(equ, equ_size, valueCnt);
      
      matchCnt = 0;
      for (i=0; i < list_size; i++)
      {
         vars_index = i * var_space_size;
         result_index = vars_index + var_space_size-1;
         
         if (exeEquation(equ, equ_size, &vars_array[vars_index]) == vars_array[result_index])
         matchCnt++;
      }
      
      // print matching equation
      if (matchCnt >= matchCnt_prior)
      {
         printf("%s %i out of %i\n", equ, matchCnt, list_size);
         matchCnt_prior = matchCnt;
         t_prior = t;
      }
      
      // new random seed after so long
      t = time(NULL);
      if (t - t_prior >= 10)
      {
         //printf("time seed:%u\n", t);
         srand(t); // new seed
         t_prior = t;
      }
   }
   
   // restore terminal settings
   tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
   fcntl(STDIN_FILENO, F_SETFL, flags);

   free_value_list(vars_array);
}
