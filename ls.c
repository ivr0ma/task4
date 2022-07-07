#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>

typedef enum 
    {
      flag_l,
      flag_g,
      flag_R
    } Tflags;

/*=================================================================================
 * Функция получает на вход массив флагов, argc, argv, возвращает слово, если
 * последовательность флагов записана правильно (т.е. она содержит только посл-ть
 * символов 'l', 'g', 'R'), иначе NULL. Меняет массив флагов.
 *=================================================================================
*/
char * GetFlags(int * flags, int argc, char ** argv)
  {
    char * str;
    int i;
    char c;
    
    for (i=1; i<argc; i++)
      {
        str = argv[i];
        
        if (*(str++) == '-')
          {
            if (*str == '\0')
              return "-z";
            while ((c = *(str++)) != '\0')
              {
                if (c == 'l')
                  flags[flag_l] = 1;
                else if (c == 'g')
                  flags[flag_g] = 1;
                else if (c == 'R')
                  flags[flag_R] = 1;
                else
                  {
                    flags[flag_l] = 0;
                    flags[flag_g] = 0;
                    flags[flag_R] = 0;
                    return NULL;
                  }
              }
            return argv[i];
          }
        
      }
      
    return "-z";
  }

/*===================================================================== 
 * показать тип файла в первой позиции выходной строки 
 *=====================================================================
*/
void display_file_type(int st_mode) 
  {                                   
    if (S_ISREG(st_mode))      /*обычный*/
      putchar ( '-' );
    if (S_ISDIR(st_mode))      /*каталог*/
      putchar ( 'd' );
    if (S_ISCHR(st_mode))      /*байт-ориентированный*/
      putchar ( 'c' );
    if (S_ISBLK(st_mode))      /*блок-ориентированный*/
      putchar ( 'b' );
    if (S_ISLNK(st_mode))      /*символическая ссылка*/
      putchar ( 'l' );
  } 

/*===================================================================== 
 * показать права доступа для владельца, группы и прочих пользователей
 *===================================================================== 
*/
void display_permission(int st_mode)
  {
    static const char xtbl[10] = "rwxrwxrwx";
    char amode[10];
    int i, j;
 
    for ( i = 0, j = ( 1 << 8 ); i < 9; i++, j >>= 1 )
      amode[i] = (st_mode & j) ? xtbl[i]: '-';
    
    amode[9]='\0';
    printf ("%s ", amode);
  }

/*=====================================================================
 * перечислить атрибуты одного файла
 *=====================================================================
*/
void long_list(char * path_name, int * flags)
{
  struct stat statv;
  struct passwd * pw_d;
    
  if (lstat(path_name, &statv) == -1)
    {
      fprintf(stderr, "файл или каталог %s должен существовать\n", path_name);
      return;
    }
  
  display_file_type ( statv.st_mode );   /*напечатать тип файла*/
  display_permission ( statv.st_mode );  /*напечатать права доступа*/
  
  printf ( "%ld ",statv.st_nlink );      /*значение счетчика жестких связей*/
  
  if (flags[flag_l])
    {
      pw_d = getpwuid ( statv.st_uid );  /*преобразовать UID в имя пользователя*/
      printf ( "%s ",pw_d->pw_name );    /*и напечатать его*/ 
    }
  if (flags[flag_g])
    {
      pw_d = getpwuid ( statv.st_gid );  /*преобразовать ID в имя группы пользователя*/ 
      printf ( "%s ",pw_d->pw_name );    /*и напечатать его*/  
    }
  
  printf ( "%5ld", statv.st_size );      /*или размер файла*/
  printf ( "  %s\n", path_name );        /*показать имя файла*/ 
}

/*=====================================================================
 * функция печатает таблицу
 *=====================================================================
*/
void PrFull(int * flags)
  { 
    #define Max 100
    FILE * fp;
    char str[Max];
    
    system("sort out.hhh > sort.hhh");
    
    if ((fp = fopen("sort.hhh", "r")) == NULL)
      {
        fprintf(stderr, "не могу открыть файл\n");
        return;
      }
    
    while (fscanf(fp, "%s", str) == 1)
      long_list(str, flags);
    
    fclose(fp);
  }

/*=====================================================================
 * функция определяет является ли файл скрытым
 *=====================================================================
*/
int IsHidden(char * word)
  {
    if (*word == '.')
      return 1;
    else
      return 0;
  }

void DirWalk(char *, FILE * fp, void (*fcn) (char *, FILE *) );

/*=====================================================================
 * Вызов функции DirWalk, которая реализует рекурсивную обработку 
 * файлов/каталогов (флаг -R), обработка флагов -l и -g. 
 *=====================================================================
*/
void fls_R0(char *name, FILE * fp)
  {
    struct stat std;
    
    if (stat(name, &std) == -1)
      {
        fprintf(stderr, "не существует файла или каталога %s", name);
        return;
      }
    if (S_ISDIR(std.st_mode))
      DirWalk(name, fp, fls_R0);
    
    fprintf(fp, "%s\n", name);
  }

void fls_R(char *name, int * flags)
  {
    FILE * fp;
    
    if ((fp = fopen("out.hhh", "w")) == NULL)
      {
        fprintf(stderr, "не могу открыть файл\n");
        return;
      }

    fls_R0(name, fp);
    
    fclose(fp);
    
    if (flags[flag_l] || flags[flag_g])
      PrFull(flags);
    else
      system("sort out.hhh");
      
  }

/*=====================================================================
 * Рекурсивная обработка файлов/каталогов, одним из параметров явл.
 * ссылка на функцию fls_R
 *=====================================================================
*/
void DirWalk(char * DirName, FILE * fp, void (*fcn) (char *, FILE *) )
  {
    #define MaxPath 1024
    
    DIR * dfd;
    struct dirent * dp;
    char name[MaxPath];
    
    if ((dfd = opendir(DirName)) == NULL) 
      {
        fprintf(stderr, "Не получается открыть %s\n",DirName);
        return;
      } 
    while ((dp = readdir(dfd)) != NULL)
      {
         if (IsHidden(dp->d_name))
              continue;
            
         if (strlen(DirName)+strlen(dp->d_name)+2 > sizeof(name))
           fprintf(stderr, "длинное имя %s %s\n", DirName, dp->d_name);
         else
           {
             sprintf(name, "%s/%s", DirName, dp->d_name);
             /*sprintf(name, "%s", dp->d_name);*/
             (*fcn)(name, fp);
           }
      }
        
    closedir(dfd);
  }

/*=====================================================================
 * Нерекурсивная обработка файлов/каталогов (т.е. без флагов)
 *=====================================================================
*/
void fls(char *DirName, int * flags)
  {
    #define MaxPath 1024
  
    DIR * dfd;
    struct dirent * dp;
    char name[MaxPath];
    
    FILE * fp;
    
    if ((fp = fopen("out.hhh", "w")) == NULL)
      {
        fprintf(stderr, "не могу открыть файл\n");
        return;
      }
    
    if ((dfd = opendir(DirName)) == NULL) 
      {
        fprintf(stderr, "Не получается открыть %s\n",DirName);
      } 
    else 
      {
        while ((dp = readdir(dfd)) != NULL)
          {
            if (IsHidden(dp->d_name))
              continue;
            if (flags[flag_l] || flags[flag_g])
              {
                sprintf(name, "%s/%s", DirName, dp->d_name);
                /*long_list(name, flags);*/
                fprintf(fp, "%s\n", name);
              }
            else
              fprintf(fp, "%s\n", dp->d_name);
          }
        closedir(dfd);
      }
    fclose(fp);
    
    if (flags[flag_l] || flags[flag_g])
      PrFull(flags);
    else
      system("sort out.hhh");
  }

/*=====================================================================
 * main
 *=====================================================================
*/
int main(int argc,char ** argv)
  {
    int i;
    int flags[3]={0};
    char * ban;
    
    if (argc == 1) 
        fls(".", flags); 
    else 
      {
        if ((ban = GetFlags(flags, argc, argv)) == NULL)
          {
            fprintf(stderr, "неверные флаги\n");
            return -1;
          }
        /*fprintf(stderr, "b=%s f_l=%d f_g=%d f_R=%d \n", ban, flags[flag_l], flags[flag_g], flags[flag_R]);*/
        
        if (argc == 2 && strcmp(ban, "-z") != 0)
          {
            if (flags[flag_R])
              fls_R(".", flags);
            else
              fls(".", flags);
          }
        
        for (i=1; i<argc; i++)
          {
            if (strcmp(argv[i],ban) == 0)
              continue;
            if (flags[flag_R])
              fls_R(argv[i], flags);
            else
              fls(argv[i], flags);
          }
      }
      
    return 0;
  }
