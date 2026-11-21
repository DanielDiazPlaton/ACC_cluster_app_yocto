#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>/*delay*/
#include "middleware.h"




int main()
{
    Ostask_create();

    while(1)
    {
        
        sleep(3);
    }

    return 0;
}