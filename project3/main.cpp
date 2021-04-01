//
//  main.cpp
//  project3
//
//  Created by Dheeraj Kumar Ramchandani on 27/03/21.
//

#include "gtfs.hpp"

string directory ="./test";


gtfs_t *root = nullptr;

int verbose =0;

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
    
    
   
    do_verbose = verbose_flag;
    VERBOSE_PRINT(do_verbose, "Initializing GTFileSystem inside directory " << directory << "\n");
    
    if( root == nullptr)
    {
        root = new gtfs();
        root->dirname = directory;
       
        struct stat sb;
        
        // check whether the directory exists or not

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
        {
            cout<<"Error in stat system call"<<endl;
            return nullptr;
        }
    }
    
    return root;
    

    VERBOSE_PRINT(do_verbose, "Success\n"); //On success returns non NULL.
}




void update_buffer( char *buffer, int file_length, string dirname, string file_name)
{
    // check whether the log file exist or not for that particular file and if it exists then update the buffer but first you have to mmap the file.
    
   
    string log_dir = dirname +"_log";
    
    struct stat sb;
    
    // check whether the directory exists or not

    if (stat(log_dir.c_str(), &sb) == 0)
    {
        if( S_ISDIR(sb.st_mode))
        cout<<"Directory already exists"<<endl;
        else
        {
            cout<<"Directory does not exist and creating the directory"<<endl;
            
            if( mkdir( log_dir.c_str(), 0777)  == -1)
                cout<<"Error in creating the directory"<<endl;
            else
                cout<<"Directory created"<<endl;
        }
    }
    
    
    size_t index = file_name.find('.', 1);
    
    
    string log_file = log_dir +"/" + file_name.substr(0,index)+"_log.txt";
    
    if( check_file_exists(log_file))
    {
        int fd = open(log_file.c_str(), O_RDONLY);
        
        char c;
       
        
        
        int offset, length;
        
        while(  read(fd, &c, 1) ==1)
        {
            string word;
            while( c!= ' ')
            {
                word.push_back(c);
                read(fd, &c, 1);
                
            }
            offset = stoi(word);
            word ="";
            read(fd, &c, 1);
            while( c!= ' ')
            {
                word.push_back(c);
                read( fd, &c, 1);
            }
            length = stoi( word);
           
            for( int i = offset; i<offset+length; i++)
            {
                read( fd, &c, 1);
                buffer[i] =c;
            }
            
        }
        
        
        
    }
    
    
    
    
    
}

int  update_file(string dirname, string file_name, int bytes)
{
    string log_dir = dirname+ "_log";
    
    size_t index = file_name.find('.', 1);
    
    string original_file = dirname+"/" +file_name;
    
    
    
    
    string log_file = log_dir +"/" + file_name.substr(0,index)+"_log.txt";
    
    if( check_file_exists(log_file))
    {
        int fd = open(log_file.c_str(), O_RDWR);
        int fd1 = open(original_file.c_str(), O_RDWR);
        
        char c;
        
        int offset, length, count;
        
        count = 0;
        
        while( read(fd, &c, 1) ==1)
        {
            string word;
            while( c!= ' ')
            {
                word.push_back(c);
                read(fd, &c, 1);
                
            }
            offset = stoi(word);
            off_t ret = lseek(fd1, off_t( offset), SEEK_SET);
            if( ret == -1)
            {
                cout<<"Error in setting the file position"<<endl;
                return -1;
            }
            
            word ="";
            read(fd, &c, 1);
            while( c!= ' ')
            {
                word.push_back(c);
                read( fd, &c, 1);
            }
            length = stoi( word);
            
            if( bytes >0)
            {
                int i = 0;
                for( i = 0; i<length&& count<bytes; i++, ++count)
                {
                    read( fd, &c, 1);
                    write(fd1, &c, 1);
                    
                }
                if(count == bytes)
                {
                    string temp;
                    // means that current transaction has to be written in the file
                    if( i <length)
                    {
                        temp+=to_string( offset+bytes);
                        temp+=" ";
                        temp+=to_string( length-bytes);
                        temp+=" ";
                       
                    }
                    while( read( fd, &c, 1) ==1)
                        temp+=c;
                    close( fd);
                    fd = open(log_file.c_str(), O_RDWR);
                    if( ftruncate(fd, (int)temp.length()) == -1)
                    {
                        cout<<"Issue in  truncating the file"<<endl;
                        return -1;
                    }
                    for( auto &ch:temp)
                        write(fd, &ch, 1);
                    
                    return 0;
                    
                }
                
            }
            else
            {
                for( int i = 0; i<length; i++)
                {
                    read( fd, &c, 1);
                    write(fd1, &c, 1);
                    
                }
                
            }
          
            
            
            
        }
        
        if( bytes  == -1)
        {
            if( ftruncate(fd, 0) == -1)
            {
                cout<<"Error in  truncating the log file"<<endl;
                return -1;
                
            }
            
        }
        close(fd);
        close(fd1);
  
        
    }
    
    return 0;
    
    
    
}

int clean_log( gtfs_t *gtfs, int bytes)
{
    // copy all the data from the logs and updates the files all at once.
    
    DIR *d;
    
    struct dirent *dir;
    d =opendir(gtfs->dirname.c_str());
    
    if( d)
    {
        while( (dir = readdir(d)) != NULL)
        {
            //cout<<dir->d_name<<endl;
            if( strcmp(dir->d_name, ".") == 0 ||  strcmp(dir->d_name,"..") == 0||  strcmp(dir->d_name, ".DS_Store") ==0)
                continue;
            else
            {
                update_file(gtfs->dirname, string(dir->d_name),bytes);
            }
            
        }
        closedir(d);
    }
    return 0;
    
    
}
int gtfs_clean(gtfs_t* gtfs)
{
    return clean_log(gtfs, -1);
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
        
        struct stat st;
        stat(file_name.c_str(), &st);
        int size = (int)st.st_size;
        
        if( size < file_length)
        {
            if( ftruncate(fd, file_length) == -1)
                cout<<"Error in increasing the file size"<<endl;
        }
        
        // update the file data structure
        
        fl->filename = filename;
        fl->fd =fd;
        fl->open =1;
        fl->file_length = file_length;
        
        
        // make a new buffer
        
        // mmap the file into a buffer
                                                                                                                                                       
        fl->buffer = (char*)mmap(0, file_length, PROT_READ|PROT_WRITE, MAP_PRIVATE,fd, 0);
        update_buffer(fl->buffer, file_length, gtf->dirname, filename);
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
    buf = new char[ length+1];
    
    int i ;
   
    
    for( i = offset; i<offset+length; i++)
    buf[i-offset] =fl->buffer[i];
    buf[i-offset] = '\0';
    return buf;

}
write_t* gtfs_write_file(gtfs_t* gtfs, file_t* fl, int offset, int length, const char* data)
{
    write_t *w = new write_t();
    
    w->data = new char[length];
    w->new_data = new char[length];
    
    w->offset = offset;
    w->length = length;
    w->filename = fl->filename;
    w->dirname = gtfs->dirname;
    w->valid=1;
    w->id = global_id;
    w->fs = gtfs;
    w->file_buffer  =fl;
    
    // copy the previous value
    int i = offset;
    for(i =offset; i<offset+length; i++)
    w->data[i-offset] = fl->buffer[i];
    
    //save the w for later usage
    
    gtfs->transactions.insert({ w->id, w});
    global_id++;
    
    
   // write the new value
    
    for( i =offset; i<offset+length; i++)
    {
        fl->buffer[i] = data[i-offset];
        w->new_data[i-offset] = data[i-offset];
        
    }
    return w;
}



void print_buffer( char *buffer, int length)
{
    for( int i = 0; i<length; i++)
    cout<<buffer[i]<<" ";
    
    cout<<endl;
    
}
int write_file( int fd, const char *data,int length)
{
    ssize_t ret;
    
    while( length != 0 && ( ret=write(fd, data, length) ) !=0)
    {
        if( ret ==-1)
        {
            if( errno == EINTR)
                continue;
            cout<<"Write to the file failed"<<endl;
            return -1;
        }
        length-=ret;
        data+=ret;
    }
    return 0;
}


int sync_write( write_t *write_id, int bytes)
{
    // sync write will check for the log file whether if exists or not if exists open the log file and write the transactions and otherwise create the log file and then do the same
    
    
    size_t index = write_id->filename.find('.', 1);
    
    
    string log_file = write_id->dirname+ "_log"+ "/"+write_id->filename.substr(0, index)+"_log.txt";
    int fd;
    
    if( check_file_exists(log_file))
    {
        fd = open(log_file.c_str(), O_RDWR|O_APPEND);
    }
    else
    {
        // create the file
        fd = open(log_file.c_str(), O_RDWR| O_CREAT|O_APPEND, S_IRWXU| S_IRWXG);
        
    }
    string data;
    
    data += to_string( write_id->offset);
    data+=" ";
    
    
    data+= to_string(bytes);
    data+=" ";
    if( write_file(fd, data.c_str(), (int)data.length()) == -1)
    {
        return -1;
    }
    
    
    if(write_file(fd, write_id->new_data, bytes) == -1)
        return -1;
    
    if( fsync(fd) == -1)
    {
        
        cout<<"issue is writing to disk"<<endl;
        return -1;
    }
    else
        cout<<"Written to disk"<<endl;
    
    
    int bytes_written = (int)data.length() +bytes;
    
    
    if(bytes == write_id->length)
    {
        delete [] write_id->data;
        delete [] write_id->new_data;
        
       
        delete write_id;
        
    }
    else
    {
        // update the offset , length and the data in the buffers
        write_id->offset = write_id->offset+bytes;
        write_id->length-=bytes;
        
        char *old_data = new char[ write_id->length];
        char *new_data = new char[ write_id->length];
        strncpy(old_data, write_id->data+bytes, write_id->length );
        
        strncpy(new_data, write_id->new_data+bytes, write_id->length );
        
        
        delete [] write_id->data;
        delete [] write_id->new_data;
        
        write_id->data = old_data;
        write_id->new_data = new_data;
        
        
        
    }
  
    
    return bytes_written;
    
    
}

int gtfs_sync_write_file(write_t* write_id)
{
    return sync_write(write_id, write_id->length);
}

int gtfs_sync_write_file_n_bytes(write_t* write_id, int bytes)
{
    return sync_write(write_id, bytes);
}
int gtfs_clean_n_bytes(gtfs_t *gtfs, int bytes)
{
    return clean_log(gtfs, bytes);
}

int gtfs_abort_write_file(write_t* write_id)
{
    
    // abort means you copy the same data back to the mmap buffer for that you should have access to the mmap buffer but that bufffer lies in the gtfs instance and we are not passing the gtfs instance here
    
    for( int i = write_id->offset; i<write_id->offset+write_id->length; i++)
    {
        write_id->file_buffer->buffer[i] = write_id->data[i-write_id->offset];
    }
    delete [] write_id->data;
    delete [] write_id->new_data;
    delete write_id;
    
    return 0;
}

// **Test 1**: Testing that data written by one process is then successfully read by another process.
void writer() {
    gtfs_t *gtfs = gtfs_init(directory, verbose);
    string filename = "test1.txt";
    file_t *fl = gtfs_open_file(gtfs, filename, 100);

    string str = "Hi, I'm the writer.\n";
    write_t *wrt = gtfs_write_file(gtfs, fl, 10, str.length(), str.c_str());
    gtfs_sync_write_file(wrt);

    gtfs_close_file(gtfs, fl);
}

void reader() {
    gtfs_t *gtfs = gtfs_init(directory, verbose);
    string filename = "test1.txt";
    file_t *fl = gtfs_open_file(gtfs, filename, 100);

    string str = "Hi, I'm the writer.\n";
    char *data = gtfs_read_file(gtfs, fl, 10, str.length());
    if (data != NULL) {
        str.compare(string(data)) == 0 ? cout << PASS : cout << FAIL;
    } else {
        cout << FAIL;
    }
    gtfs_close_file(gtfs, fl);
}

void test_write_read() {
    int pid;
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(-1);
    }
    if (pid == 0) {
        writer();
        exit(0);
    }
    waitpid(pid, NULL, 0);
    reader();
}

// **Test 2**: Testing that aborting a write returns the file to its original contents.

void test_abort_write() {

    gtfs_t *gtfs = gtfs_init(directory, verbose);
    string filename = "test2.txt";
    file_t *fl = gtfs_open_file(gtfs, filename, 100);

    string str = "Testing string.\n";
    write_t *wrt1 = gtfs_write_file(gtfs, fl, 0, str.length(), str.c_str());
    gtfs_sync_write_file(wrt1);

    write_t *wrt2 = gtfs_write_file(gtfs, fl, 20, str.length(), str.c_str());
    gtfs_abort_write_file(wrt2);

    char *data1 = gtfs_read_file(gtfs, fl, 0, str.length());
    if (data1 != NULL) {
        // First write was synced so reading should be successfull
        if (str.compare(string(data1)) != 0) {
            cout << FAIL;
        }
        // Second write was aborted and there was no string written in that offset
        char *data2 = gtfs_read_file(gtfs, fl, 20, str.length());
        if (data2 == NULL) {
            cout << FAIL;
        } else if (string(data2).compare("") == 0) {
            cout << PASS;
        }
    } else {
        cout << FAIL;
    }
    gtfs_close_file(gtfs, fl);
}

void test_clean_n_bytes( )
{
    int pid;
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(-1);
    }
    if (pid == 0) {
        
        gtfs_t *g  = gtfs_init(directory, 0);
        
        file_t *x = gtfs_open_file( g, "test11.txt", 100 );
        
        string name ="Dheera";
        
        write_t *w1 = gtfs_write_file(g, x, 0, (int)name.length(), name.c_str());
        gtfs_sync_write_file(w1);
        gtfs_clean_n_bytes(g, 5);
        gtfs_close_file(g, x);
        exit(0);
    }
    waitpid(pid, NULL, 0);
    
    gtfs_t *g  = gtfs_init(directory, 0);
    
    file_t *x = gtfs_open_file( g, "test11.txt", 100 );
    string name ="Dheer";
    
    char *buffer = gtfs_read_file(g, x, 0, (int)name.length());
    if( strcmp( buffer, name.c_str()) == 0)
        cout<<"PASS"<<endl;
    else
        cout<<"FAIL"<<endl;
    
 
}

void test_sync_write_n_bytes()
{
    int pid;
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(-1);
    }
    if (pid == 0) {
        
        gtfs_t *g  = gtfs_init(directory, 0);
        file_t *x = gtfs_open_file( g, "test11.txt", 100 );
        string name ="Dheera";
        
        write_t *w1 = gtfs_write_file(g, x, 0, (int)name.length(), name.c_str());
        gtfs_sync_write_file_n_bytes(w1, 5);
        exit(0);
    }
    waitpid(pid, NULL, 0);
    
    gtfs_t *g  = gtfs_init(directory, 0);
    file_t *x = gtfs_open_file( g, "test11.txt", 100 );
    char *buffer = gtfs_read_file(g, x, 0, 5);
    
    if( strcmp(buffer, "Dheer") == 0)
        cout<<"PASS"<<endl;
    else
        cout<<"FAIL"<<endl;
    
    
}

void test_mutiple_writes()
{
    // this test checks multiple writes to the same location and multiple sync writes to the same location
    
    gtfs_t *g  = gtfs_init(directory, 0);
    file_t *x = gtfs_open_file( g, "test11.txt", 100 );
    
    string name ="Apple";
    
    char *buffer = gtfs_read_file(g, x, 0, (int)name.length());
    
    
    
    
    
    write_t *w1 = gtfs_write_file(g, x, 0, (int)name.length(), name.c_str());
    name="Orang";
    write_t *w2 = gtfs_write_file(g, x, 0, (int)name.length(), name.c_str());
    buffer = gtfs_read_file(g, x, 0, (int)name.length());
    
    if( strcmp(buffer, name.c_str()) == 0)
        cout<<"PASS"<<endl;
    else
        cout<<"FAIL"<<endl;
    gtfs_sync_write_file(w2);
    gtfs_sync_write_file(w1);
    int pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(-1);
    }
    if (pid == 0) {
        gtfs_t *g  = gtfs_init(directory, 0);
        file_t *x = gtfs_open_file( g, "test11.txt", 100 );
        string name ="Apple";
        char *buffer = gtfs_read_file(g, x, 0, (int)name.length());
        if( strcmp(buffer, name.c_str()) == 0)
            cout<<"PASS"<<endl;
        else
            cout<<"FAIL"<<endl;
        
        exit(0);
    }
    
    waitpid(pid, NULL, 0);
 
}

void test_clean_n_b()
{
    gtfs_t *g  = gtfs_init(directory, 0);
    
    file_t *x = gtfs_open_file( g, "test11.txt", 100 );
    
    string name ="Dheeraj Kumar Ramchandani";
    
    write_t *w1 = gtfs_write_file(g, x, 0, (int)name.length(), name.c_str());
    gtfs_sync_write_file(w1);
    gtfs_clean_n_bytes(g, 5);
    gtfs_close_file(g, x);
    
}
 

 



int main(int argc, const char * argv[]) {
    
    
    test_clean_n_b();
    
    cout << "================== Test 1 ==================\n";
    cout << "Testing that clean n bytes function whether n bytes are written to the file or not .\n";
    
    test_clean_n_bytes();
    
    cout << "================== Test 2 ==================\n";
    cout << "Testing the sync n bytes function only n bytes of the data should be written and read afterwards.\n";
    
    test_sync_write_n_bytes();
    
    cout << "================== Test 3 ==================\n";
    cout << "Testing the  multiple , continuous writes and multiple sync writes.\n";
    
    test_mutiple_writes();
    
    cout << "================== Test 4 ==================\n";
    cout << "Testing that data written by one process is then successfully read by another process.\n";
    test_write_read();

    cout << "================== Test 5 ==================\n";
    cout << "Testing that aborting a write returns the file to its original contents.\n";
    test_abort_write();
 
    return 0;

}
