#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __WIN32__
enum OPS
{
	err,
	nop,
	scr,
	nul,
	write,
	rl

}operation;

int main(int argc, char **argv){

	if(argc<2){
		printf("USAGE: hiue <binary file> [optional scriptfile]\n");
		exit(1);
	}
	FILE *fp;
	fp=fopen(argv[1],"rb+");
	if(!fp){
		printf("ERROR - Can't Open Infile\n");
		exit(1);
	}
	int fsize;
	fseek(fp,0,SEEK_END);
	fsize = ftell(fp);
	rewind(fp);
	char *filebuf;
	filebuf = (char*) malloc(fsize+1);
	fread(filebuf,fsize,1,fp);
	fclose(fp);

#if defined (__WIN32__)
	system("cls");
#else
	system("clear");
#endif

	printf("Hack it Up Parsing Engine Alpha\n");

	if(argc<3){

		printf("Begin your commands below -- type 'q' to quit\n");
		char inputbuf[2048];

		int pos;

		while(1){
			pos = 0;
			operation = err;
			printf("# ");	
			fgets(inputbuf,sizeof(inputbuf),stdin);
			char *cmd = strtok(inputbuf, " ");
			while(cmd != NULL) {

				if(pos == 0){
					
					if(strcmp(cmd,"nop")==0){
					printf("NOP USED!\n");
					operation = nop;
					}

					if(strcmp(cmd,"scr")==0){
					printf("SCR USED!\n");
					operation = scr;
					}

					if(strcmp(cmd,"nul")==0){
					printf("NUL USED!\n");
					operation = nul;
					}

					if(strcmp(cmd,"rl")==0){
					printf("RL USED!\n");
					operation = rl;
					}

					if(strcmp(cmd,"write")==0){
					printf("WRITE USED!\n");
					operation = write;
					}

				}

				if(operation==nop){
					unsigned int offset=0;
					bool range=0;
					int j=0;
					while(1){
						
						if(cmd[j] == 0x2D){
							range = 1;
							printf("we got a range on our hands here, folks...\n");
						}
						if(cmd[j] == 0x00){
							
							memcpy(&offset,cmd,j-1);
							break;
						}
						j++;
					}

					if(!range){
					filebuf[offset] = 0x90;
					}
				}

				if(operation==write){
					FILE *fd;
				fd = fopen("OUTFILE","wb+");
				fwrite(filebuf,fsize,1,fd);
				fclose(fd);
				}

				if(operation==err && inputbuf[0] != 0x71 && inputbuf[1] != 0x00){
					printf("Unrecognized Operation: Syntax is <cmd> <value or range> <...>\n");
					break;
				}
				cmd = strtok(NULL, " ");
				pos++;
			}


			if(inputbuf[0] == 0x71 && inputbuf[1] == 0x00){ // type q to quit
				
				break;
			}

		}

	}

	return 0;
}

