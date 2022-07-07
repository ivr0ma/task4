#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*=================================================================
 * Функция определяет индексы файла и строки (IndFile, IndStr)
 *=================================================================
*/
int GetInd(char ** argv, int *IndFile, int *IndStr)
  {
    if (strcmp(argv[1],"-v") == 0)
    {
      *IndFile=3;
      *IndStr=2;
      return 0;
    }
    else if (strcmp(argv[2],"-v") == 0)
    {
      *IndFile=3;
      *IndStr=1;
      return 0;
    }
    else if (strcmp(argv[3],"-v") != 0)
      return -1;
  }

int main(int argc, char ** argv)
{
  char *buf = NULL;
  char c = 0;
  int i=0;
  FILE *fp;
  
  int IndFile=2, IndStr=1;
  int flag = 0;
  
  if ((argc < 3) || (argc > 4))
  {
    fprintf(stderr, "не совпадает число параметров\n");
    return -1;
  }
  
  if (argc == 4)
  {
    flag = 1;
    if (GetInd(argv, &IndFile, &IndStr) == -1)
      {
        fprintf(stderr, "не совпадает число параметров\n");
        return -1;
      }
  }
  
  
  if ((fp = fopen(argv[IndFile], "r")) == NULL)
  {
    fprintf(stderr, "не могу открыть файл\n");
    return -1;
  }
  
  while((c = getc(fp)) != EOF) 
  {
    if (c == '\n')
    {
      if ((buf = (char*)realloc(buf, (i + 1) * sizeof(char))) == NULL)
        {
          fprintf(stderr, "error");
          return;
        }
      buf[i++] = '\0';
      
      if (((strstr(buf, argv[IndStr]) != NULL) && !flag) || ((strstr(buf, argv[IndStr]) == NULL) && flag))
        printf("%s\n", buf);
      
      i = 0;
      free(buf);
      buf = NULL;
    }
    else
    {
      buf = realloc(buf, (i+1) * sizeof(char));
      buf[i++] = c;
    }
  }
  
  fseek(fp, -1, SEEK_CUR);
    
  if (getc(fp) != '\n')
  {
    if ((buf = (char*)realloc(buf, (i + 1) * sizeof(char))) == NULL)
      {
        fprintf(stderr, "error");
        return;
      }
    buf[i++] = '\0';
    if (((strstr(buf, argv[IndStr]) != NULL) && !flag) || ((strstr(buf, argv[IndStr]) == NULL) && flag))
        printf("%s\n", buf);
    free(buf);
    buf = NULL;
  }

  fclose(fp);
  return 0;
}
