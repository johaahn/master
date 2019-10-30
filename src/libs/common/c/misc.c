/***********************************************************************
 ** misc.h
 ***********************************************************************
 ** Copyright (c) SEAGNAL SAS
 **
 **
 ***********************************************************************/
#include <time.h>
#include <c/common.h>
#include <c/misc.h>

/* Stats*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

uint64_t f_get_time_ns64(void) {
  int ec;
  uint64_t i_tmp;
  struct timespec s_time;
  ec = clock_gettime(CLOCK_REALTIME, &s_time);
  if (ec == -1) {
    CRIT("Unable to get time");
  }
  //D("%d %d",s_time.tv_sec, s_time.tv_nsec);
  i_tmp = s_time.tv_sec;
  i_tmp *= 1000000000LL;
  i_tmp += s_time.tv_nsec;
  //D("%lld", i_tmp);
  return i_tmp;
}

uint64_t f_get_time_ns64_from_time_t(time_t * in_pi_time) {
  return (1000000000LL * (uint64_t) *in_pi_time);
}

uint64_t f_get_time_time_t_from_ns64(uint64_t * in_pi_time) {
  return ((time_t) (*in_pi_time) / 1000000000LL);
}

uint64_t f_get_time_ns64_from_timespec(struct timespec * in_s_time) {
  uint64_t i_tmp;
  i_tmp = in_s_time->tv_nsec;
  i_tmp += 1000000000LL * (in_s_time->tv_sec);
  return i_tmp;
}

void f_dump_stat(struct stat * in_s_sb) {
  switch (in_s_sb->st_mode & S_IFMT) {
  case S_IFBLK:
    printf("block device\n");
    break;
  case S_IFCHR:
    printf("character device\n");
    break;
  case S_IFDIR:
    printf("directory\n");
    break;
  case S_IFIFO:
    printf("FIFO/pipe\n");
    break;
  case S_IFLNK:
    printf("symlink\n");
    break;
  case S_IFREG:
    printf("regular file\n");
    break;
  case S_IFSOCK:
    printf("socket\n");
    break;
  default:
    printf("unknown?\n");
    break;
  }

  printf("I-node number:            %ld\n", (long) in_s_sb->st_ino);

  printf("Mode:                     %lo (octal)\n",
      (unsigned long) in_s_sb->st_mode);

  printf("Link count:               %ld\n", (long) in_s_sb->st_nlink);
  printf("Ownership:                UID=%ld   GID=%ld\n",
      (long) in_s_sb->st_uid, (long) in_s_sb->st_gid);

  printf("Preferred I/O block size: %ld bytes\n", (long) in_s_sb->st_blksize);
  printf("File size:                %lld bytes\n",
      (long long) in_s_sb->st_size);
  printf("Blocks allocated:         %lld\n", (long long) in_s_sb->st_blocks);

  printf("Last status change:       %s", ctime(&in_s_sb->st_ctime));
  printf("Last file access:         %s", ctime(&in_s_sb->st_atime));
  printf("Last file modification:   %s", ctime(&in_s_sb->st_mtime));

}

int f_file_dump(char * in_str_file, char * out_buffer, size_t sz_buffer) {
  int fd = open(in_str_file, O_RDONLY);
  int ec = EC_FAILURE;
  if (fd >= 0) {
    int ret = read(fd, out_buffer, sz_buffer);
#if 0
    int i;
    DBG("%d", ret);
    for( i=0; i<ret; i++) {
      DBG("%d %x %c", i, out_buffer[i], out_buffer[i]);
    }
#endif
    out_buffer[ret - 1] = 0;
    if (ret == -1) {
      ec = EC_FAILURE;
    } else {
      ec = EC_SUCCESS;
    }
    close(fd);
  }
  return ec;
}

#include <unistd.h>

size_t f_misc_get_total_system_memory(void) {
  long pages = sysconf(_SC_PHYS_PAGES);
  long page_size = sysconf(_SC_PAGE_SIZE);
  return pages * page_size;
}
#if 0
#include <sys/resource.h>
#include <sys/time.h>
#include <stdint.h>
#else
#include <sys/sysinfo.h>
#include <string.h>
#include <inttypes.h>
#endif
size_t f_misc_get_total_free_mem() {
#if 0
  uint64_t vm_size = 0;
  FILE *statm = fopen("/proc/self/statm", "r");
  if (!statm)
  return 0;
  if (fscanf("%llu", &vm_size) != 1)
  {
    flcose(statm);
    return 0;
  }
  vm_size = (vm_size + 1) * 1024;

  rlimit lim;
  if (getrlimit(RLIMIT_AS, &lim) != 0)
  return 0;
  if (lim.rlim_cur <= vm_size)
  return 0;
  if (lim.rlim_cur >= 0xC000000000000000ull) // most systems cannot address more than 48 bits
  lim.rlim_cur = 0xBFFFFFFFFFFFFFFFull;
  return lim.rlim_cur - vm_size;
#endif
#if 0
  int ec;
  struct sysinfo s_info;
  ec = sysinfo(&s_info);

  if(ec == -1) {
    return 0;
  } else {
    return s_info.freeram;
  }
#endif

  char *p;
  FILE *mem;
  /* put this in permanent storage instead of stack */
  static char meminfo[2048];
  /* we might as well get both swap and memory at the same time.
   * sure beats opening the same file twice */
  mem = fopen("/proc/meminfo", "r");
  memset(meminfo, 0, sizeof(meminfo));
  if (fread(meminfo, 2048, 1, mem) < 2048) {
    if (ferror(mem)) {
      fprintf(stderr, "error reading /proc/meminfo\n");
      return 0;
    }
  }
  uint64_t i_cached = 0;
  uint64_t i_mem_free = 0;

  p = strstr(meminfo, "MemFree");
  if (p) {
    sscanf(p, "MemFree:%"PRIu64, &i_mem_free);
    i_mem_free <<=10;

    p = strstr(p, "Cached");
    if (p) {
      sscanf(p, "Cached:%"PRIu64, &i_cached);
      i_cached <<=10;
    }
  }

  fclose(mem);
  return i_mem_free+i_cached;
}

int f_set_file_content(char * in_str_file, char * buff, size_t sz_buffer) {

  int fd = open(in_str_file, O_WRONLY);
  int ec = EC_FAILURE;

  // printf("Writing \"%s\" in file \"%s\"\n", buff, in_str_file);

  if (fd >= 0) {
    int ret = write(fd, buff, sz_buffer);
    if (ret == -1) {
      // printf("Write error : %s\n", strerror(errno));
      ec = EC_FAILURE;
    } else {
      // printf("ret %i\n", ret);
      ec = EC_SUCCESS;
    }
    close(fd);
  } else {
    // printf("Open error : %s\n", strerror(errno));
  }
  return ec;
}

int f_misc_file_exists (const char * in_str_file) {
  struct stat info;
  if( stat( in_str_file, &info ) != 0 ) {
    return EC_FAILURE;
  }  else if( info.st_mode & S_IFDIR )  {
    return EC_FAILURE;
  } else {
    return EC_SUCCESS;
  }
}

int f_misc_folder_exists (const char * in_str_folder) {
  struct stat info;
  if( stat( in_str_folder, &info ) != 0 ) {
    return EC_FAILURE;
  }  else if( info.st_mode & S_IFDIR )  {
    return EC_SUCCESS;
  } else {
    return EC_FAILURE;
  }
}

pid_t f_misc_popen2(const char *command, int *infp, int *outfp)
{
    int p_stdin[2];
    int p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) != 0)
        return -1;

    if (pipe(p_stdout) != 0)
        return -1;

    pid = fork();

    if (pid < 0) {
      perror("fork");
      CRIT("An error has occured during fork()");
      return pid;
    }
    else if (pid == 0)
    {
        close(p_stdin[1]);
        dup2(p_stdin[0], STDIN_FILENO);
        /* DO NOT PERFORM DEBUG PRINTF FROM HERE */
        close(p_stdout[0]);
        dup2(p_stdout[1], STDOUT_FILENO);
        const char * ac_argv[4] ={"sh", "-c", command, (char *)NULL};
        execve("/bin/sh", (char * const *) ac_argv, NULL);
        /* We should never reach this point */
        perror("execvp");
        exit(1);
    } else {
        close(p_stdout[1]);
        close(p_stdin[0]);
    }

    if (infp == NULL)
        close(p_stdin[1]);
    else
        *infp = p_stdin[1];

    if (outfp == NULL)
        close(p_stdout[0]);
    else
        *outfp = p_stdout[0];

    return pid;
}

/* Retourne la valeur de la variable d'environnement dont le nom correspond au 1er paramètre
 * si elle existe, sinon, retourne la valeur du second paramètre */
int f_misc_check_env_i(const char * in_str_varenv, uint32_t const i_default_value) {
  uint32_t i_tmp = i_default_value;
  char * str_env = getenv(in_str_varenv);
  if(str_env) {
    i_tmp = atoi(str_env);
  } else {
    WARN("Variable d'environnement %s non trouvée", in_str_varenv);
  }
  return i_tmp;
}
