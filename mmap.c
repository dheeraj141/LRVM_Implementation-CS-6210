#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include <sys/mman.h>

int main( int argc, char *argv[]) 
{

	// open a file 
	int fd = open("./test1.txt", O_RDWR| O_CREAT, S_IRWXU); 

	if( fd == -1)
	{
		printf( " error in opening the file \n");
	}




	char *p ;

	p = (char*)mmap(0, 100, PROT_READ|PROT_WRITE, MAP_PRIVATE,fd, 0); 

	
	p[0] ='C'; 
	char c; 
	ssize_t nr; 

	nr = read( fd, &c, sizeof(c));
	if( nr >0)
	{
		printf(" call is successful and the value is %c\n", c);
	}

	else 
	{
		printf("read failed\n");
	}

	return 0;
}

	

	
	
