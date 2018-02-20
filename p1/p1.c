#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

typedef struct node{
	pid_t pid;
	char* cmd;
	struct node* next;
} b_node;

void add(b_node* head, pid_t pid, char* cmd, char* args) {
	
	size_t size = pathconf(".", _PC_PATH_MAX);
	char* fp;
	char* buffer;
	char init[size];
	
	if ((buffer = (char *)malloc((size_t)size)) != NULL)
		fp = getcwd(buffer, (size_t)size);
	
	b_node* curr = head;
	while(curr->next != NULL)
		curr = curr->next;
	
	curr->next = malloc(sizeof(b_node));
	curr = curr->next;
	curr->pid = pid;
	curr->cmd = (char*)malloc((size_t)size);
	realpath(cmd,curr->cmd);
	strcat(curr->cmd, args);
	curr->next = NULL;
}

void delete(b_node* head, pid_t pid) {
	b_node* curr = head;
	b_node* prev = NULL;
	while(curr->pid != pid) {
		prev = curr;
		curr = curr->next;
	}
	printf("%d: %s has terminated.\n", curr->pid, curr->cmd);
	if(curr->next == NULL) {
		free(curr);
		prev->next = NULL;
	} else {
		prev->next = curr->next;
		free(curr);
	}
}

void list(b_node* head) {
	b_node* curr = head->next;
	int num = 0;
	printf("\n");
	while(curr != NULL) {
		printf("%d: %s\n", curr->pid,curr->cmd);
		curr = curr->next;
		num++;
	}
	printf("Total Background Jobs: %d\n\n", num);
}

void killProc(b_node* head) { // FIX THIS
	b_node* curr = head->next;
	b_node* prev = NULL;
	
	while(curr != NULL) {
		kill(curr->pid, SIGKILL);
		prev = curr;
		curr = curr->next;
		free(prev);
	}
}

int getPrompt(char* name, size_t size){
	// free buffer?
	char* fp;
	char* buffer;
	char init[size];
	memset(name, 0, size);
	memset(init, 0, size);
	strcat(init, "SSI: ");
	
	if ((buffer = (char *)malloc((size_t)size)) != NULL)
		fp = getcwd(buffer, (size_t)size);
	
	strncpy(name, fp, size);
	strcat(init, name);
	strncpy(name, init, size);
	strcat(name, " > ");
	name[size - 1] = 0;
	return 0;
}

int getCommandLength(char* input) {
	int len = 0;
	int st = strlen(input);
	char* cmd = (char*)malloc(st);
	// parse length of first word
	while(*input != 32 && st > len) {
		input++;
		len++;
	}
	
	return len;
	
}

int spaces(char** in) {
	//removes spaces
	int num = 0;
	while(**in == 32) {
		(*in)++;
		num++;
	}
	return num;
}

int changeDir(char* dir) {
	// make it handle empty string (just cd as input)
	spaces(&dir);
	if(!strcmp(dir, "~") || !strcmp(dir, ""))
		return chdir(getenv("HOME"));
	
	size_t size = pathconf(".", _PC_PATH_MAX);
	char* fp;
	char* buffer;
	char init[size];
	
	if ((buffer = (char *)malloc((size_t)size)) != NULL)
		fp = getcwd(buffer, (size_t)size);
	//printf("%s\n", getenv());
	
	return chdir(dir);
}

int main() {
	
	long size = pathconf(".", _PC_PATH_MAX);
	char prompt[size];
	int processes[0];
	int* status;
	b_node* head = NULL;
	head = malloc(sizeof(b_node));
	if(head == NULL)
		return 1;
	head->pid = 0;
	head->cmd = malloc(sizeof("head"));
	head->cmd = "head";
	head->next = NULL;
	
	getPrompt(prompt, size);
	

	int bailout = 0;
	while (!bailout) {
		int dist = 0;
		char* reply = readline(prompt);
		//check processes here
		int id = waitpid(-1, NULL, WNOHANG);
		//printf("%d\n",id);
		while(id > 0) {
			delete(head, id);
			id = waitpid(-1, NULL, WNOHANG);
		}
		
		//remove leading spaces
		dist = spaces(&reply);
		int len = getCommandLength(reply);
		char* cmd = (char*)malloc(len);
		strncpy(cmd, reply, len);
		cmd[len] = 0;
		reply += len;
		dist += len;
		
		if (!strcmp(cmd, "exit") || !strcmp(cmd, "quit")) {
			killProc(head);
			bailout = 1;
		} else if(!strcmp(cmd, "cd")) {
			// code to change directory
			dist += spaces(&reply);
			if(changeDir(reply))
				printf("directory DNE\n");
			else {
				getPrompt(prompt, size);
			}
		} else if(!strcmp(cmd, "bglist")) {
			list(head);
		} else {
			int bgFlag = 0;
			int i;
			char* args[size];
			// if bg is used, a background process is made
			if(!strcmp(cmd, "bg")) {
				bgFlag = 1;
				free(cmd);
				dist += spaces(&reply);
				len = getCommandLength(reply);
				cmd = (char*)malloc(len);
				strncpy(cmd, reply, len);
				cmd[len] = 0;
				reply += len;
				dist += len;
			}
			args[0] = cmd;
			for(i = 1; strlen(reply) > 0; i++) {
				// remove spaces, and get length of next word
				dist += spaces(&reply);
				len = getCommandLength(reply);
				if(len != 0) {
					args[i] = malloc(len);
					args[i][len] = 0;
					strncpy(args[i], reply, len);
				}
				// shift the pointer, and save the distance
				reply += len;
				dist += len;
			}
			
			pid_t child = fork();
			
			if(bgFlag) {
				if(child != 0) { // last argument is the args of the command
					add(head, child, cmd, reply-dist+3+strlen(cmd));
					waitpid(WNOHANG);
					usleep(15000);
				}
				else if(child == 0) {
					//setpgid(0,0);
					if(execvp(args[0], args) == -1) return;
				}
			} else {
				if(child != 0)
					wait(NULL);
				else if(child == 0) // 0 means child process
					if(execvp(args[0], args) == -1) return;
			}
			memset(args, 0, size);
		}
		reply -= dist;
		free(cmd);
		free(reply);
	}
	printf("Leaving shell...\n");
}
