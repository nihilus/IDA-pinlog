#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/mman.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<unistd.h>
#include        <mach-o/loader.h>
#include	<mach-o/fat.h>
#include	<arpa/inet.h>
#include	<mach/machine.h>

#define		SECTION_SIZE	0x100000 * 50

int main(int argc, char **argv){
	int	secfd;
	int	fd;
	void	*ptr;
	unsigned char *p;
	int	exec_argc;
	char	*exe_argv[14];
	pid_t   pid;
	unsigned long index;
	char path[260];
	char symlink[260];
	char pintoolpath[260];
	unsigned char b_pin32;
	struct mach_header mach_header;
	struct fat_header  fat_header;
	struct fat_arch	   fat_arch;
	
	if (argc != 4){
		printf("Usage:\n");
		printf("	pinlog <file_to_trace> <module> <log_file>\n");
		printf("Example:\n");
		printf("	pinlog /bin/bash bash log.log\n");
		return 1;
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1){
		printf("failed to open %s\n", argv[1]);
		return 1;
	}
	
	if (read(fd, &mach_header, sizeof(mach_header)) == -1){
	        printf("failed to read mach file...\n");
	        return 1;
	}
	lseek(fd, 0, SEEK_SET);
	if (read(fd, &fat_header, sizeof(fat_header)) == -1){
		printf("failed to read mach file...\n");
		return 1;
	}
	

	
	//in case of fat binary we assume it's gonna run x64
	//so no need to parse whole structure	
	if (fat_header.magic == FAT_CIGAM){
		printf("Fat binary\n");
		printf("Number of binaries : %.08X\n", htonl(fat_header.nfat_arch));
		//check if there is x64 inside (if not, bail...)
		for (index = 0; index < htonl(fat_header.nfat_arch); index++){
			if (read(fd, &fat_arch, sizeof(fat_arch)) == -1){
				printf("Failed to read fat_arch...abort...\n");
				exit(1);
			}
			if (htonl(fat_arch.cputype) == CPU_TYPE_X86_64){
				printf("found intel x64 binary in fat\n");
				printf("trace will continue...\n");
				b_pin32 = 0;
				close(fd);
				goto __Execute;
			}	
		}
		printf("no x64 found in fat binary... trace will stop...\n");
		close(fd);
		exit(0);
		
	}
	close(fd);
	if (mach_header.magic == MH_MAGIC){
	        b_pin32 = 1;
	}else if (mach_header.magic == MH_MAGIC_64){
	        b_pin32 = 0;
	}else{
	        printf("Unknown file... not MG_MAGIC nor MG_MAGIC_X64\n");
	        return 1;
	}
__Execute:	
	memset(path, 0, sizeof(path));
	memset(symlink, 0, sizeof(symlink));

	strncpy(path, argv[0], sizeof(path)-1);
	//snprintf(symlink, sizeof(symlink), "/proc/%d/exe", getpid());
	//readlink(symlink, path, sizeof(path)-1);
	p = strrchr(path, '/');
	*p = 0;
	//printf("%s\n", path);
       
	memset(pintoolpath, 0, sizeof(pintoolpath));
	if (b_pin32)	
		snprintf(pintoolpath, sizeof(pintoolpath)-1, "%s/pin32/trace.dylib", path);
	else
		snprintf(pintoolpath, sizeof(pintoolpath)-1, "%s/pin64/trace.dylib", path);
        if (0 == (pid = fork())){
	        printf("Starting child process...");
		exe_argv[0] = "pin.sh";
	        //exe_argv[1] = "-injection";
	        //exe_argv[2] = "child";
	        exe_argv[3-2] = "-smc_strict";
	        exe_argv[4-2] = "1";
	        exe_argv[5-2] = "-follow_execv";
	        exe_argv[6-2] = "1";
	        exe_argv[7-2] = "-t";
	        exe_argv[8-2] = pintoolpath;
	        exe_argv[9-2] = "-p";
	        exe_argv[10-2]= argv[2];
	        exe_argv[11-2]= "--";
	        exe_argv[12-2]= argv[1];
	        exe_argv[13-2]= NULL;
	        execvp("pin", exe_argv);
		perror("execv");
	} 
	
	waitpid(pid, NULL, 0);
	printf("all done...\n");
        sleep(1);
        secfd = open("/tmp/pin_shared_mem", O_RDWR); //, S_IREAD | S_IWRITE);
	if (secfd == -1){
		perror("shm_open");
		return 0;
	}
	
	ptr = malloc(SECTION_SIZE);
	read(secfd, ptr, SECTION_SIZE);
        
	index = SECTION_SIZE - 1;
	p = ptr;
	while (p[index] == 0 && index != 0)
		index--;
	if (index == 0){
		printf("No trace was performed...\n");
		return 1;
	}
	close(secfd);
	unlink("/tmp/pin_shared_mem");
        
	fd = open(argv[3], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1){
	        perror("On creatign output lof file");       
	}
	write(fd, ptr, index);
	close(fd);
}
