#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*========================================================
 * Функция перемещает файл или каталог NameFile в 
 * каталог NameDir. Заполняется новая строка вида
 * "NameDir/NameFile" и вызывается функция rename
 *========================================================
*/
int ToDir(char * NameFile, char * NameDir)
  {
    char * NameNew;
    int size=0,i;
    int LenFile = strlen(NameFile);
    int LenDir = strlen(NameDir);
    
    /*printf("filenew=%s, fileold=%s\n", NameFile, NameDir);*/
    size = LenFile + LenDir + 2;
    if ((NameNew = malloc(size*sizeof(NameNew))) == NULL)
      return -1;
    for (i=0;i<LenDir;i++)
      NameNew[i] = NameDir[i];
    NameNew[i++] = '/';
    for (i=0;i<LenFile;i++)
      NameNew[i+LenDir+1] = NameFile[i];
    NameNew[i+LenDir+1] = '\0';
    
    if ( rename( NameFile, NameNew ) != 0 )
      {
        free(NameNew);
        return -1;
      }
    /*printf("nfile=%s, size=%d\n", NameNew, size);*/
    free(NameNew);
    return 0;
  }
 
int main(int argc, char ** argv)
  {
    char * OutF;
    char * InpF;
    struct stat StOut;
    struct stat StInp;
    int i;
    
    if (argc < 3)
      {
        fprintf(stderr, "получено < 2 параметров\n");
        return -1;
      }
    else if (argc == 3)
      {
        OutF = argv[2];
        InpF = argv[1];
        
        if (stat(OutF, &StOut) != -1 && S_ISDIR(StOut.st_mode))
          {
            fprintf(stderr, "перемещение\n");
            
            if ( stat(InpF, &StInp) == -1)
              {
                fprintf(stderr, "файл или каталог %s должен существовать\n", InpF);
                return -1;
              }
            if (ToDir(InpF, OutF) != 0)
              {
                fprintf(stderr, "не удалось переместить файл или каталог %s\n", InpF);
                return -1;
              }
          }
        else
          {
            fprintf(stderr, "смена\n");
            
            if ( stat(InpF, &StInp) == -1)
              {
                fprintf(stderr, "файл или каталог %s должен существовать\n", InpF);
                return -1;
              }
            if (S_ISREG(StInp.st_mode) == 0)
              {
                fprintf(stderr, "%s должен быть файлом\n", InpF);
                return -1;
              }
            if ( rename(InpF, OutF) != 0 )
              {
                fprintf(stderr, "не удалось переместить файл или каталог %s\n", InpF);
                return -1;
              }
          }
      }
    else
      {
        OutF = argv[argc-1];
        
        if ( stat(OutF, &StOut) == -1)
          {
            fprintf(stderr, "файл или каталог %s должен существовать\n", OutF);
            return -1;
          }
        if (S_ISDIR(StOut.st_mode) == 0)
          {
            fprintf(stderr, "%s должен быть каталогом\n", OutF);
            return -1;
          }
        
        for (i=1; i<argc - 1; i++)
          {
            InpF = argv[i];
            /*printf("%s\n", NewF);*/
            if ( stat(InpF, &StInp) == -1)
              {
                fprintf(stderr, "файл или каталог %s должен существовать\n", InpF);
                return -1;
              }
            if (ToDir(InpF, OutF) != 0)
              {
                fprintf(stderr, "не удалось переместить файл или каталог %s\n", InpF);
                return -1;
              }
          }
      }
      
    return 0;
  }
