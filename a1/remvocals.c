#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	if (argc < 2){
		fprintf(stderr, "Usage: %s <sourcewav> <destwav>\n", argv[0]);
		exit(1);
	}

	FILE *wav1 = fopen(argv[1], "r");
	FILE *wav2 = fopen(argv[2], "w");

	if (wav1 == 0){
		perror(argv[1]);
		exit(1);
	}
	if (wav2 == 0){
		perror(argv[2]);
		exit(1);
	}

	char buf[44];

	/*
	Copies the first 44 bytes
	*/
	fread(&buf, sizeof(char), 44, wav1);
	fwrite(&buf, sizeof(char), 44, wav2);

	fseek(wav1, 44, SEEK_SET);
	fseek(wav2, 44, SEEK_SET);
	while (1){


		short left;
		short right;
		fread(&left, sizeof(short), 1, wav1);
		fread(&right, sizeof(short), 1, wav1);

		if (feof(wav1)){
			break;
		}

		short combined = (left-right)/2;
		fwrite(&combined, sizeof(short), 1, wav2);
		fwrite(&combined, sizeof(short), 1, wav2);
	}

	fclose(wav1);
	fclose(wav2);
	return 0;
}