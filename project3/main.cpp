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

void update_buffer( gtfs_t *gtf, char *buffer, int file_length, string file_name)
{
    // check whether the log file exist or not for that particular file and if it exists then update the buffer but first you have to mmap the file.
    string  log_file = gtf->dirname +"/" + file_name+"_log";
    
    if( check_file_exists(log_file))
    {
        std::ifstream file(log_file);
        std::string str;
        while (std::getline(file, str))
        {
            stringstream ss(str);
            string word;
            ss>>word;
            int offset = stoi( word);
            ss>>word;
            int length= stoi( word);
            
            ss>>word;
            for( int i = offset; i<offset+length; i++)
            {
                buffer[i] = word[i-offset];
            }
            
        }
        
    }
    
    
    
    
    
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
        
        // update the file data structure
        
        fl->filename = file_name;
        fl->fd =fd;
        fl->open =1;
        fl->file_length = file_length;
        
        
        // make a new buffer
        
        // mmap the file into a buffer
                                                                                                                                                       
        fl->buffer = (char*)mmap(0, file_length, PROT_READ|PROT_WRITE, MAP_PRIVATE,fd, 0);
        update_buffer(gtf,fl->buffer, file_length, filename);
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
    
    
    
    
    char *buf;
    buf = (char*)malloc(sizeof(char)*(length+1));
    
   
    
    for( int i = offset; i<offset+length; i++)
    buf[i-offset] =fl->buffer[i];
    
    return buf;
    
    
    
    
}
 
 


int main(int argc, const char * argv[]) {
    
    
    
    string dir ="./test";
    
    gtfs_t *g  = gtfs_init(dir, 0);
    
    
    file_t *x = gtfs_open_file( g, "test11.txt", 100 );
    
    char *buffer = gtfs_read_file(g, x, 0, 10);
    
    for( int i = 0; i<10; i++)
    cout<<buffer[i]<<" ";
    
    int check = gtfs_close_file(g, x);
    
    cout<<check<<endl;
    
    
    
    
    
    
    
    return 0;
    
    
   
    
    
    
    
    
}
