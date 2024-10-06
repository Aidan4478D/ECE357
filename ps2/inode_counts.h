#ifndef _INODE_COUNTS_H
#define _INODE_COUNTS_H

const char *labels[] = {
    "regular files",
    "directories",
    "sym links",
    "block devs",
    "character devs",
    "fifos",
    "sockets"
};

enum FileTypes {
    FILES,
    DIRS,
    SYMS,
    BLOCKS,
    CHARS,
    FIFOS,
    SOCKS,
    FILE_TYPE_COUNT
};

typedef struct {
    int fcts[FILE_TYPE_COUNT]; //keep track of file counts
} file_counts;

#endif
