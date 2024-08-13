#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
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


char *get_equ_list(char *filename, int *opCnt, int *equ_list_size)
{
   int i;
   char ch;
   int file_size;
   struct stat file_stats;
   static char *equ_array = 0;
   
   // read in values list
   FILE *fp = fopen(filename, "r");if (fp == NULL) {printf("Error opening file.\n");exit(0);}
   
   // get the value count
   *opCnt = 1;
   while ((ch = fgetc(fp)) != '\n')
   if (ch == ';')
   (*opCnt)++;
   
   // get the list size
   *equ_list_size = 1;
   while ((ch = fgetc(fp)) != EOF)
   if (ch == '\n')
   (*equ_list_size)++;
   
   rewind(fp);
   
   // Get the file size
   if (fstat(fileno(fp), &file_stats) == -1) {printf("Error getting file statistics\n");fclose(fp);exit(0);}
   file_size = file_stats.st_size;
   
   equ_array = (char *)malloc(file_size + 1);
   if (equ_array == NULL) {printf("Error allocating memory\n");fclose(fp);exit(0);}
   
   fread(equ_array, 1, file_size, fp);
   
   for (i=0; i < file_size; i++)
   if (equ_array[i] == '\n')
   equ_array[i] = 0;
   
   return equ_array;
}

void free_equ_list(char *equ_array)
{
   if (equ_array)
   free(equ_array);
}

void free_value_list(int *vars_array)
{
   if (vars_array)
   free(vars_array);
}

int main()
{
   int i, j;
   char *equ;
   int equ_size;
   int valueCnt;
   int list_size;
   int *vars_array;
   int var_space_size;
   
   int vars_index;
   int result_index;
   
   char *equ_array;
   int opCnt;
   int equ_list_size;
   
   equ_array = get_equ_list("equList.txt", &opCnt, &equ_list_size);
   
   vars_array = get_value_list("valList.txt", opCnt, &equ, &equ_size, &valueCnt, &list_size, &var_space_size);
   
   for (j=0; j < (equ_size+1)*equ_list_size; j+=equ_size+1)
   {
      equ = &equ_array[j];
      
      for (i=0; i < list_size; i++)
      {
         vars_index = i * var_space_size;
         result_index = vars_index + var_space_size-1;
         
         if (exeEquation(equ, equ_size, &vars_array[vars_index]) == vars_array[result_index])
         printf("%i,", i);
      }
      printf("\n");
   }
   
   free_value_list(vars_array);
   free_equ_list(equ_array);
}
