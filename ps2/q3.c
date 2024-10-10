#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "inode_counts.h"
#include "stack.h"

// find problematic names
int problematic_name(const char *name) {
    for (int i = 0; name[i] != '\0'; ++i) {

        unsigned char c = (unsigned char)name[i];
        
        // if character is non printable ascii or shell return 1
        if (strchr(" \t*?{}$&;\'\"`\\|()<>~", c) || (c < 32 || c >= 127)) return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    
    char* root_path;
    //add / on the end of directory path if it doesn't exist
    if(argc == 2) root_path = (argv[1][(strlen(argv[1]))-1] == '/' ? argv[1] : strcat(argv[1], "/"));
    else {
        printf("Please enter in exaclty one argument! (starting path)\n");
        return -1;
    }

    // use a stack to search directories in a DFS manner
    Stack directories;
    initialize(&directories);
    push(&directories, root_path);

    while(!is_empty(&directories)) {
        
        file_counts counts = {0};
        size_t file_sizes = 0;
        size_t disk_blocks = 0;
        size_t hard_links = 0;
        size_t broken_symlinks = 0;
        size_t prob_paths = 0;

        char* path = pop(&directories);
        printf("\nExploring directory path %s\n", path);

        struct dirent *entry;
        DIR *dr = opendir(path); 
        if (dr == NULL) { 
            printf("Could not open directory %s: %s\n", root_path, strerror(errno)); 
            continue;
        } 

        while ((entry = readdir(dr)) != NULL) {
            
            struct stat file_stat;
            struct stat target_stat;

            int type = entry->d_type; 
            int path_len = strlen(path), entry_len = strlen(entry->d_name);
            char* file_path = malloc(path_len + entry_len);

            // add 1 to account for \0
            snprintf(file_path, path_len + entry_len + 1, "%s%s", path, entry->d_name);
            
            /* part a - report inode counts */
            if (lstat(file_path, &file_stat) == 0) {
                if (S_ISREG(file_stat.st_mode)) {
                    
                    /* part b - report regular file sizes and block space */
                    file_sizes += file_stat.st_size;
                    disk_blocks += file_stat.st_blocks;

                    counts.fcts[FILES]++;
                }
                else if (S_ISDIR(file_stat.st_mode)) {

                    // add new directories (that aren't . and ..) onto the stack and add to counts
                    if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                        push(&directories, strcat(file_path, "/"));
                        counts.fcts[DIRS]++;
                    }
                }
                else if (S_ISLNK(file_stat.st_mode)) {
                    /* part d */

                    // if target path doesn't exist add 1 to broken_symlinks
                    if (stat(file_path, &target_stat) == -1) {
                        if (errno == ENOENT) broken_symlinks++;
                    }
                    counts.fcts[SYMS]++;
                }
                else if (S_ISCHR(file_stat.st_mode)) counts.fcts[CHARS]++;
                else if (S_ISBLK(file_stat.st_mode)) counts.fcts[BLOCKS]++;
                else if (S_ISFIFO(file_stat.st_mode)) counts.fcts[FIFOS]++;
                else if (S_ISSOCK(file_stat.st_mode)) counts.fcts[SOCKS]++;
            }
            else printf("Could not identify file '%s' with lstat num %d: %s\n", file_path, lstat(file_path, &file_stat), strerror(errno));

            /* part c */
            if((!S_ISDIR(file_stat.st_mode) || !(type == DT_DIR)) && file_stat.st_nlink > 1) hard_links += file_stat.st_nlink;

            /* part e */
            if (problematic_name(entry->d_name)) {
                /*printf("problematic name is %s\n", entry->d_name);*/
                prob_paths++;
            }
        }

        // print directory statistics
        for (int i = 0; i < FILE_TYPE_COUNT; i++) {
            if(counts.fcts[i]) printf("%s: %d\n", labels[i], counts.fcts[i]);
        }
        printf("total file size is: %ld\n", file_sizes);
        printf("total disk blocks allocated is: %ld\n", disk_blocks);
        printf("total num hard links > 1 (excluding dirs) is: %ld\n", hard_links);
        printf("symlinks broken: %ld\n", broken_symlinks);
        printf("problematic pathnames: %ld\n\n", prob_paths);

        closedir(dr);     
    }

    return 0; 
}
