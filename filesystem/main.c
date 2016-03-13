//
//  main.c
//  filesystem
//
//  Created by justin01031 on 2/28/16.
//  Copyright © 2016 justin01031. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <math.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mach/thread_policy.h>
#include "timing.c"

char* random_char_array(unsigned long long entries) {
    srand((unsigned int)time(NULL));
    unsigned long long i = 0;
    unsigned long long arraySize = entries * sizeof(char);
    char* array = (char*)malloc(arraySize);
    for (i=0; i<entries; i++) {
        *(array+i) = rand()%26+'a';
    }
    return array;
}

void createCharFile(char* fileName, int sizeMB){
    int oneKB=1024;
    int filedesc = open(fileName, O_RDWR |O_CREAT| O_APPEND);
    
    for(int i=0;i<sizeMB;i++){
        char* oneMB=random_char_array(oneKB*oneKB);
        write(filedesc,oneMB, 1024*1024);
        free(oneMB);
    }    //fopen bufferIO
    //open nobufferIO
    close(filedesc);
    
}
void sizeOfFileCache(char* filePath, int iteraLimit){
    uint64_t start;
    uint64_t end;

    char fileName[10] = ".tmp";
    int file=0;
    //==============function warmup====
    char fileFullPath[200]= {0};
    strcat(fileFullPath, filePath);
    strcat(fileFullPath, "0.tmp");
    int filedesc = open(fileFullPath, O_RDONLY);
    char* buffer=(char*)malloc(1000);
    for(int i=0;i<=1000;i++){
        ssize_t bytesRead=read(filedesc,buffer,1);
    }
    free(buffer);
    //==============function warmup====

    for(int i=0; i<=10;i++){// i different size of file
        char fileFullPath[200]= {0};
        char numBuff[20];
        sprintf(numBuff, "%d", file);
        //int onceRead=1024*256;
        int fileSize=1024*1024*pow((float)2,i);
        strcat(fileFullPath, filePath);
        strcat(fileFullPath,numBuff);
        strcat(fileFullPath, fileName);
       // printf("file path is:\n %s\n",fileFullPath);
       // printf("start to read the file size= %d \n ",fileSize);
        double  totalNano = 0.0;
        for(int itera=0;itera<iteraLimit;itera++){
            int filedesc = open(fileFullPath, O_RDWR | O_APPEND);

            char* buffer=(char*)malloc(fileSize);
            start = rdtsc();
            ssize_t bytesRead=read(filedesc,buffer,fileSize);
            //printf("%zd\n",bytesRead);
            end = rdtsc();
            if(bytesRead<=0) {
                printf("Error: no read file\n");
                return ;
            }
            totalNano+=end-start;
          //  printf("%llu\n", end-start);
            free(buffer);
            close(filedesc);
        }
       // printf("size = %f, cycles = %lf average read I/O time=%lf\n",pow((float)2,i), totalNano/iteraLimit,totalNano/iteraLimit/pow((float)2,i));
         printf("size = %f, read I/O time=%lf\n",pow((float)2,i), totalNano/iteraLimit/pow((float)2,i));
        file++;
    }


}
void sizeOfFileCacheSpecBaseSize(char* filePath,int stride,int iteraLimit){
    uint64_t start;
    uint64_t end;
    
    char fileName[10] = ".tmp";
    int file=0;
    //==============function warmup====
    char fileFullPath[200]= {0};
    strcat(fileFullPath, filePath);
    strcat(fileFullPath, "0.tmp");
    int filedesc = open(fileFullPath, O_RDWR);
    char* buffer=(char*)malloc(1000);
    for(int i=0;i<=1000;i++){
        ssize_t bytesRead=read(filedesc,buffer,1);
    }
    free(buffer);
    lseek(filedesc, 0, SEEK_SET);
    close(filedesc);
    //==============function warmup====
    
    for(int i=0; i<=10;i++){// i different size of file
        char fileFullPath[200]= {0};
        char numBuff[20];
        sprintf(numBuff, "%d", file);
        strcat(fileFullPath, filePath);
        strcat(fileFullPath,numBuff);
        strcat(fileFullPath, fileName);
        
        
        int onceRead=1024*stride;
        int fileSize=1024*1024*pow((float)2,i);
        char* buffer=(char*)malloc(onceRead);
       //  printf("file path is:\n %s\n",fileFullPath);
        // printf("start to read the file size= %d \n ",fileSize);
        double  totalNano = 0.0;
        long long int pos = 0;
        int filedesc = open(fileFullPath, O_RDONLY );
        for(int itera=0;itera<iteraLimit;itera++){

            start = rdtsc();
            ssize_t bytesRead=read(filedesc,buffer,onceRead);
            end = rdtsc();
            if(bytesRead<=0) {
                printf("Error: no read file\n");
                return ;
            }
            pos+=(int)(bytesRead/1024);
            totalNano+=end-start;
            //printf("%d\n",pos);
            //printf("%s\n",buffer);
            //  printf("%llu\n", end-start);
            if(fileSize/1024-pos<onceRead){
               int holder= lseek(filedesc, 0, SEEK_SET);
                if(holder<0){
                    printf("Error: descriptor error\n");
                    return ;
                }
            }
            
        }
        close(filedesc);
        free(buffer);
        // printf("size = %f, cycles = %lf average read I/O time=%lf\n",pow((float)2,i), totalNano/iteraLimit,totalNano/iteraLimit/pow((float)2,i));
        printf("size = %fMB, read I/O time=%lf\n",pow((float)2,i), totalNano/iteraLimit);
        file++;
    }
    
    
}
void getFileSeqRead(char* filePath,int onceReadMB,int iteraLimit){
    uint64_t start;
    uint64_t end;
    int onceReadBytes=onceReadMB*1024*1024;
    char fileName[10] = ".tmp";
    int file=0;
    //==============function warmup====
        char fileFullPath[200]= {0};
        strcat(fileFullPath, filePath);
        strcat(fileFullPath, "0.tmp");
        int filedesc = open(fileFullPath, O_RDONLY);
        char* buffer=(char*)malloc(1000);
        for(int i=0;i<=1000;i++){
            ssize_t bytesRead=read(filedesc,buffer,1);
        }
        free(buffer);
        lseek(filedesc, 0, SEEK_SET);
        close(filedesc);
    //==============function warmup====
    
    for(int i=0; i<=10;i++){// i different size of file
        char fileFullPath[200]= {0};
        char numBuff[20];
        sprintf(numBuff, "%d", file);
        strcat(fileFullPath, filePath);
        strcat(fileFullPath,numBuff);
        strcat(fileFullPath, fileName);
        
        
        int fileSize=1024*1024*pow((float)2,i);
        char* buffer=(char*)malloc(onceReadBytes);
        //  printf("file path is:\n %s\n",fileFullPath);
        // printf("start to read the file size= %d \n ",fileSize);
        double  totalNano = 0.0;
        long long int pos = 0;
        int filedesc = open(fileFullPath, O_RDONLY );
        fcntl( filedesc,F_NOCACHE);   //disable the file cache
        
        for(int itera=0;itera<iteraLimit;itera++){
            
            start = rdtsc();
            ssize_t bytesRead=read(filedesc,buffer,onceReadBytes);
            end = rdtsc();
            if(bytesRead<=0) {
                printf("Error: no read file\n");
                return ;
            }
            pos+=(int)(bytesRead);
            totalNano+=end-start;
            //printf("%d\n",pos);
            //printf("%s\n",buffer);
            //  printf("%llu\n", end-start);
            if(fileSize-pos<onceReadBytes){
                int holder= lseek(filedesc, 0, SEEK_SET);
                if(holder<0){
                    printf("Error: descriptor error\n");
                    return ;
                }
            }
            
        }
        close(filedesc);
        free(buffer);
        // printf("size = %f, cycles = %lf average read I/O time=%lf\n",pow((float)2,i), totalNano/iteraLimit,totalNano/iteraLimit/pow((float)2,i));
        printf("size = %fMB, read I/O time=%lf\n",pow((float)2,i), totalNano/iteraLimit);
        file++;
    }
    
    
}
void getFileRanRead(char* filePath,int  onceReadMB,int iteraLimit){
    uint64_t start;
    uint64_t end;
    
    char fileName[10] = ".tmp";
    int file=0;
    //==============function warmup====
    char fileFullPath[200]= {0};
    strcat(fileFullPath, filePath);
    strcat(fileFullPath, "0.tmp");
    int filedesc = open(fileFullPath, O_RDONLY);
    char* buffer=(char*)malloc(1000);
    for(int i=0;i<=1000;i++){
        ssize_t bytesRead=read(filedesc,buffer,1);
    }
    free(buffer);
    lseek(filedesc, 0, SEEK_SET);
    close(filedesc);
    //==============function warmup====
    
    for(int i=0; i<=10;i++){// i different size of file
        char fileFullPath[200]= {0};
        char numBuff[20];
        sprintf(numBuff, "%d", file);
        strcat(fileFullPath, filePath);
        strcat(fileFullPath,numBuff);
        strcat(fileFullPath, fileName);
        
        
        int onceReadByte=onceReadMB*1024*1024;
        int fileSize=1024*1024*pow((float)2,i);
        int totalStep = fileSize/onceReadByte;
        char* buffer=(char*)malloc(onceReadByte);
        //  printf("file path is:\n %s\n",fileFullPath);
        // printf("start to read the file size= %d \n ",fileSize);
        double  totalNano = 0.0;
        
        int filedesc = open(fileFullPath, O_RDONLY );
        fcntl( filedesc,F_NOCACHE);
        srand(time(NULL));
        for(int itera=0;itera<iteraLimit;itera++){
            int pos= abs((rand()*RAND_MAX + rand()))%totalStep;
            lseek(filedesc, pos*onceReadByte, SEEK_SET);
            start = rdtsc();
            ssize_t bytesRead=read(filedesc,buffer,onceReadByte);
            end = rdtsc();
            if(bytesRead<=0) {
                printf("Error: no read file\n");
                return ;
            }
            totalNano+=end-start;
            //printf("%d\n",pos);
            //printf("%s\n",buffer);
            //  printf("%llu\n", end-start);
            
        }
        close(filedesc);
        free(buffer);
        // printf("size = %f, cycles = %lf average read I/O time=%lf\n",pow((float)2,i), totalNano/iteraLimit,totalNano/iteraLimit/pow((float)2,i));
        printf("size = %fMB, read I/O time=%lf\n",pow((float)2,i), totalNano/iteraLimit);
        file++;
    }
    
    
}




int main(int argc, const char * argv[]) {
    /* Use this function to create file larger than 1 G*/
     //createCharFile("/Users/justin01031/UCSD/2016winter/CSE221/CSE221project/filesystem/TestFile/11.tmp", 1024*2);
     //createCharFile("/Users/justin01031/UCSD/2016winter/CSE221/CSE221project/filesystem/TestFile/12.tmp", 1024*4);
    
    //sizeOfFileCache
    //sizeOfFileCache("/Users/justin01031/UCSD/2016winter/CSE221/CSE221project/filesystem/TestFile/",100);
    //sizeOfFileCacheSpecBaseSize("/Users/justin01031/UCSD/2016winter/CSE221/CSE221project/filesystem/TestFile/",512,10000);
    
    //File read time:
    //getFileSeqRead("/Users/justin01031/UCSD/2016winter/CSE221/CSE221project/filesystem/TestFile/",1,10000);
    //getFileRanRead("/Users/justin01031/UCSD/2016winter/CSE221/CSE221project/filesystem/TestFile/",1,10000);
    
    //Remote File Read time
    //need to map to remote server
    
    //
    
    //contention
    return 0;
}

