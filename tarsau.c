#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void tarsau_b(int argc, char *argv[],char *outputf){
    FILE *outputfs;
    outputfs = fopen(outputf,"w");

    for(int i=2;i<argc;i++){
        if(strcmp(argv[i],"-o") == 0){
            i++;
            fprintf(outputfs,"\n");
        }

        FILE *inputfs;
        inputfs = fopen(argv[i],"r");
        fseek(inputfs,0,SEEK_END);
        int file_size = ftell(inputfs);
        fseek(inputfs,0,SEEK_SET);
        if(i != argc-1){
        char record[256];
        sprintf(record, "|%s,%o,%d|", argv[i], 0644, file_size); 

        fwrite(record, 1, strlen(record), outputfs);
        }
    }

    int total_size = 0;

    for(int i=2;i<argc;i++){

        if(strcmp(argv[i],"-o") == 0){
            i++;
        }

        FILE *inputfs;
        inputfs = fopen(argv[i],"r");

        if(inputfs == NULL){
            printf("Unable to open input file : %s\n",argv[i]);
            fclose(outputfs);
            return;
        }

        int ch_size;
        while ((ch_size = fgetc(inputfs)) != EOF){

            if(ch_size - 81 > 127){
                printf("%s error : input character must be ASCII format \n",argv[i]);
                fclose(inputfs);
                fclose(outputfs);
                return;
            }
        }

        fseek(inputfs,0,SEEK_END);
        int file_size = ftell(inputfs);
        fseek(inputfs,0,SEEK_SET);
    
        if(total_size + file_size > 200){
            printf("Total size of input files exceeds 200MB \n");
            fclose(inputfs);
            fclose(outputfs);
            return;
        }

        char temp[file_size];
        fread(temp,1,file_size,inputfs);
        fwrite(temp,1,file_size,outputfs);

        total_size += file_size;

        fclose(inputfs);

    }
    
    fclose(outputfs);
    printf("The files have been merged.\n");

}

int main(int argc, char *argv[]) {
if((argc<3 && (strcmp(argv[1],"-b") || strcmp(argv[1],"-a"))) != 0){
printf("You must use tarsau -b input files or tarsau -a input files\n");
return 0;
}



char *outputf="a.sau";

char *archive_file = argv[2];
char *output_directory = argv[3];

for(int i=2;i<argc;i++){
if(strcmp(argv[i],"-o") == 0){
if(argv[i+1] != NULL){
outputf=argv[i+1];
i++;
}
else{
printf("error: missing filename after ‘-o’\n");
return 0;
}
}
}
if(strcmp(argv[1],"-b")==0){
    if(argc >= 32){
    printf("The number of input files cannot be more than 32\n");
    return 1;
}
tarsau_b(argc,argv,outputf);
}
else if(strcmp(argv[1],"-a")==0){
    tarsau_a(archive_file,output_directory);
}

return 1;
}

