#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <readline/history.h>

#define Number_Of_Builtins 4

int shell_cd(char** parsed_values){
	if (parsed_values[1]==NULL){
		printf("Path is missing\n");
	}
	else {
		if (chdir(parsed_values[1])>0){
			perror("Error due to chdir()");
		}
	}
	return 1;
}
int shell_support(char** parsed_values){
	if (strcmp(parsed_values[0],"support")==0){
		printf("\t\tWelcome to Shell Support\n ");
		printf("Supported Commands\n1. cd \n2. help \n3. exit \n");
	}
	return 1;
}
int shell_exit(char** parsed_values){	
	if (strcmp(parsed_values[0],"exit")==0){
		return 0;
	}
	return 1;
}

int shell_history(char** parsed_values){
	HISTORY_STATE *myhist = history_get_history_state ();
    HIST_ENTRY **mylist = history_list ();
	int i;
    for (i=0;i<myhist->length;i++) { 
        printf (" %8s\n", mylist[i]->line);   
    }
	return 1;
}

char** tokenizing(char *entered_value){
	int mem_space=32;
	int current_position=0;
	char** array_of_token = malloc(mem_space*sizeof(char*));		
	if (array_of_token==NULL){
		fprintf(stderr,"Allocation Error\n");
		exit(EXIT_SUCCESS);  
	}
	
	char* token=strtok(entered_value," \n\r\a\t");
	while(token!=NULL){
		array_of_token[current_position]=token;
		current_position++;
		
		if (current_position >= mem_space){
			mem_space=mem_space+32;			
			array_of_token=realloc(array_of_token,mem_space*sizeof(char*));
			if (!array_of_token){
				fprintf(stderr,"Allocation Error\n");
				exit(EXIT_FAILURE);
			}
		}
		
		token=strtok(NULL," \n\r\a\t");
	}
	array_of_token[current_position]=NULL;
	return array_of_token;
}
int executing(char **parsed_values){
	char* builtin_data[Number_Of_Builtins] = {"cd",  "support", "exit","history"};

	int i = 0;
	if(parsed_values[0] == NULL){
		return 1;
	}
	for(i=0;i<Number_Of_Builtins;i++){
		if(strcmp(parsed_values[0], builtin_data[i]) == 0){
			switch(i){
				case 0:
					return shell_cd(parsed_values);
				case 1:
					return shell_support(parsed_values);
				case 2:
					return shell_exit(parsed_values);
				case 3:
					return shell_history(parsed_values);
				default:
					printf("Command not found\n");
			}
		}
	}
	

	int pid;
	pid = fork();
	if (pid == 0){			
		if (execvp(parsed_values[0], parsed_values) == -1) {
     		 perror("Error Encountered to run this Program \n");
    	}
    	exit(EXIT_FAILURE);
	}
	else if (pid < 0){
		perror("Could not fork() \n");
	}
	else {
		wait(NULL);
	}
	return 1;
}


int main(){

	char cwd[1024];
	int mem_size=0;
	int i=0;
	char *input=NULL;
	char **array_of_tokens;
	bool can_run=1;

	while(can_run){
		getcwd(cwd,sizeof(cwd));
		printf("[ %s ]",cwd);
		printf(" >> ");
		
		if (getline(&input,&mem_size,stdin)==-1){
			if (feof(stdin)){
				exit(EXIT_SUCCESS);
			}
			else{
				exit(EXIT_FAILURE);
			}
		}
		
		add_history(input);
		
		array_of_tokens = tokenizing(input);
 		can_run = executing(array_of_tokens);	
		
    	free(input);
    	free(array_of_tokens);
    	input=NULL;
    	array_of_tokens=NULL;
  	}
	
	return 0;
}
