/*!
 * @author tdhuang
 * @date 2022/3/28
 * Copyright (c) 2022 tdhuang. All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dirent.h"
#include "rtklib.h"

#define FILESIZE 1024
#define FILENUM 100

/**
 * get list of files in the directory
 * @param filePath
 * @param file
 */
static void read_filedir(char *filePath, char file[FILENUM][FILESIZE]) {
    int i = 0, j = 0;
    int filesize = 0;
    DIR *dir     = NULL;
    struct dirent *entry;
    char temp[FILESIZE];

    if ((dir = opendir(filePath)) == NULL) {
        printf("opendir failed!");
    } else {
        while ((entry = readdir(dir))) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            strcpy(temp, filePath);
            strcat(temp, entry->d_name);
            strcpy(file[i], temp);
            i++;
            // printf("filename%d = %s\n", i, entry->d_name);
        }
    }
}

/**
 * append string at end
 * @param str
 * @param aim
 * @param add
 */
static void addchar(const char *str, char *aim, char *add) {
    int count = strlen(str);
    char temp[FILESIZE];
    strcpy(temp, str);

    *(temp + count - 1) = '\0';
    strcat(temp, add);
    strcpy(aim, temp);
}

extern void postInit(char *filepath) {
    gtime_t ts = {0}, te = {0};
    double ti = 0.0, tu = 0.0;
    int i, n            = 0, stat;
    char *rov, *base, *p, *q, *r;
    char file[FILENUM][FILESIZE] = {""};
    char *infile[FILENUM], outfile[FILESIZE];
    char proname[FILESIZE];

    read_filedir(filepath, file);
    // 处理参数
    prcopt_t prcopt = {
        PMODE_PPP_KINEMA,                      // mode
        0,                                     // soltype: 0:forward,1:backward,2:combined
        2,                                     // nf
        SYS_GPS | SYS_GAL | SYS_CMP | SYS_QZS, // navsys
        10.0 * D2R,                            // elmin
        {{0, 0}},                              /* snrmask */
        EPHOPT_PREC,                           // sateph
        1,                                     // modear
        1,                                     // glomodear
        1,                                     // bdsmodear
        5,                                     // maxout
        0,                                     // minlock
        10,                                    // minfix
        1,                                     // armaxiter
        IONOOPT_IFLC,                          // est ion
        TROPOPT_ESTG,                          // est trop
        0,                                     // dynamics
        2,                                     // tidecorr
        1,                                     // niter
        0,                                     // codesmooth
        0,                                     // intpref
        0,                                     // sbascorr
        0,                                     // sbassatsel
        0,                                     // rovpos
        0,                                     // refpos
        {100.0, 100.0},                        /* eratio[] */
        {100.0, 0.003, 0.003, 0.0, 1.0},       /* err[] */
        {30.0, 0.03, 0.3,10.0,10.0,60.0,60.0,0.01,30.0,60.0,60.0,0.6},/* initial-state std [0]bias,[1]iono [2]trop [3]acc  [4]vel [5] pos [6]clk
* [7]ZTDG [8]dcb [9]phase bias [10]iono [11] glo_ifb*/
        {1E-4, 1E-3, 1E-4, 1E-1, 1E-2, 0.0},   /* process-noise [0]bias,[1]iono [2]trop [3]acch [4]accv [5]pos */
        5E-12,                                 /* sclkstab */
        {3.0, 0.9999, 0.25, 0.1, 0.05},        /* thresar */
        0.0,                                   // elmaskar
        0.0,                                   // almaskhold
        {0.05,10},                                  // thresslip gf mw
        30.0,                                  // maxtdif
        30.0,                                  // maxinno
        30.0,                                  //,maxgdop
    };

    // 输出参数
    solopt_t solopt = {
        SOLF_XYZ,   TIMES_GPST, 1, 3, /* posf,times,timef,timeu */
        1,          1,          1, 0,
        0,          0,          0, /* degf,outhead,outopt,outvel,datum,height,geoid
                                    */
        0,          2,          5, /* solstatic,sstat,trace */
        {0.0, 0.0},                /* nmeaintv */
        " ",        ""             /* separator/program name */
    };
    // 文件参数
    filopt_t filopt = {"", "", "", "", "", "", "", "", "", "", "", "",""};
    for (i = 0; i < FILENUM; i++) {
        if (strlen(file[i]) == 0) {
            break;
        }
        char *substr  = file[i]+strlen(file[i])-4;
        char *substr2 =file[i]+strlen(file[i])-1;
        if (strcasecmp(substr, ".atx") == 0) {
            sprintf(filopt.satantp, "%s", file[i]); /* satellite antenna parameters file */
            sprintf(filopt.rcvantp, "%s", file[i]); /* receiver antenna parameters file */
            continue;
        } else if (strcasecmp(substr, ".snx") == 0) {
            sprintf(filopt.stapos, "%s", file[i]); /* station positions file */
            continue;
        } else if (strcasecmp(substr, ".BSX") == 0) {
            sprintf(filopt.dcb, "%s", file[i]); /* dcb data file */
            continue;
        } else if (strcasecmp(substr, ".BIA") == 0) {
            sprintf(filopt.bia, "%s", file[i]); /* dcb data file */
            continue;
        } else if (strcasecmp(substr, ".erp") == 0) {
            sprintf(filopt.eop, "%s", file[i]); /* eop data file */
            continue;
        } else if (strcasecmp(substr, ".blq") == 0) {
            sprintf(filopt.blq, "%s", file[i]); /* ocean tide loading blq file */
            continue;
        } else if (strcasecmp(substr2, "i") == 0) {
            sprintf(filopt.iono, "%s", file[i]); /* ionex file */
            continue;
        } else if (strcasecmp(substr2, "o") == 0 || strcmp(substr2, "O") == 0) {
            addchar(file[i], outfile, "pos");
            strcpy(proname, file[i]);
        }
        infile[n] = file[i];
        n += 1;
    }

    stat = postpos(ts, te, 0, 0, &prcopt, &solopt, &filopt, infile, n, outfile, "", "");
    printf("%s has done.\n", proname);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("give project_directory");
        return -1;
    }
    postInit(argv[1]);
    return 0;
}
