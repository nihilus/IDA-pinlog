#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/mman.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<error.h>
#include	<unistd.h>
#include	<elf.h>

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
	Elf32_Ehdr elf;
	unsigned char b_pin32;
	
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

	if (read(fd, &elf, sizeof(elf)) == -1){
		printf("Failed to read Elf header...\n");
		return 1;
	}

	close(fd);
	if (memcmp(&elf, ELFMAG, 4)){
		printf("File %s is not ELF file...\n", argv[1]);
		return 1;
	}
	
	if (elf.e_machine == EM_386)
		b_pin32 = 1;
	else if (elf.e_machine == EM_X86_64)
		b_pin32 = 0;
	else{
		printf("File is not x32 or x64 intel... aborting...\n");
		return 1;
	}

	

	memset(path, 0, sizeof(path));
	memset(symlink, 0, sizeof(symlink));

	snprintf(symlink, sizeof(symlink), "/proc/%d/exe", getpid());
	readlink(symlink, path, sizeof(path)-1);

	p = strrchr(path, '/');
	*p = 0;

	secfd = shm_open("/pin_shared_mem", O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
	if (secfd == -1){
		perror("shm_open");
		return 0;
	}
	ftruncate(secfd, SECTION_SIZE);
	ptr = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, secfd, 0);
	if (ptr == NULL){
		perror("mmap");
		return 0;
	}
	memset(ptr, 0, SECTION_SIZE);

	memset(pintoolpath, 0, sizeof(pintoolpath));
	if (b_pin32)	
		snprintf(pintoolpath, sizeof(pintoolpath)-1, "%s/pin32/trace.so", path);
	else
		snprintf(pintoolpath, sizeof(pintoolpath)-1, "%s/pin64/trace.so", path);

        if (0 == (pid = fork())){
	        printf("Starting child process...");
		exe_argv[0] = "pin";
	        exe_argv[1] = "-injection";
	        exe_argv[2] = "child";
	        exe_argv[3] = "-smc_strict";
	        exe_argv[4] = "1";
	        exe_argv[5] = "-follow_execv";
	        exe_argv[6] = "1";
	        exe_argv[7] = "-t";
	        exe_argv[8] = pintoolpath;
	        exe_argv[9] = "-p";
	        exe_argv[10]= argv[2];
	        exe_argv[11]= "--";
	        exe_argv[12]= argv[1];
	        exe_argv[13]= NULL;
	        execvp("pin", exe_argv);
		perror("execv");
	} 
	
	waitpid(pid, NULL, 0);
	printf("all done...\n");

	index = SECTION_SIZE - 1;
	p = ptr;
	while (p[index] == 0 && index != 0)
		index--;
	if (index == 0){
		printf("No trace was performed...\n");
		return 1;
	}

	fd = open(argv[3], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	write(fd, ptr, index);
	close(fd);
}
