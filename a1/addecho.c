#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>

#define HEADER_SIZE 22

int main(int argc, char *argv[]){

    int i = 0;
    int opt = 0;

    short read_count = 0;
    unsigned short delay = 8000, volume = 4;

    FILE *sourcewav = fopen(argv[argc-2], "r");
    FILE *destwav = fopen(argv[argc-1], "w");

    if (argc < 2){
        fprintf(stderr, "Usage: %s <sourcewav> <destwav> \n", argv[0]);
        exit(1);
    }

    //Check if file is NULL
    if (sourcewav == NULL || destwav == NULL){ 
        if (sourcewav == NULL){
            perror(argv[1]);
        }else{
            perror(argv[2]);
        }
        exit(1);
    }

    while ((opt = getopt(argc, argv, "d:v:")) != -1){
        switch (opt) {
        case 'd' : 
            delay = atoi(optarg);
            break;
        case 'v' : 
            volume = atoi(optarg);
            break;
        default: 
            fprintf(stderr, "Usage: %s [-d delay] [-v volume] <sourcewav> <destwav>\n", argv[0]);
            exit(-1);
        }
    }

    //Filesize
    fseek(sourcewav, 0L, SEEK_END);
    unsigned short file_size = ftell(sourcewav);
    fseek(sourcewav, 0L, SEEK_SET);


    //Read header and change size 
    short header[HEADER_SIZE];
    unsigned int *sizeptr;

    fread(&header, sizeof(short), 22, sourcewav);

    sizeptr = (unsigned int *)(header + 2);
    *sizeptr = *sizeptr + (delay * 2);
    sizeptr = (unsigned int *)(header + 20);
    *sizeptr = *sizeptr + (delay * 2);

    fwrite(&header, sizeof(short), 22, destwav);

    short orig[delay];
    short echo_buf[delay];
    short mix[delay];

    //Writes while sample is not at delay
    fread(&echo_buf, sizeof(short), delay, sourcewav);
    fwrite(&echo_buf, sizeof(short), delay, destwav);

    //Divide by volume
    for (i = 0; i < delay; i++){
        echo_buf[i] = echo_buf[i]/volume;
    }

    while(1){
         //Mix echo buffer with original file
        read_count = fread(&orig, sizeof(short), delay, sourcewav);

         if (read_count < delay){
             for (i = 0; i < read_count; i++){
                //Add to mix
                mix[i] = echo_buf[i] + orig[i];
            }
            fwrite(&mix, sizeof(short), read_count, destwav);
            break;
        }

        for (i = 0; i < delay; i++){
            mix[i] = echo_buf[i] + orig[i]; 
            //Input into echo buffer for next samples
            echo_buf[i] = orig[i];
        }

        //Write to file
        fwrite(&mix, sizeof(short), delay, destwav);
    
        for (i = 0; i < delay; i++){
            echo_buf[i] = echo_buf[i]/volume;
        }
    }

    //Writing x 0 samples
    short x = 0;
    short x_delays = 0;
    fseek(sourcewav, 0L, SEEK_END);
    short total_samples =  (file_size-44)/2;

    x = delay - total_samples;

    if (x > 0){
        for (i = 0; i < x; i++){
            fwrite(&x_delays,2,1,destwav);
        }
    }else{
        //Do nothing
    }

    fwrite(&echo_buf,sizeof(short), delay, destwav);

    return 0;
}
