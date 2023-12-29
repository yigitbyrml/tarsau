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
            break;
        }

        FILE *inputfs;
        inputfs = fopen(argv[i],"r");
        fseek(inputfs,0,SEEK_END);
        int file_size = ftell(inputfs);
        fseek(inputfs,0,SEEK_SET);

        char record[256];
        sprintf(record, "|%s,%o,%d|", argv[i], 0644, file_size); 

        fwrite(record, 1, strlen(record), outputfs);

        if(i==argc-1){
            fprintf(outputfs,"\n");
        }
        fclose(inputfs);

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

            if(ch_size > 127){
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

void tarsau_a(char *archive_file, char *output_directory) {
    FILE *in_fp = fopen(archive_file, "r");
    if (in_fp == NULL) {
        printf("Archive file is inappropriate or corrupt!\n");
        return;
    }

    int filelen = strlen(archive_file);
    if(filelen < 4 || strcmp(archive_file + filelen - 4, ".sau") != 0){
        printf("Archive file is inappropriate or corrupt!\n");
        return;
    }
    fclose(in_fp);

    mkdir(output_directory, 0777);

    FILE *archive_fp = fopen(archive_file, "r");
    if (archive_fp == NULL) {
        printf("Error opening archive file.\n");
        return;
    }
    int toplam=0;
    
    char created_files[32][1000];
    int total_files=0;

    char file_list[200];
    fread(file_list, 1, sizeof(file_list), archive_fp);
    fclose(archive_fp);

    char *file_data = strchr(file_list, '|');
    while (file_data != NULL) {
        file_data++; 

        char *file_info_end = strchr(file_data, '|');
        if (file_info_end != NULL) {
            *file_info_end = '\0'; 

            char *file_name = strtok(file_data, ",");
            char *file_perm = strtok(NULL, ",");
            char *file_content = strtok(NULL, "|");

            if (file_name != NULL && file_perm != NULL && file_content != NULL) {
                char output_path[256];
                snprintf(output_path, sizeof(output_path), "%s/%s", output_directory, file_name);

                FILE *out_fp = fopen(output_path, "w");
                if (out_fp == NULL) {
                    printf("Error creating file: %s\n", file_name);
                    return;
                }
                int size = atoi(file_content);

                FILE *in_fp1 = fopen(archive_file, "r");
                char file_list1[200];
                fread(file_list1, 1, sizeof(file_list1), in_fp1); 
                int i=toplam;
                if(i == 0){
                    while(file_list1[i] != '\n'){
                        i++;
                    }
                    i++;
                    toplam=i;
                }

                for(;i<size+toplam;i++){
                    fprintf(out_fp,"%c",file_list1[i]);
                }
                toplam += size;
                fclose(in_fp1);

                *file_info_end = '|'; 
                file_data = strchr(file_info_end + 1, '|'); 
                strcpy(created_files[total_files],file_name);
                total_files++;
                
            } 
            else {
                printf("Invalid file format: %s\n", file_data);
            }
        } else {
            printf("Invalid file format: '|' separator not found.\n");
            break;
        }
    }

    for(int i=0;i<total_files;++i){
        printf("%s ",created_files[i]);
    }

    printf("Files opened in the directory: %s\n", output_directory);
}

int main(int argc, char *argv[]) {
    if((strcmp(argv[1],"-b") && strcmp(argv[1],"-a")) != 0){
        printf("You must use tarsau -b input files or tarsau -a input files\n");
        return 0;
    }


    if(argc<3){
        printf("You must input more than 2\n");
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
        printf("ERROR Tarsau -b : The number of input files cannot be more than 32\n");
        return 0;
        }
        tarsau_b(argc,argv,outputf);
    }
    else if(strcmp(argv[1],"-a")==0){
        if(argc >4){
            printf("ERROR Tarsau -a : The number of input files cannot be more than 4\n");
            return 0;
        }
        tarsau_a(archive_file,output_directory);
    }

    return 1;
}

