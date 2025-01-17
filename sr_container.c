/**
 *  @title      :   sr_container.c
 *  @author     :   Shabir Abdul Samadh (shabirmean@cs.mcgill.ca)
 *  @date       :   20th Nov 2018
 *  @purpose    :   COMP310/ECSE427 Operating Systems (Assingment 3) - Phase 2
 *  @description:   A template C code to be filled in order to spawn container instances
 *  @compilation:   Use "make container" with the given Makefile
*/

#include "sr_container.h"

/**
 *  The cgroup setting to add the writing task to the cgroup
 *  '0' is considered a special value and writing it to 'tasks' asks for the wrinting 
 *      process to be added to the cgroup. 
 *  You must add this to all the controls you create so that it is added to the task list.
 *  See the example 'cgroups_control' added to the array of controls - 'cgroups' - below
 **/  
struct cgroup_setting self_to_task = {
	.name = "tasks",
	.value = "0"
};

/**
 *  ------------------------ TODO ------------------------
 *  An array of different cgroup-controllers.
 *  One controller has been been added for you.
 *  You should fill this array with the additional controls from commandline flags as described 
 *      in the comments for the main() below
 *  ------------------------------------------------------
 **/ 
struct cgroups_control *cgroups[6] = {
	& (struct cgroups_control) {
		.control = CGRP_BLKIO_CONTROL,
		.settings = (struct cgroup_setting *[]) {
			& (struct cgroup_setting) {
				.name = "blkio.weight",
				.value = "64"
			},
			&self_to_task,             // must be added to all the new controls added
			NULL                       // NULL at the end of the array
		}
	},
	NULL                               // NULL at the end of the array
};


/**
 *  ------------------------ TODO ------------------------
 *  The SRContainer by default suppoprts three flags:
 *          1. m : The rootfs of the container
 *          2. u : The userid mapping of the current user inside the container
 *          3. c : The initial process to run inside the container
 *  
 *   You must extend it to support the following flags:
 *          1. C : The cpu shares weight to be set (cpu-cgroup controller)
 *          2. s : The cpu cores to which the container must be restricted (cpuset-cgroup controller)
 *          3. p : The max number of process's allowed within a container (pid-cgroup controller)
 *          4. M : The memory consuption allowed in the container (memory-cgroup controller)
 *          5. r : The read IO rate in bytes (blkio-cgroup controller)
 *          6. w : The write IO rate in bytes (blkio-cgroup controller)
 *          7. H : The hostname of the container 
 * 
 *   You can follow the current method followed to take in these flags and extend it.
 *   Note that the current implementation necessitates the "-c" flag to be the last one.
 *   For flags 1-6 you can add a new 'cgroups_control' to the existing 'cgroups' array
 *   For 7 you have to just set the hostname parameter of the 'child_config' struct in the header file
 *  ------------------------------------------------------
 **/
int main(int argc, char **argv)
{
    struct child_config config = {0};
    int option = 0;
    int sockets[2] = {0};
    pid_t child_pid = 0;
    int last_optind = 0;
    bool found_cflag = false;
    while ((option = getopt(argc, argv, "c:m:u:H:M:p:C:s:")))
    {


	    struct cgroups_control* mem = malloc(sizeof(struct cgroups_control));
        struct cgroup_setting* mem_setting = malloc(sizeof(struct cgroup_setting));
        struct cgroup_setting** mem_settings = malloc(sizeof(struct cgroup_setting *));

        struct cgroups_control* pid = malloc(sizeof(struct cgroups_control));
        struct cgroup_setting* pid_setting = malloc(sizeof(struct cgroup_setting));
        struct cgroup_setting** pid_settings = malloc(sizeof(struct cgroup_setting *));

        struct cgroups_control* cpu = malloc(sizeof(struct cgroups_control));
        struct cgroup_setting* cpu_setting = malloc(sizeof(struct cgroup_setting));
        struct cgroup_setting** cpu_settings = malloc(sizeof(struct cgroup_setting *));

        struct cgroups_control* cpu_set = malloc(sizeof(struct cgroups_control));
        struct cgroup_setting* cpu_set_setting = malloc(sizeof(struct cgroup_setting));
//        struct cgroup_setting* cpu_set_mem_setting = malloc(sizeof(struct cgroup_setting));
        struct cgroup_setting** cpu_set_settings = malloc(sizeof(struct cgroup_setting *));

        if (found_cflag)
            break;

        switch (option)
        {
        case 'c':
            config.argc = argc - last_optind - 1;
            config.argv = &argv[argc - config.argc];
            found_cflag = true;
            break;
        case 'm':
            config.mount_dir = optarg;
            break;
        case 'u':
            if (sscanf(optarg, "%d", &config.uid) != 1)
            {
                fprintf(stderr, "UID not as expected: %s\n", optarg);
                cleanup_stuff(argv, sockets);
                return EXIT_FAILURE;
            }
            break;
        case 'H':
            config.hostname = optarg;
            break;
        case 'M':
	     
            strcpy(mem->control,CGRP_MEMORY_CONTROL);
            strcpy(mem_setting->name,"memory.limit_in_bytes");
            strcpy(mem_setting->value, optarg);
            
            *(mem_settings) = mem_setting;
	        *(mem_settings+1) = &self_to_task;
	        *(mem_settings+2) = NULL;
 
     	    mem->settings= mem_settings;	    
            cgroups[1] = mem;
            break;
        case 'p':
            strcpy(pid->control,CGRP_PIDS_CONTROL);
            strcpy(pid_setting->name,"pids.max");
            strcpy(pid_setting->value, optarg);
            
            *(pid_settings) = pid_setting;
            *(pid_settings+1) = &self_to_task;
            *(pid_settings+2) = NULL;

            pid->settings= pid_settings;
            cgroups[2] = pid;
            break;
        case 'C':
            strcpy(cpu->control,CGRP_CPU_CONTROL);
            strcpy(cpu_setting->name,"cpu.shares");
            strcpy(cpu_setting->value, optarg);
            
            *(cpu_settings) = cpu_setting;
            *(cpu_settings+1) = &self_to_task;
            *(cpu_settings+2) = NULL;

            cpu->settings= cpu_settings;
            cgroups[3] = cpu;
            break;

        case 's':
            strcpy(cpu_set->control,CGRP_CPU_SET_CONTROL);
            strcpy(cpu_set_setting->name,"cpuset.cpus");
            strcpy(cpu_set_setting->value, optarg);

            *(cpu_set_settings) = cpu_set_setting;

            struct cgroup_setting* cpu_set_mem_setting = malloc(sizeof(struct cgroup_setting));
            strcpy(cpu_set_mem_setting->name,"cpuset.mems");
            strcpy(cpu_set_mem_setting->value,"0-1");
            
            //strcpy((*(cpu_set_settings+1))->name, "cpuset.mems");
            //strcpy((*(cpu_set_settings+1))->value, "0-1");
            *(cpu_set_settings+1)=cpu_set_mem_setting;
            *(cpu_set_settings+2)=&self_to_task;
            *(cpu_set_settings+3)=NULL;

            cpu_set->settings= cpu_set_settings;
            cgroups[4] = cpu_set;
            break;


/*        case 's':
            strcpy(cpu_set->control,CGRP_CPU_SET_CONTROL);
            strcpy(cpu_set_setting->name,"cpuset.cpus");
            strcpy(cpu_set_setting->value, optarg);

            //strcpy(*(cpu_set_settings+1)->name, "cpuset.mems")
            //strcp(*(cpu_set_settings+1)-> value, "0-1")
            // *(cpu_set_settings+2)=self to task
            // *(cpu_set_settings+3)=NULL

            *(cpu_set_settings) = cpu_set_setting;
            *(cpu_set_settings+1) = &self_to_task;
            *(cpu_set_settings+2) = NULL;

            cpu_set->settings= cpu_set_settings;
            cgroups[4] = cpu_set;


            break;*/
        default:
            cleanup_stuff(argv, sockets);
            return EXIT_FAILURE;
        }
        last_optind = optind;
    }

    if (!config.argc || !config.mount_dir){
        cleanup_stuff(argv, sockets);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "####### > Checking if the host Linux version is compatible...");
    struct utsname host = {0};
    if (uname(&host))
    {
        fprintf(stderr, "invocation to uname() failed: %m\n");
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }
    int major = -1;
    int minor = -1;
    if (sscanf(host.release, "%u.%u.", &major, &minor) != 2)
    {
        fprintf(stderr, "major minor version is unknown: %s\n", host.release);
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }
    if (major != 4 || (minor < 7))
    {
        fprintf(stderr, "Linux version must be 4.7.x or minor version less than 7: %s\n", host.release);
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }
    if (strcmp(ARCH_TYPE, host.machine))
    {
        fprintf(stderr, "architecture must be x86_64: %s\n", host.machine);
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }
    fprintf(stderr, "%s on %s.\n", host.release, host.machine);

    if (socketpair(AF_LOCAL, SOCK_SEQPACKET, 0, sockets))
    {
        fprintf(stderr, "invocation to socketpair() failed: %m\n");
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }
    if (fcntl(sockets[0], F_SETFD, FD_CLOEXEC))
    {
        fprintf(stderr, "invocation to fcntl() failed: %m\n");
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }
    config.fd = sockets[1];

    /**
     * ------------------------ TODO ------------------------
     * This method here is creating the control groups using the 'cgroups' array
     * Make sure you have filled in this array with the correct values from the command line flags 
     * Nothing to write here, just caution to ensure the array is filled
     * ------------------------------------------------------
     **/
    if (setup_cgroup_controls(&config, cgroups))
    {
        clean_child_structures(&config, cgroups, NULL);
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }

    /**
     * ------------------------ TODO ------------------------
     * Setup a stack and create a new child process using the clone() system call
     * Ensure you have correct flags for the following namespaces:
     *      Network, Cgroup, PID, IPC, Mount, UTS (You don't need to add user namespace)
     * Set the return value of clone to 'child_pid'
     * Ensure to add 'SIGCHLD' flag to the clone() call
     * You can use the 'child_function' given below as the function to run in the cloned process
     * HINT: Note that the 'child_function' expects struct of type child_config.
     * ------------------------------------------------------
     **/
    
    char *stack;                        /* Start of stack buffer area */
    char *stackTop;                     /* End of stack buffer area */
    int flags;
    
    
    stack = malloc(STACK_SIZE);
    if(stack == NULL) {
        perror("MALLOC");
        exit(EXIT_FAILURE);
    }
    
    flags = CLONE_NEWCGROUP | CLONE_NEWPID | CLONE_NEWIPC | CLONE_NEWNS | CLONE_NEWUTS | CLONE_NEWNET;
    
    stackTop = stack + STACK_SIZE;  /* Assume stack grows downward */
    child_pid = clone(child_function, stackTop, flags | SIGCHLD, (void *)&config);



        // You code for clone() goes here

    /**
     *  ------------------------------------------------------
     **/ 
    if (child_pid == -1)
    {
        fprintf(stderr, "####### > child creation failed! %m\n");
        clean_child_structures(&config, cgroups, stack);
        cleanup_sockets(sockets);
        return EXIT_FAILURE;
    }
    close(sockets[1]);
    sockets[1] = 0;

    if (setup_child_uid_map(child_pid, sockets[0]))
    {
        if (child_pid)
            kill(child_pid, SIGKILL);
    }

    int child_status = 0;
    waitpid(child_pid, &child_status, 0);
    int exit_status = WEXITSTATUS(child_status);

    clean_child_structures(&config, cgroups, stack);
    cleanup_sockets(sockets);
    return exit_status;
}


int child_function(void *arg)
{
    struct child_config *config = arg;
    if (sethostname(config->hostname, strlen(config->hostname)) || \
                setup_child_mounts(config) || \
                setup_child_userns(config) || \
                setup_child_capabilities() || \
                setup_syscall_filters()
        )
    {
        close(config->fd);
        return -1;
    }
    if (close(config->fd))
    {
        fprintf(stderr, "invocation to close() failed: %m\n");
        return -1;
    }
    if (execve(config->argv[0], config->argv, NULL))
    {
        fprintf(stderr, "invocation to execve() failed! %m.\n");
        return -1;
    }
    return 0;
}
