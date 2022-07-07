#include <stdio.h>

void filecopy(FILE *ifp, FILE *ofp, int *i, int flag)
{
  int c;
  
  if ((c = getc(ifp)) == EOF)
    return;
  
  if (flag)
  {
    fprintf(ofp, "%d\t", (*i)++);
  }
  else
    putc('\t', ofp);
    
  ungetc(c, ifp);
  while ((c = getc(ifp)) != EOF)
  {  
    if (c == '\n')
    {  
      putc(c, ofp);
      if ((c = getc(ifp)) == EOF)
        return;
      if (flag)
        fprintf(ofp, "%d\t", (*i)++);
      else
        putc('\t', ofp);
      ungetc(c, ifp);
    }
    else
      putc(c, ofp);
  }
}


main(int argc, char **argv)
{
  FILE *fp;
  char *prog = argv[0];
  int flag = 0, i;
  int count=1;
  
  if (argc == 1)
    filecopy(stdin, stdout, &count, flag);
  else
    {
    for (i=0; i<argc; i++)
      if (strcmp(argv[i],"-n") == 0)
        flag = 1;
        
    if ((argc == 2) && flag)
    {
      filecopy(stdin, stdout, &count, flag);
      return 0;
    }
        
    i = 0;
    while (++i < argc)
    {  
      if (strcmp(argv[i],"-n") == 0)
      {  
      	 /*fprintf(stderr, "+++++\n");*/
      	 continue;
      }
      if ((fp = fopen(argv[i], "r")) == NULL) 
      {
        fprintf(stderr, "%s: не могу открыть файл %s\n", prog, argv[i]);
        return -1;
      } 
      else
      {
        filecopy(fp, stdout, &count, flag);
        fclose(fp);
      }
    }
    }
  return 0;
}
