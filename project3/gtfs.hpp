//
//  gtfs.hpp
//  project3
//
//  Created by Dheeraj Kumar Ramchandani on 27/03/21.
//

#ifndef gtfs_hpp
#define gtfs_hpp


#include <string>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <unordered_map>
#include <sys/mman.h>
#include <fstream>
#include <dirent.h>
#include <sstream>

using namespace std;

#define PASS "\033[32;1m PASS \033[0m\n"
#define FAIL "\033[31;1m FAIL \033[0m\n"

// GTFileSystem basic data structures

#define MAX_FILENAME_LEN 255
#define MAX_NUM_FILES_PER_DIR 1024

extern int do_verbose;

static int global_id = 0;

typedef struct file {
    string filename;
    int file_length;
    int open, fd;
    char *buffer;
    
    file()
    {
        filename="";
        file_length =0;
        open =0; fd =-1;
    }
} file_t;

typedef struct gtfs gtfs_t;
typedef struct write {
    string filename, dirname;
    int offset;
    int length;
    char *data;
    char *new_data;
    gtfs_t *fs;
    file_t *file_buffer;
    int valid;
    int id;
    // TODO: Add any additional fields if necessary
} write_t;


typedef struct gtfs {
    string dirname;
    
    unordered_map<string, file_t*> file_map;
    
    unordered_map<string, pair<char*, int>> mmap_buffer;
    
    unordered_map<int, write_t*> transactions;
    unordered_map<write_t*, int> transaction_id;
    
    
    
    // TODO: Add any additional fields if necessary
} gtfs_t;











// GTFileSystem basic API calls

gtfs_t* gtfs_init(string directory, int verbose_flag);
int gtfs_clean(gtfs_t *gtfs);

file_t* gtfs_open_file(gtfs_t* gtfs, string filename, int file_length);
int gtfs_close_file(gtfs_t* gtfs, file_t* fl);
int gtfs_remove_file(gtfs_t* gtfs, file_t* fl);

char* gtfs_read_file(gtfs_t* gtfs, file_t* fl, int offset, int length);
write_t* gtfs_write_file(gtfs_t* gtfs, file_t* fl, int offset, int length, const char* data);
int gtfs_sync_write_file(write_t* write_id);
int gtfs_abort_write_file(write_t* write_id);

// BONUS: Implement below API calls to get bonus credits

int gtfs_clean_n_bytes(gtfs_t *gtfs, int bytes);
int gtfs_sync_write_file_n_bytes(write_t* write_id, int bytes);


#endif /* gtfs_hpp */
