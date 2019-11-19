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

#define DEFAULT_CONFIG_CONTENT "userconfig\tnull\ndefault\toff\n"
#define VERSION "GPU Switcher for Dell XPS 15 1.0.0\nCopyright (C) 2019  mahoshojohcg\nThis program comes with ABSOLUTELY NO WARRANTY;\nThis is free software, and you are welcome to redistribute it\nunder certain conditions;\n"
#define BUFFER_SIZE 16384
#define SLEEP(X) usleep(1000*X)

char UserConfigFile[FILENAME_MAX + 1];
int DefaultStatus;
int CurrentStatus;

void EnableGPU();
void DisableGPU();

int main(int argc, char **argv)
{
    //check if querying version
    if (argc > 1 && !strcmp(argv[1], "--version"))
    {
        printf(VERSION);
        exit(EXIT_SUCCESS);
    }

    //check if run as root
    uid_t uid = getuid();
    if (uid != 0)
    {
        //abort when not run as root
        fprintf(stderr, "Must run as root, now aborting.\n");
        exit(EXIT_FAILURE);
    }
    char *buffer = malloc(16384);
    char *beginBuf;
    char *endBuf;
    int configfile = open("/etc/gpu.d/gpu.conf", O_RDONLY);
    if (configfile < 0)
    {
        //open faild, possible file not exist.

        //create dir
        if (!mkdir("/etc/gpu.d", S_IRGRP | S_IWGRP | S_IRUSR | S_IWUSR | S_IROTH))
        {
            //fails
            fprintf(stderr, "Create directory faild, now aborting.\n");
            free(buffer);
            exit(EXIT_FAILURE);
        }

        //create file
        configfile = open("/etc/gpu.d/gpu.conf", O_RDWR | O_CREAT, S_IRGRP | S_IWGRP | S_IRUSR | S_IWUSR | S_IROTH);
        if (configfile < 0)
        {
            //create faild
            fprintf(stderr, "Create config file faild, now aborting.\n");
            free(buffer);
            exit(EXIT_FAILURE);
        }

        //write default config to file
        write(configfile, DEFAULT_CONFIG_CONTENT, sizeof(DEFAULT_CONFIG_CONTENT));

        //close for reopen
        close(configfile);

        //reopen the file for readonly
        configfile = open("/etc/gpu.d/gpu.conf", O_RDONLY);

        //check if opened
        if (configfile < 0)
        {
            //open faild
            fprintf(stderr, "Open config file faild, now aborting.\n");
            free(buffer);
            exit(EXIT_FAILURE);
        }
    }

    //opened
    beginBuf = malloc(8192);
    endBuf = malloc(8192);

    //scan config
    for (;;)
    {
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
            else if (!strcmp(beginBuf, "default"))
            {
                //default status
                if (!strcmp(endBuf, "on"))
                    DefaultStatus = 1;
            }
        }
        //check if user config file updated
        if (strcmp(UserConfigFile, "null"))
        {
            free(beginBuf);
            free(endBuf);
            close(configfile);
            break;
        }
        //Check again after 100 ms.
        SLEEP(100);
        //move current to head of file read again.
        lseek(configfile, 0, SEEK_SET);
    }

    //Set current as default
    DefaultStatus ? EnableGPU() : DisableGPU();
    CurrentStatus = DefaultStatus;

    int userConfig = open(UserConfigFile, O_RDONLY);
    if (userConfig < 0)
    {
        //userConfig open faild
        fprintf(stderr, "User config file open faild, now aborting.\n");
        free(buffer);
        exit(EXIT_FAILURE);
    }
    printf("Deamon started successfully, now watching user config %s.\n", UserConfigFile);
    for (;;)
    {
        read(userConfig, buffer, BUFFER_SIZE);
        int tmp;
        tmp = buffer[0] - '0';
        if (tmp != CurrentStatus && (tmp == 0 || tmp == 1))
        {
            CurrentStatus = tmp;
            CurrentStatus ? EnableGPU() : DisableGPU();
        }
        SLEEP(100);
        lseek(userConfig, 0, SEEK_SET);
    }
}

void EnableGPU()
{
    printf("Now enabling.\n");
    system("mv /etc/modprobe.d/disable-nvidia.conf /etc/modprobe.d/disable-nvidia.conf.disable");
    system("echo -n 1 > /sys/bus/pci/devices/0000\\:01\\:00.0/remove");
    SLEEP(2);
    system("echo -n on > /sys/bus/pci/devices/0000\\:00\\:01.0/power/control");
    SLEEP(2);
    system("echo -n 1 > /sys/bus/pci/rescan");
    system("modprobe nvidia");
}
void DisableGPU()
{
    printf("Now disabling.\n");
    system("modprobe -r nvidia_drm");
    system("modprobe -r nvidia_uvm");
    system("modprobe -r nvidia_modeset");
    system("modprobe -r nvidia");
    system("echo -n auto > /sys/bus/pci/devices/0000\\:01\\:00.0/power/control");
    SLEEP(2);
    system("echo -n auto > /sys/bus/pci/devices/0000\\:00\\:01.0/power/control");
    SLEEP(2);
    system("mv /etc/modprobe.d/disable-nvidia.conf.disable /etc/modprobe.d/disable-nvidia.conf");
}