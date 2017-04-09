#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include <sched.h>


const char *netns_template = "/var/run/netns/user.%s";


int main(int argc, char **argv)
{
    uid_t ruid, euid, suid;
    char *nspath = NULL;

    // Debug
#if 0
    {
        pid_t pid = getpid();
        printf("PID: %d\n", pid);
        raise(SIGSTOP);
    }
#endif

    // Check IDs
    {
        if (getresuid(&ruid, &euid, &suid) < 0)
        {
            perror("Could not initialize");
            return 5;
        }

        if (euid != 0)
        {
            puts("Error: Must be a setuid binary");
            return 1;
        }
        if (ruid == 0)
        {
            puts("Error: As root, use `ip netns exec <ns name>` instead");
            return 1;
        }
    }

    // Get netns path
    {
        
        struct passwd *pw;
        size_t len;
        
        pw = getpwuid(ruid);

        if (!pw)
        {
            perror("Could not identify user");
            return 6;
        }

        len = snprintf(NULL, 0, netns_template, pw->pw_name);

        nspath = malloc(++len);

        if (!nspath)
        {
            perror("Could not identify user");
            return 6;
        }

        snprintf(nspath, len, netns_template, pw->pw_name);
    }

    // Check path
    {
        struct stat nsstat;

        if (lstat(nspath, &nsstat) < 0)
        {
            perror("Could not find user namespace");
            puts("You might want to contact your system administrator");
            return 2;
        }
    }

    // Enter
    {
        int fd = open(nspath, O_RDONLY);

        if (setns(fd, CLONE_NEWNET) < 0)
        {
            perror("Could not join namespace");
            return 3;
        }
    }

    // Drop privs
    setresuid(ruid, ruid, ruid);

    // Exec
    memmove(argv[0], argv[1], sizeof(char *) * (argc - 1));
    argv[argc - 1] = NULL;

    execvp(*argv, argv);
    perror("Could not execute specified command");
    return 4;
}

