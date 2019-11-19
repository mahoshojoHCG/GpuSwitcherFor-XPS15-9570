/*
    Copyright (C) 2019  mahoshojohcg

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

char UserConfigFile[FILENAME_MAX + 1];

#define VERSION "GPU Switcher for Dell XPS 15 1.0.0\nCopyright (C) 2019  mahoshojohcg\nThis program comes with ABSOLUTELY NO WARRANTY;\nThis is free software, and you are welcome to redistribute it\nunder certain conditions;\n"
#define HELP "usage:\ngpum on\t\tEnable GPU\ngpum off\t\tDisable GPU\ngpum --help\tShow this message and exit.\n"
#define BUFFER_SIZE 16384

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf(HELP);
        exit(EXIT_FAILURE);
    }
    if (!strcmp(argv[1], "--version"))
    {
        printf(VERSION);
        exit(EXIT_SUCCESS);
    }
    if (!strcmp(argv[1], "--help"))
    {
        printf(HELP);
        exit(EXIT_SUCCESS);
    }
    int configfile = open("/etc/gpu.d/gpu.conf", O_RDONLY);
    char *buffer = malloc(16384);
    char *beginBuf;
    char *endBuf;
    if (configfile < 0)
    {
        //open faild
        fprintf(stderr, "Open config file faild, now aborting.\n");
        free(buffer);
        exit(EXIT_FAILURE);
    }
    //opened
    beginBuf = malloc(8192);
    endBuf = malloc(8192);

    while (read(configfile, buffer, BUFFER_SIZE))
    {
        if (buffer[0] == '#')
            continue;
        sscanf(buffer, "%s%s", beginBuf, endBuf);
        if (!strcmp(beginBuf, "userconfig"))
        {
            //userconfig
            strcpy(UserConfigFile, endBuf);
        }
    }

    //check if user config file updated
    if (strcmp(UserConfigFile, "null"))
    {
        free(beginBuf);
        free(endBuf);
        close(configfile);
    }
    else
    {
        fprintf(stderr, "Config path is null\n");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    int userConfig = open(UserConfigFile, O_TRUNC | O_RDWR | O_CREAT, S_IRGRP | S_IWGRP | S_IRUSR | S_IWUSR | S_IROTH);
    if (userConfig < 0)
    {
        //open faild
        fprintf(stderr, "Open config file faild, now aborting.\n");
        free(buffer);
        exit(EXIT_FAILURE);
    }
    if (!strcmp(argv[1], "on"))
    {
        printf("Now turnning on.\n");
        write(userConfig, "1", 2);
    }
    if (!strcmp(argv[1], "off"))
    {
        printf("Now turnning off.\n");
        write(userConfig, "0", 2);
    }
}