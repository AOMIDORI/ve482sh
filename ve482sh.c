#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
//#include <sys/dir.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#define STDIN 0
#define STDOUT 1
#define LINESIZE 1024

/*===================================================*
 *               >,>>,                 *
 *===================================================*/
void Output2file(char *cm, int ifout){
	int i=0,j=0;	
	char *write_type="w+";
	char *program;
	char *file;
	if(ifout==2){
		write_type="a+";
		program=strtok(cm, ">>");
		file=strtok(NULL, ">>");
	}
	else{
		program=strtok(cm, ">");
		file=strtok(NULL, ">");
	}
	int l=strlen(file);
    for(i=0;i<l;i++){
		if(file[i]==' '){
			j++;
		}
		else{
			break;
		}
	}
	for(i=0;i<l;i++){
		file[i]=file[i+j];
	}

	char *args[50];
	i=0;
	args[0]=strtok(program, " ");
	while(args[i]!=NULL){
		i++;
		args[i]=strtok(NULL," ");
		if(args[i]!=NULL){
			if(args[i][0]=='"'){
				int ll=strlen(args[i]);
				if(args[i][ll-1]=='"'){
					memmove(&args[i][0],&args[i][1],ll);
					args[i][ll-2]=0;
				}
			}			
		}
	}
	pid_t pid=fork();
	if(pid < 0){
		fprintf(stderr, "Fork failed.\n");
	}
	else if(pid==0){
		FILE *output_file=fopen(file,write_type);	
		if(output_file==NULL){
			printf("Pipe failed.\n");
			return;
		}	
		dup2(fileno(output_file),STDOUT_FILENO);
		fclose(output_file);
		execvp(args[0],args);
		perror("Error:");
	}
	else{
		wait(NULL);
	}

}
/*===================================================*
 *                   <              
 *===================================================*/
 void Input2file(char *cm){
 	char *substring="<";
 	if(strstr(cm,"< ")!=NULL){
 		substring="< ";
 	}
 	char *program=strtok(cm, substring);
 	char *file=strtok(NULL,substring);
 	char *args[50];
	int i=0;
	if(file==NULL){
		printf("bash: syntax error near unexpected token 'newline'\n");
		//break;
	}

	//printf("%s, %s\n",program,file);
	args[0]=strtok(program, " ");
	while(args[i]!=NULL){
		i++;
		args[i]=strtok(NULL," ");
		if(args[i]!=NULL){
			if(args[i][0]=='"'){
				int ll=strlen(args[i]);
				if(args[i][ll-1]=='"'){
					memmove(&args[i][0],&args[i][1],ll);
					args[i][ll-2]=0;
				}
			}			
		}
	}
 	pid_t pid=fork();
 	if(pid < 0){
		fprintf(stderr, "Fork failed.\n");
	}
	else if(pid==0){
		FILE *input_file=fopen(file,"r");
		dup2(fileno(input_file),STDIN_FILENO);
		fclose(input_file);
		execvp(args[0],args);
		perror("Error:");
	}
	else{
		wait(NULL);
	}
	waitpid(pid,NULL,0);
 }
 /*===================================================*
 *            If it contains >,>>,<
 *===================================================*/
 int if_out(char *cm){ //return 1 if >, return 2 if >>
 	int i=0,j=0;
 	while(cm[i]){
 		if(cm[i]=='>'){
 			j=1;
 			if(cm[i+1]=='>'){
 				j=2;
 			}
 			break;
 		}
 		i++;
 	}
 	return j;
 }
 int if_in(char *cm){
 	int i=0;
 	if(strstr(cm,"<")!=NULL){
 		i=1;
 	}
 	return i;
 }

 /*===================================================*
 *                  Remove all spaces  
 *===================================================*/
 char *rmspace(char *ch){
 	int i,j=0,l=strlen(ch);
 	for(i=0;i<l;i++){
 		if(ch[i]==' '){
 			j++;
 		}
 		else{
 			break;
 		}
 	}
 	for(i=0;i<l;i++){
		ch[i]=ch[i+j];
	}
	for(i=0;i<l-j;i++){
		if(ch[i]!=' '){
			i++;
		}
		if(ch[i]==' '){
			ch[i]='\0';
		}
	}
	return ch;
 }


 /*==================================================*
  *                      MAIN                         *
  *==================================================*/
int main(){
	char cmd[LINESIZE];
	int i=0,j=0,b=0;
	int cmdfull;
	int n_quote;
	while(strcmp(cmd, "exit")!=0){
		printf("ve482sh $> ");
		gets(cmd);
		if(strcmp(cmd,"exit")==0){
			return 0;
		}
		cmdfull=0;
		//if(cmd[0]!='\n'&&cmd[0]!=0){
		//	size=strlen(cmd);
		//}
		n_quote=0;
//------------------------------------------------------
//         Deal with quotes                  
//------------------------------------------------------
		if(cmd[0]!=0 && cmd[0]!='\n'){
			for(i=0;i<strlen(cmd);i++){
				if(cmd[i]==34&&n_quote!=1){ //34: double-quote
					n_quote=2;
					memmove(&cmd[i],&cmd[i+1],strlen(cmd)-i);
					for(j=i;j<strlen(cmd);j++){
						if(cmd[j]==34){
							cmdfull=1;
						}
					}
					while(cmdfull==0){
						printf("> ");
						char cmdadd[300];
						gets(cmdadd);
						for(b=0;b<300;b++){
							if(cmdadd[b]==34){
								cmdfull=1;
								break;
							}
						}
						b=0;
						char *r=malloc(strlen(cmd)+strlen(cmdadd)+1);
						strcpy(r,cmd);
						strcat(r,"\n");
						strcat(r,cmdadd);
						strcpy(cmd,r);
						free(r);
					}
				}
				else if(cmd[i]==39&&n_quote!=2){ //39: single quote
					n_quote=1;
					memmove(&cmd[i],&cmd[i+1],strlen(cmd)-i);
					for(j=i;j<strlen(cmd);j++){
						if(cmd[j]==39){
							cmdfull=1;
						}
					}
					while(cmdfull==0){
						printf("> ");
						char cmdadd[300];
						gets(cmdadd);
						for(b=0;b<300;b++){
							if(cmdadd[b]==39){
								cmdfull=1;
								break;
							}
						}
						b=0;
						char *r=malloc(strlen(cmd)+strlen(cmdadd)+1);
						strcpy(r,cmd);
						strcat(r,"\n");
						strcat(r,cmdadd);
						strcpy(cmd,r);
						free(r);
					}
				}
				else if(cmd[i]=='>'){
					for(j=i+1;i<strlen(cmd);j++){
						if(!cmd[j]){
							break;
						}
						if(cmd[j]>30&&cmd[j]<126){
							cmdfull=1;
							break;
						}					
					}
					while(cmdfull==0){
						printf("> ");
						char cmdadd[300];
						gets(cmdadd);
						for(b=0;b<300;b++){
							if(cmdadd[b]!=32&&cmdadd[b]!=0&&cmdadd[j]!='\n'){
								cmdfull=1;
								break;
							}
							if(cmdadd[b]==0){
								break;
							}
						}
						b=0;
						char *r=malloc(strlen(cmd)+strlen(cmdadd)+1);
						strcpy(r,cmd);
						strcat(r,cmdadd);
						strcpy(cmd,r);
						free(r);
					}
				}
			}
		

		char *cm[2];
		//char *substring="|";
		char *arglis[100];
		char *arglis2[100];
		cm[0]=strtok(cmd,"|");
		cm[1]=strtok(NULL,"|");
		//printf("%s,%s\n",cm[0],cm[1] );
		i=0;
		j=0;
		if(cm[1]!=NULL){
			while(cm[1][i]!='\0'&& cm[1][i]!='\n'){
				if(cm[1][i]==' '){
					j++;            
				}
				else if(cm[1][i]=='x'){
					if(cm[1][i+1]=='a' && cm[1][i+2]=='r' && cm[1][i+3]=='g' && cm[1][i+4]=='s'){
						j=j+5;
					}
					if(cm[1][i+5]==' '){
						j++;
					}
				}
				else{
					break;
				}
				i++;
			}
			i=0;
			while(cm[1][i]){
				cm[1][i]=cm[1][i+j];
				i++;
			}		
		}
		i=0;
		
/*================If there is no pipe.================*/
		if(!cm[1]){
			//printf("ifin, ifout %s: %i,%i\n",cm[0],if_in(cm[0]),if_out(cm[0]));
			if(if_out(cm[0])!=0){
				Output2file(cm[0],if_out(cm[0]));
			}
			else if(if_in(cm[0])==1){
				Input2file(cm[0]);
			}
			else{
				arglis[0]=strtok(cm[0], " ");
				while(arglis[i]!=NULL){
					i++;
					arglis[i]=strtok(NULL," ");
					if(arglis[i]!=NULL){
						if(arglis[i][0]=='"'){
							int ll=strlen(arglis[i]);
							if(arglis[i][ll-1]=='"'){
								memmove(&arglis[i][0],&arglis[i][1],ll);
								arglis[i][ll-2]=0;
							}
						}			
					}
				}
		    //--------------------------------------------
            //         Deal with cd
            //---------------------------------------------
				int result;
				if(strcmp(arglis[0],"cd")==0){
					if(arglis[1]){
						if(strcmp(arglis[1],"~")==0){
							result=chdir(getenv("HOME"));
						}
						else{
							result=chdir(arglis[1]);
						}
					}
					else{
						result=chdir(getenv("HOME"));
					}
					if(result<0){
						printf("cd: No such file or directory\n");
					}
				}
				else{
					pid_t pid;
					pid = fork();
					if(pid<0){
					     fprintf(stderr, "Fork failed.\n");
		 			    return 1;
					}
					else if(pid==0){
						execvp(arglis[0],arglis);
						perror("Error");
					}
					else{
						wait(NULL);
					}
				}
			}
		}
/*================If there is a pipe.================*/		
		else{
			char *program1=cm[0], *program2=cm[1];
			char *file1, *file2;
			char *write_type2="w+";
			int type1=0,type2=0;
			file1=NULL;
			file2=NULL;
			if(if_out(cm[1])!=0){
				type2=1;
				if(if_out(cm[1])==1){
					program2=strtok(cm[1],">");
					file2=strtok(NULL,">");
				}
				else{
					program2=strtok(cm[1],">>");
					file2=strtok(NULL,">>");
					write_type2="a+";
				}
			}		
			if(if_in(cm[0])==1){
				type1=2;
				program1=strtok(cm[0],"<");
				file1=strtok(NULL,"<");
				if(file1==NULL){
					printf("bash: syntax error near unexpected token 'newline'\n");
				}	
			}

			if(file2!=NULL){
		     	file2=rmspace(file2);
			}
			if(file1!=NULL){
				file1=rmspace(file1);
			}
			//printf("program1, program2: %s,%s \n",program1,program2);
			//printf("file1, file2:%s,%s\n",file1,file2 );
			//printf("type1,2 %i,%i\n", type1,type2);
			i=0;
			arglis[0]=strtok(program1, " ");
			while(arglis[i]!=NULL){
				i++;
				arglis[i]=strtok(NULL," ");
				if(arglis[i]!=NULL){
					if(arglis[i][0]=='"'){
						int ll=strlen(arglis[i]);
						if(arglis[i][ll-1]=='"'){
							memmove(&arglis[i][0],&arglis[i][1],ll);
							arglis[i][ll-2]=0;
						}
					}			
				}
			}
			i=0;			
	     	arglis2[0]=strtok(program2," ");
			while(arglis2[i]!=NULL){
				i++;
				arglis2[i]=strtok(NULL," ");
				if(arglis2[i]!=NULL){
						if(arglis2[i][0]=='"'){
							int ll=strlen(arglis2[i]);
							if(arglis2[i][ll-1]=='"'){
								memmove(&arglis2[i][0],&arglis2[i][1],ll);
								arglis2[i][ll-2]=0;
							}
						}			
					}
			}
			int fd[2];
			if(pipe(fd)<0){
				perror("Pipe failed.");
				return 1;
			}
			pid_t pid1, pid2;
			pid1=fork();
			if(pid1<0){
				fprintf(stderr, "Fork failed.\n");
			}
			else if(pid1==0){
				if(type1==2){ //command 1 has "<"
					FILE *in1=fopen(file1,"r");
					dup2(fileno(in1),STDIN_FILENO);
					fclose(in1);
					//close(STDOUT);
					dup2(fd[1],STDOUT);
					close(fd[0]);
					if((execvp(arglis[0],arglis))<0){
						perror("Error");
						return 1;
					}
				}
				else{
					//close(STDOUT);	
					dup2(fd[1],STDOUT);
					close(fd[0]);
					if((execvp(arglis[0],arglis))<0){
						perror("Error");
						return 1;
					}
				}
			}
			else{
				wait(NULL);
			}
			close(fd[1]);
			pid2=fork();
			if(pid2<0){
				fprintf(stderr, "Fork failed.\n");
				return 1;
			}
			else if(pid2==0){		
				if(type2==1){ //command has 2 ">" or ">>"
					//printf("write_type2:%s,file2:%s\n",write_type2,file2);
					FILE *out2=fopen(file2,write_type2);
					dup2(fileno(out2),STDOUT_FILENO);
					fclose(out2);
					//close(STDIN);
					dup2(fd[0],STDIN);
					close(fd[1]);
					if((execvp(arglis2[0],arglis2))<0){
						perror("Error");
						return 1;
					}
				}
				else{
					//close(STDIN);
					dup2(fd[0],STDIN);
					close(fd[1]);
					if((execvp(arglis2[0],arglis2))<0){
						perror("Error");
						return 1;
					}
				}
			}
			else{
				wait(NULL);
			}
			close(fd[0]);
			close(fd[1]);
			waitpid(pid1,NULL,0);
			waitpid(pid2,NULL,0);
		}
	}
	}

	return 0;
}