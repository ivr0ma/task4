#include <stdio.h>
#include <string.h>

typedef enum 
    {
      flag_n,
      flag_e,
      flag_E
    } Tflags;

/*=================================================================================
 * Функция получает на вход массив флагов, argc, argv, возвращает слово, если
 * последовательность флагов записана правильно (т.е. она содержит только посл-ть
 * символов 'n', 'e', 'E'), иначе NULL. Меняет массив флагов.
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
            while ((c = *(str++)) != '\0')
              {
                if (c == 'n')
                  flags[flag_n] = 1;
                else if (c == 'e')
                  flags[flag_e] = 1;
                else if (c == 'E')
                  flags[flag_E] = 1;
                else
                  {
                    flags[flag_n] = 0;
                    flags[flag_e] = 0;
                    flags[flag_E] = 0;
                    return NULL;
                  }
              }
            return argv[i];
          }
        
        return NULL;
      }
  }

/*=================================================================================
 * Функция получает на вход строку, определяет Escape последовательность и 
 * печатает строку 
 *=================================================================================
*/
void PrWithFl(char * s)
  {
    int c;
    int esc = 0;
    
    while ((c=*(s++)) != '\0')
      {
        /*fprintf(stderr, "\n-%c-\n", c);*/
        
        if (esc)
          switch (c)
            {
              case 'n': 
                esc = 0;
                putchar('\n'); 
                break;
              case 't':
                esc = 0;
                putchar('\t'); 
                break;
              case 'v':
                esc = 0;
                putchar('\v'); 
                break;
              case 'b':
                esc = 0;
                putchar('\b'); 
                break;
              case 'r':
                esc = 0;
                putchar('\r'); 
                break;
              default: 
                esc = 0;
                putchar('\\');
                if (c != '\\')
                  putchar(c);
            }
        else
          if (c == '\\')
            esc = 1;
          else
            putchar(c);
        
      }
  }

main(int argc, char **argv)
{
  int i;
  int flags[3]={0};
  char * ban;
  
  ban = GetFlags(flags, argc, argv);
  
  /*fprintf(stderr, "b=%s f_n=%d f_e=%d f_E=%d \n", ban, flags[flag_n], flags[flag_e], flags[flag_E]);*/
   
  for (i=1; i<argc; i++)
    {
      if (ban != NULL && strcmp(argv[i], ban) == 0)  /*игнорируем посл-ть флагов*/
        continue;
      if (flags[flag_e] == 1 && flags[flag_E] == 0)
        {
          PrWithFl(argv[i]); 
          printf("%s", (i < argc-1) ? " " : "");
        }
      else
        printf("%s%s", argv[i], (i < argc-1) ? " " : "");
    }
    
  if (flags[flag_n] == 0)
    printf("\n");
    
  return 0;
}
