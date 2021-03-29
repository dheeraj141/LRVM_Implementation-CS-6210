//
//  main.cpp
//  project3
//
//  Created by Dheeraj Kumar Ramchandani on 27/03/21.
//

#include "gtfs.hpp"

#define VERBOSE_PRINT(verbose, str...) do { \
    if (verbose) cout << "VERBOSE: "<< __FILE__ << ":" << __LINE__ << " " << __func__ << "(): " << str; \
} while(0)

int do_verbose;


int check_file_exists( string file_name)
{
    struct stat sb;
    
    return ( stat(file_name.c_str(), &sb) == 0);
    
    
}

gtfs_t* gtfs_init(string directory, int verbose_flag) {
    
    gtfs_t *root = nullptr;
   
    do_verbose = verbose_flag;
    VERBOSE_PRINT(do_verbose, "Initializing GTFileSystem inside directory " << directory << "\n");
    
    if( root == nullptr)
    {
        root = new gtfs();
        
        
        root->dirname = directory;
        struct stat sb;

        if (stat(root->dirname.c_str(), &sb) == 0)
        {
            if( S_ISDIR(sb.st_mode))
            cout<<"Directory already exists"<<endl;
            else
            {
                cout<<"Directory does not exist and creating the directory"<<endl;
                
                if( mkdir( root->dirname.c_str(), 0777)  == -1)
                    cout<<"Error in creating the directory"<<endl;
                else
                    cout<<"Directory created"<<endl;
                
            }
                
        }
        else
            cout<<"stat call failed"<<endl;
    }
    
    return root;
    

    VERBOSE_PRINT(do_verbose, "Success\n"); //On success returns non NULL.
}



file_t* gtfs_open_file(gtfs_t* gtf, string filename, int file_length) {
    
    file_t *fl = new file_t();
    
    string file_name = gtf->dirname+ "/" + filename;
    
    if(  gtf->mmap_buffer.count(file_name) == 0 )
    {
        cout<<"file is not opened or created "<<endl;
        // open a file
        int fd = open(file_name.c_str(), O_RDWR| O_CREAT, S_IRWXU| S_IRWXG);
                                                                                                                                                     
        if( fd == -1)
        {
            cout<<"Error in opening the file"<<endl;
        }
        else
            cout<<"file opened"<<endl;
        
        fl->filename = file_name;
        fl->fd =fd;
        fl->open =1;
        fl->file_length = file_length;
        
        char *buffer;
        buffer = new char[ file_length];
        std::fill_n(buffer, file_length, '1');
        
        
        
    }
    else
    {
        cout<<"file is already opened"<<endl;
        
        fl = gtf->file_map.at(file_name);
        
    }
  
    if (gtf != nullptr) {
        VERBOSE_PRINT(do_verbose, "Opening file " << filename << " inside directory " << gtf->dirname << "\n");
    } else {
        VERBOSE_PRINT(do_verbose, "GTFileSystem does not exist\n");
        return NULL;
    }
    
    

    VERBOSE_PRINT(do_verbose, "Success\n"); //On success returns non NULL.
    return fl;
}



void update_buffer( char *buffer, int file_length)
{
    
}

int gtfs_close_file(gtfs_t* gtfs, file_t* fl)
{
    if( fl->open  == 0)
    {
        cout<<"File is not opened"<<endl;
        return -1;
    }
    if( close( fl->fd) == -1)
    {
        cout<<"Error in closing the file"<<endl;
        return -1;
        
    }
        
    else
    {
        cout<<" closed the file"<<endl;
        fl->fd =-1;
        fl->open = 0;
        return 0;
    }
    
    
    
}
char* gtfs_read_file(gtfs_t* gtfs, file_t* fl, int offset, int length)
{
    
    off_t ret;
    ret = lseek(fl->fd, offset, SEEK_SET);
    
    if( ret == -1)
        cout<<"Error in setting the file descriptor"<<endl;
    else
        cout<<"Operation successful"<<endl;
    
    
    
    
    char *buf;
    buf = (char*)malloc(sizeof(char)*(length+1));
    
    ssize_t bytes_read;
    
    while( length >0 && (bytes_read =read( fl->fd,  buf, length)!= 0 ))
    {
        if( bytes_read == -1)
        {
            if( errno == EINTR)
                continue;
            cout<<" Error in reading the file"<<endl;
            break;
        }
        length-=bytes_read;
        buf+=bytes_read;
        
    }
    
    return buf;
    
    
    
    
}
 
 


int main(int argc, const char * argv[]) {
    
    
    
    string dir ="./test";
    
    gtfs_t *g  = gtfs_init(dir, 0);
    
    
    file_t *x = gtfs_open_file( g, "test11.txt", 100 );
    
    int check = gtfs_close_file(g, x);
    
    cout<<check<<endl;
    
    
    
    
    
    
    
    return 0;
    
    
   
    
    
    
    
    
}
