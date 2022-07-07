#include <stdio.h>
#include <unistd.h>

#define BUF_MAX 255

int main(int argc, char **argv) {
    char buf[BUF_MAX];
    
    if (getcwd(buf, BUF_MAX) == NULL) 
      {
        fprintf(stderr, "не могу получить путь\n");
        return -1;
      }
    
    printf("%s\n", buf);
    return 0;
}
