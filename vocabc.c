#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char **argv) {

	char *fvalue = NULL;
	int CHAR;
	opterr = 0;
	
	FILE *vocabfile;
	char lang1_word[40];
	char lang2_word[40];
	char source_str[40];
	char newstr[40];
	char token[] = "=\n";
	char input_str[40];
	char line[40];
	int pairs = 0;
	int pair = 1;

	while ((CHAR = getopt (argc, argv, "hf:")) != -1) {
		switch (CHAR) {
          		case 'h':
            			printf("\nUse:\tvocabc -f <file>\n");
				return EXIT_FAILURE;	
            		case 'f':
				fvalue = optarg;
             			break;
           		case '?':
             			if (optopt == 'f')
               				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
             			else if (isprint (optopt))
               				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
             			else
               				fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
             			return EXIT_FAILURE;
           		default:
             			return EXIT_FAILURE;
           	}
	}
	vocabfile = fopen(fvalue,"r");
	if (NULL == vocabfile) {
		printf("\nError in Opening...\n");
		return EXIT_FAILURE;
	}
	while((fscanf(vocabfile,"%s\n",line)) != EOF) {
		pairs++;
	}
	printf("Word pairs: %d\n",pairs);
	fseek(vocabfile,0L, SEEK_SET);
	while ((fscanf(vocabfile,"%s\n",source_str)) != EOF ) {
		strcpy(newstr,strtok(source_str,token));
		strcpy(lang1_word,newstr);
		strcpy(newstr,strtok(NULL,token));
		strcpy(lang2_word,newstr);
		printf("(%d/%d)\t%s ?\n",pair,pairs,lang1_word);
		scanf("%s",input_str);
		if (strcmp(lang2_word, input_str) == 0) {
			printf("Right!\n");
		} else {
			printf("Wrong!\n");
		}
		pair++;
	}
	fclose(vocabfile);
       	return EXIT_SUCCESS;
     }

