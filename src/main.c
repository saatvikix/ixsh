#include <stdio.h>
#include <string.h>

int main(void)
{
    printf("Welcome to ixsh!\n");

    while(true) {

        char buffer[1024];
        printf("ixsh> ");
        fflush(stdout);

        fgets(buffer, 1024, stdin);

        if(strcmp(buffer, "exit\n") == 0) {
            break;
        }

        printf("You entered: %s\n", buffer);        
        
    }
}