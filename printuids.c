#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>


int main(void)
{
    uid_t ruid, euid, suid;

    getresuid(&ruid, &euid, &suid);

    printf("Real UID: %d\nEffective UID: %d\nSaved UID: %d\n", ruid, euid, suid);

    return 0;
}
