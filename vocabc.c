// #####	VocabC v1.6	##### //

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#define MAX_WORDS 5
#define MAX_TRIES 2
#define MAX_LENGTH 200

FILE *vocabfile;
FILE *sourcefile;

char *errors[] = {"VocabC requires argument -f <file>","Error in opening vocabulary file","too high argument of option -n","Error in reading vocabulary file",
			"Error in input","Error in deleting temporary file","Unable to clear screen","Internal Error"};

int Error(int error) {
	printf("Error %d - %s!\n",error,errors[error]);
	if (remove("vocab.tmp") < 0) {
		fclose(vocabfile);
		remove("vocab.tmp");
	}
	exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {
	//variables for getopt
	char *fvalue = NULL;
	char *dvalue = "1";
	char *nvalue = "all";
	int rvalue = 0;
	int cvalue = 0;
	int CHAR;
	opterr = 0;
	//variables for query and output
	char buffer[MAX_LENGTH], source_str[MAX_LENGTH], input_str[MAX_LENGTH];
	char comm_str[MAX_LENGTH], lang1_comm[MAX_LENGTH], lang2_comm[MAX_LENGTH];
	char lang1_word[MAX_LENGTH], lang1_str[MAX_LENGTH], lang2_str[MAX_LENGTH];
	char line[MAX_LENGTH], temp[MAX_LENGTH], temp_word[MAX_LENGTH], lang2_temp_str[MAX_LENGTH];
	char lang2_word[MAX_WORDS][MAX_LENGTH];
	char *ptr;
	//different tokens for dividing strings
	char lang_token[] = "=\n";
	char word_token[] = ",";
	char comm_token[] = "#";
	int pairs = 0, pair = 1;
	int lines = 0;
	int word = 0;
	//loop variables
	int i,j;
	int k = 0;
	int is_giv;
	int index_a, index_b, temp_rand;
	int right = 0, correct = 0, tries = 0;
	char direction[2] = "1";
	float percent;
	//variables for bar
	float bar_num;
	float bar_loop;

	if (argc < 2) {
		Error(0);
	}
	while ((CHAR = getopt (argc, argv, "hrf:d:n:c")) != -1) {
		switch (CHAR) {
          		case 'h':
				printf("\nVocabC v1.6\n");
            			printf("\nUse:\tVocabC -f <file>\n");
				printf("\nOptional arguments:\n-h\tShow this help\n-r\tRandomize the order of the words\n");
				printf("-d1\tThe program asks the first word\n-d2\tThe program asks the second word\n-dr\tthe program asks randomly\n");
				printf("-n <num>\tAsk only <num> words\n");
				printf("-c\tCase sensitive\n");
				return EXIT_FAILURE;	
            		case 'f':
				fvalue = optarg;
             			break;
			case 'r':
				rvalue = 1;
				break;
			case 'd':
				dvalue = optarg;
				break;
			case 'n':
				nvalue = optarg;
				break;
			case 'c':
				cvalue = 1;
				break;
           		case '?':
             			if (optopt == 'f' || optopt == 'd') {
               				fprintf (stderr, "Error - Option -%c requires an argument.\n", optopt);
					return EXIT_FAILURE;
             			} else if (isprint (optopt)) {
               				fprintf (stderr, "Error - Unknown option `-%c'.\n", optopt);
					return EXIT_FAILURE;
             			} else {
               				fprintf (stderr,"Error - Unknown option character `\\x%x'.\n",optopt);
             				return EXIT_FAILURE;
				}
           		default:
             			return EXIT_FAILURE;
           	}
	}
	//Clear screen
	if (system("clear") == -1) {
		Error(6);
	}
	//Open vocabulary file
	sourcefile = fopen(fvalue,"r");
	vocabfile = fopen("vocab.tmp","w");
	if (NULL == vocabfile || NULL == sourcefile) {
		Error(1);
	}
	//copy lines from source to temporary file
	while((fgets(buffer, sizeof(buffer), sourcefile)) != NULL) {
		if (buffer[0] != '#') {
			fputs(buffer, vocabfile);
		}
	}
	//reopen temporary file to read lines
	if (freopen("vocab.tmp","r",vocabfile) == NULL) {
		Error(7);
	}
	//Count lines
	while ((fgets(line,MAX_LENGTH,vocabfile)) != NULL) {
		lines++;
	}
	srand(time(NULL));
	int rand_lines[lines-1];
	//fill array with line numbers
	for(i = 0; i < lines; i++) {
		rand_lines[i] = i+1;
	}
	//if -r is set, shuffle array
	if (rvalue == 1) {
		for (i = 0; i < 5000; i++) {
			index_a = rand() % lines;
			index_b = rand() % lines;
			temp_rand = rand_lines[index_a];
			rand_lines[index_a] = rand_lines[index_b];
			rand_lines[index_b] = temp_rand;
		}
	}
	//How many words should be asked?
		//ask all pairs
	if (strcmp(nvalue, "all") == 0) {
		pairs = lines;
	} else {
		//ask only -n <num> words
		pairs = atoi(nvalue);
	}
	if (pairs > lines) {
		Error(2);
	}
	printf("Word pairs: %d\n",pairs);
	//main loop with query
	for (i = 0; i < pairs; i++) {
		strcpy(comm_str,"NULL");
		strcpy(lang1_comm,"NULL");
		strcpy(lang2_comm,"NULL");
		strcpy(lang2_temp_str,"NULL");
		strcpy(temp_word,"NULL");
		strcpy(lang1_word,"NULL");
		for (j = 0; j < MAX_WORDS; j++) {
			strcpy(lang2_word[j],"NULL");
		}
		fseek(vocabfile,0L,SEEK_SET);
		word = 0;
		//read lines until line i
		for (j = 0; j < rand_lines[i]-1; j++) {
			if (fgets(temp, MAX_LENGTH, vocabfile) == NULL) 
				Error(3);
		}
		//read line i
		if (fgets(source_str, MAX_LENGTH, vocabfile) == NULL)
			Error(3);
		//divide into lang1 and lang2
		ptr = strtok(source_str,lang_token);
		strcpy(lang1_str,ptr);
		ptr = strtok(NULL,lang_token);
		strcpy(lang2_str,ptr);
		//divide lang1 into word and comment
		ptr = strtok(lang1_str,comm_token);
		strcpy(lang1_word,ptr);
		while (ptr != NULL) {
			ptr = strtok(NULL,comm_token);
			if (ptr != NULL) {
				strcpy(lang1_comm,ptr);
			}
		}
		//divide lang2 into words and comment
		ptr = strtok(lang2_str,comm_token);
		strcpy(lang2_temp_str,ptr);
		while (ptr != NULL) {
			ptr = strtok(NULL,comm_token);
			if (ptr != NULL) {
				strcpy(lang2_comm,ptr);
			}
		}
		//save words of second language in lang2_word[]
		ptr = strtok(lang2_temp_str,word_token);
		strcpy(lang2_word[0],ptr);
		word = 1;
		while (ptr != NULL) {
			ptr = strtok(NULL,word_token);
			if (ptr != NULL) {
				strcpy(lang2_word[word],ptr);
			}
			word++;
		}
		is_giv = 0;
		//is -dr set?
		if (strcmp(dvalue,"r") != 0) {
				strcpy(direction,dvalue);
		} else {
			if ((rand() % 2) != 0) {
				strcpy(direction,"2");
			} else {
				strcpy(direction,"1");
			}
		}
		//if direction is "2"
		if (strcmp(direction,"2") == 0) {
			//choose one meaning of a word
			k = rand() % MAX_WORDS;
			while (is_giv != 1) {
				k = rand() % MAX_WORDS;
				if (strcmp(lang2_word[k],"NULL") != 0) {
					is_giv = 1;
				}
			}
			//swap lang1_word and lang2_word
			strcpy(temp_word,lang1_word);
			strcpy(lang1_word,lang2_word[k]);
			strcpy(lang2_word[k],temp_word);
		}
		correct = 0;
		tries = 0;
		while (tries < MAX_TRIES && correct != 1) {
			tries++;
			//ask user a word
			printf("\n(%d/%d)\t[", pair, pairs);
			bar_num = (float) pair / (float) pairs * 10;
			for (bar_loop=1; bar_loop<=10; bar_loop++) {
				(bar_loop<=bar_num) ? printf("=") : printf(" ");
			}
			//is lang1_comm or lang2_comm not "NULL"?
			if ((strcmp(lang1_comm,"NULL") != 0) && (strcmp(direction,"1") == 0)) {
				strcpy(comm_str,lang1_comm);
			}
			if ((strcmp(lang2_comm,"NULL") != 0) && (strcmp(direction,"2") == 0)) {
				strcpy(comm_str,lang2_comm);
			}
			//ask for a word and print the comment (if one exists)
			if (strcmp(comm_str, "NULL") != 0) {
				printf("]  ?:  \"%s\"\t#: %s\n>>>  ",lang1_word,comm_str);
			} else {
				printf("]  ?:  \"%s\"\n>>>  ",lang1_word);
			}
			//user gives answer
			if(fgets(input_str, MAX_LENGTH, stdin) == NULL) {
				Error(4);
			}
			//removing '\n'
			input_str[strlen(input_str)-1] = '\0';
			//remove " " at the end of input
			while (input_str[strlen(input_str)-1] == ' ' || input_str[strlen(input_str)-1] == '\t') {
				input_str[strlen(input_str)-1] = '\0';
			}
			//remove " " at the end of word for which is asked
			while (lang1_word[strlen(lang1_word)-1] == ' ' || lang1_word[strlen(lang1_word)-1] == '\t') {
				lang1_word[strlen(lang1_word)-1] = '\0';
			}
			//remove " " at the beginning of input
			while (input_str[0] == ' ' || input_str[0] == '\t') {
				for (j = 0; j < strlen(input_str); j++) {
					input_str[j] = input_str[j+1];
				}
			}
			//remove " " at the beginning of word for which is asked
			while (lang1_word[0] == ' ' || lang1_word[0] == '\t') {
				for (j = 0; j < strlen(lang1_word); j++) {
					lang1_word[j] = lang1_word[j+1];
				}
			}
			//is any of the meanings equal to the user`s answer?
			if (strcmp(direction,"2") != 0) {
				for (j = 0; j < 5; j++) {
					if (cvalue == 1) {
						if (strcmp(lang2_word[j], input_str) == 0) {
							correct = 1;
						}
					} else {
						if (strcasecmp(lang2_word[j], input_str) == 0) {
							correct = 1;
						}
					}
				}
			} else {
				if (cvalue == 1) {
					if (strcmp(lang2_word[k], input_str) == 0) {
						correct = 1;
					}
				} else {
					if (strcasecmp(lang2_word[k], input_str) == 0) {
						correct = 1;
					}
				}
			}
			//display "correct" or "wrong"
			if (correct == 1) {
				printf("Correct!\n");
				right++;
			} else {
				printf("Wrong!");
			}
		}
		//too many tries? -> next word
		if (correct != 1) {
			if (strcmp(direction,"2") != 0) {
				printf("\tCorrect was: \"%s\"",lang2_word[0]);
			} else {
				printf("\tCorrect was: \"%s\"",lang2_word[k]);
			}
		}
		printf("\n");
		pair++;
	}
	percent = (float) right / (float) pairs * 100;
	printf("\nYou have known %g%% (%d/%d) of the words.\n\n",percent,right,pairs);
	//End of program, close files, remove temporary file
	fclose(vocabfile);
	fclose(sourcefile);
	if (remove("vocab.tmp") < 0) {
		Error(5);
	}
       	return EXIT_SUCCESS;
}
