#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/mman.h>

#include "tests.h"

int test1() {
    fprintf(stderr, "Executing Test #1...\n"); 

    char* map;
    int fd;
    
    // addr = NULL so kernel chooses addres
    // length = 4096 -> page aligned
    // offset = 0
    if((map = mmap(NULL, PAGE_LEN, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd, 0)) == MAP_FAILED) {
        fprintf(stderr, "mmap failed in test 1: %s\n", strerror(errno));
        return 255; 
    }
    close(fd);

    map[3] = 'A';
    fprintf(stderr, "map[3] == '%c'\n", map[3]);

    // change protection to read-only
    if(mprotect(map, PAGE_LEN, PROT_READ) != 0) {
        fprintf(stderr, "Changing permissions in test 1 using mprotect failed: %s\n", strerror(errno));
        return 255;
    }

    fprintf(stderr, "writing a 'B'\n");
    map[3] = 'B';

    if(map[3] == 'B') {
        fprintf(stderr, "Write succedded in test 1\n");
        return 0;
    }
    else {
        fprintf(stderr, "Write failed in test 1: %s\n", strerror(errno));
        return 255;
    }
}

int test_template(int permission) {

    char* map;
    char file_byte;
    int fd;

    char* f_name = "testfile.txt";
    int offset = 3;
    char write_content = 'A';

    if((fd = open(f_name, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0) {
        fprintf(stderr, "Error opening %s for test 2: %s", f_name, strerror(errno));
        return -1;
    }

    // need to make sure file is of sufficient size
    if(ftruncate(fd, PAGE_LEN) == -1) {
        fprintf(stderr, "Error truncating file: %s\n", strerror(errno));
        return -1;
    }

    if((map = mmap(NULL, PAGE_LEN, PROT_READ | PROT_WRITE, permission, fd, 0)) == MAP_FAILED) {
        fprintf(stderr, "mmap failed in test 2: %s\n", strerror(errno));
        return -1; 
    }

    map[offset] = write_content;
    fprintf(stderr, "map[%d] == '%c'\n", offset, map[offset]);

    // lseek to offset
    if(lseek(fd, offset, SEEK_SET) == -1) {
        fprintf(stderr, "lseek to offset %d failed: %s\n", offset, strerror(errno));
        return -1;
    }

    // read 1 byte from the position we just lseek'd to
    if(read(fd, &file_byte, 1) != 1) {
        fprintf(stderr, "reading from map[%d] failed: %s\n", offset, strerror(errno));
        return -1;
    }

    // exit 1 if the file's byte didn't change - aka the write didn't occur in the file
    fprintf(stderr, "Byte read from file: '%c'\n", file_byte);
    if(file_byte != write_content) {
        fprintf(stderr, "\nCharacter is: %c from original\nCharacter is: %c from file\nBytes do not match!\n", write_content, file_byte); 
        return 1;
    }
    // exit 0 if the file's byte changes - aka the write occured in the file
    else {
        fprintf(stderr, "Bytes match!!!\n"); 
        return 0;
    }
    
}

int test2() {
    fprintf(stderr, "Executing Test #2...\n"); 
    test_template(MAP_SHARED);
}

int test3() {
    fprintf(stderr, "Executing Test #3...\n"); 
    test_template(MAP_PRIVATE);
}

int test4() {
    fprintf(stderr, "Executing Test #4...\n"); 

    char* map;
    char file_byte;
    int fd;

    char* f_name = "testfile4.txt";
    char write_content = 'X';

    int init_file_size = 4100;
    int offset_write = init_file_size + 1;
    int offset_extend = init_file_size + 16; // Offset to extend the file by 16 bytes

    if((fd = open(f_name, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0) {
        fprintf(stderr, "Error opening %s for test 4: %s\n", f_name, strerror(errno));
        return errno;
    }

    // make file 4101 bytes long
    if(ftruncate(fd, init_file_size) == -1) {
        fprintf(stderr, "Error truncating file: %s\n", strerror(errno));
        return errno;
    }

    // mmap 8192 bytes
    if((map = mmap(NULL, PAGE_LEN * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        fprintf(stderr, "mmap failed in test 4: %s\n", strerror(errno));
        return errno; 
    }

    // read from area beyond file length and see if it's mapped to 0
    fprintf(stderr, "reading memory beyond the end of the file\n");
    char mem_byte = map[offset_write];
    fprintf(stderr, "byte at offset %d in memory before writing: '%c'\n", offset_write, mem_byte);

    // write 'X' tp map[4101]
    fprintf(stderr, "writing '%c' to map[%d]\n", write_content, offset_write);
    map[offset_write] = write_content;

    // extend the file by seeking to map[4117] and writing X
    if(lseek(fd, offset_extend, SEEK_SET) == -1) {
        fprintf(stderr, "lseek to offset %d failed: %s\n", offset_extend, strerror(errno));
        return errno;
    }
    if(write(fd, &write_content, 1) != 1) {
        fprintf(stderr, "write to map[%d] failed: %s\n", offset_extend, strerror(errno));
        return errno;
    }

    // read from map[4101] and see if it's visible through the read syscall
    if(lseek(fd, offset_write, SEEK_SET) == -1) {
        fprintf(stderr, "lseek to offset %d failed: %s\n", offset_write, strerror(errno));
        return errno;
    }
    if(read(fd, &file_byte, 1) != 1) {
        fprintf(stderr, "read from map[%d] failed: %s\n", offset_write, strerror(errno));
        return errno;
    }
    fprintf(stderr, "byte read from file at offset %d: '%c'\n", offset_write, file_byte);

    // compare the byte read with 'X'
    if(file_byte == write_content) {
        fprintf(stderr, "\nbyte is visible in the file!!\n");
        return 0;
    } 
    else {
        fprintf(stderr, "\nbyte is not visible in the file\n");
        return 1;
    }
}

