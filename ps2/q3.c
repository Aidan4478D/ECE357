#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "inode_counts.h"
#include "stack.h"

int main(int argc, char* argv[]) {
    
    char* root_path;
    if(argc == 2) root_path = (argv[1][(strlen(argv[1]))-1] == '/' ? argv[1] : strcat(argv[1], "/"));
    else {
        printf("Please enter in exaclty one argument! (starting path)\n");
        return -1;
    }

    Stack directories;
    initialize(&directories);
    push(&directories, root_path);

    while(!is_empty(&directories)) {
        
        file_counts counts = {0};
        size_t file_sizes = 0;
        size_t disk_blocks = 0;
        size_t hard_links = 0;
        size_t broken_symlinks = 0;

        char* path = pop(&directories);
        printf("\nExploring directory path %s\n", path);

        struct dirent *entry;
        DIR *dr = opendir(path); 
        if (dr == NULL) { 
            printf("Could not open directory %s\n", root_path); 
            return -1; 
        } 

        while ((entry = readdir(dr)) != NULL) {
            
            struct stat file_stat;
            int type = entry->d_type; 
            int path_len = strlen(path), entry_len = strlen(entry->d_name);
            char* file_path = malloc(path_len + entry_len);

            snprintf(file_path, path_len + entry_len + 1, "%s%s", path, entry->d_name);
            /*printf("%s type is %d\n", entry->d_name, entry->d_type);*/
            
            /* part a */
            /*if (lstat(file_path, &file_stat) == 0 || type != DT_UNKNOWN) {*/
                /*if (S_ISREG(file_stat.st_mode) || type == DT_REG) counts.fcts[FILES]++;*/
                /*else if (S_ISLNK(file_stat.st_mode) || type == DT_LNK) counts.fcts[SYMS]++;*/
                /*else if (S_ISCHR(file_stat.st_mode) || type == DT_BLK) counts.fcts[BLOCKS]++;*/
                /*else if (S_ISBLK(file_stat.st_mode) || type == DT_CHR) counts.fcts[CHARS]++;*/
                /*else if (S_ISFIFO(file_stat.st_mode) || type == DT_FIFO) counts.fcts[FIFOS]++;*/
                /*else if (S_ISSOCK(file_stat.st_mode) || type == DT_SOCK) counts.fcts[SOCKS]++;*/
                /*else if (S_ISDIR(file_stat.st_mode) || type == DT_DIR) {*/
                    /*if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {*/
                        /*push(&directories, file_path);*/
                    /*}*/
                    /*counts.fcts[DIRS]++;*/
                /*}*/
            /*}*/
            /*else {*/
                /*printf("file type %d unknown\n", type);*/
            /*}*/

            if (lstat(file_path, &file_stat) == 0) {
                if (S_ISREG(file_stat.st_mode)) counts.fcts[FILES]++;
                else if (S_ISLNK(file_stat.st_mode)) counts.fcts[SYMS]++;
                else if (S_ISCHR(file_stat.st_mode)) counts.fcts[CHARS]++;
                else if (S_ISBLK(file_stat.st_mode)) counts.fcts[BLOCKS]++;
                else if (S_ISFIFO(file_stat.st_mode)) counts.fcts[FIFOS]++;
                else if (S_ISSOCK(file_stat.st_mode)) counts.fcts[SOCKS]++;
                else if (S_ISDIR(file_stat.st_mode)) {
                    if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                        push(&directories, strcat(file_path, "/"));
                    }
                    counts.fcts[DIRS]++;
                }
            }
            else if (type != DT_UNKNOWN) {
                if (type == DT_REG) counts.fcts[FILES]++;
                else if (type == DT_LNK) counts.fcts[SYMS]++;
                else if (type == DT_CHR) counts.fcts[CHARS]++;
                else if (type == DT_BLK) counts.fcts[BLOCKS]++;
                else if (type == DT_FIFO) counts.fcts[FIFOS]++;
                else if (type == DT_SOCK) counts.fcts[SOCKS]++;
                else if (type == DT_DIR) {
                    if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                        push(&directories, strcat(file_path, "/"));
                    }
                    counts.fcts[DIRS]++;
                }
            } 
            else {
                printf("could not identify file");
            }

            
            /* part b */
            file_sizes += file_stat.st_size;
            disk_blocks += file_stat.st_blocks;

            /* part c */
            if(!S_ISDIR(file_stat.st_mode) && file_stat.st_nlink > 1) hard_links += file_stat.st_nlink;
            
            /* part d */
            struct stat slink_stat;
            struct stat target_stat;
            // get info about link
            if (lstat(file_path, &slink_stat) == -1) continue;

            // check if link resolves
            if (S_ISLNK(slink_stat.st_mode)) {
                if (stat(file_path, &target_stat) == -1) {
                    if (errno == ENOENT) {
                        broken_symlinks++;
                    }
                }
            }
       }

        for (int i = 0; i < FILE_TYPE_COUNT; i++) {
            if(counts.fcts[i]) printf("%s: %d\n", labels[i], counts.fcts[i]);
        }
        printf("total file size is: %ld\n", file_sizes);
        printf("total disk blocks allocated is: %ld\n", disk_blocks);
        printf("total num hard links > 1 (excluding dirs) is: %ld\n", hard_links);
        printf("symlinks broken: %ld\n\n", broken_symlinks);

        // reset variable counts
        /*file_sizes = disk_blocks = hard_links = broken_symlinks = 0;*/
        closedir(dr);     
    }


    return 0; 
}
