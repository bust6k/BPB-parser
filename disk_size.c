#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>  

#define MIN_ARGS 2
#define POST_SUCCESS 1
#define POST_FAILURE 0
#define BPB_FAILURE 1

int* read_first_sector(int file_descriptor)
{
int* buf = malloc(512);

size_t readed = read(file_descriptor,buf,512);

if(readed != 512)
{
    printf("size of readed blocks not 512, his size:%d",readed);
}

if(readed == -1)
{
    printf("error of reading \n");
}

if(readed == 0)
{
    printf("EOF");
}

return buf;
}

bool is_hdd(char byte)
{
    return byte == 0xF8;
}

bool is_bpb(int byte1,int byte2)
{
    return byte1 == 0xEB || byte1 == 0xE9 && byte2 == 0xAA55;
}

int  post(int* data_for_check)
{

    if(!(is_bpb((*data_for_check),(*(data_for_check += 0x1FE)))))
    {
        printf("[BPB-PARSER] warning: the proggram work only with BPB-like disks \n");
        return POST_FAILURE;
    }
    data_for_check -= 0x1FE;

    if(is_hdd((*(data_for_check += 0x15))))
    {
        printf("[BPB-PARSER] warning: the proggram works only with floppy");
        return POST_FAILURE ;
    }
    return POST_SUCCESS;
}

int disk_size(int* bpb)
{
    int* byte_per_one_sector = bpb+= 0x0B;
    bpb -= 0x0B;
    int* sectors_per_tom = bpb+=  0x13;
    return  ((*byte_per_one_sector) * (*sectors_per_tom)) / 1024 / 1024;
}

int main(int argc,char *argv[])
{
if(argc < MIN_ARGS)
{
printf("[BPB-PARSER] erorr: the proggram must have an argument");
return BPB_FAILURE;
}
    
int disk_fd = open(argv[1],O_RDONLY);

if(disk_fd == -1)
{
    printf("[BPB-PARSER] error:are you sure the file exists?");
    return  BPB_FAILURE;
}

int* buf = read_first_sector(disk_fd);

int post_sucsess = post(buf);
    
if(post_sucsess)
{
printf("the disk size %d MiB",disk_size(buf));
}
    
free(buf);
}
