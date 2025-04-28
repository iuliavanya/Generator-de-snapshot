#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>

#define st_atime st_atim.tv_sec
#define st_mtime st_mtim.tv_sec
#define st_ctime st_ctim.tv_sec


void scriere(int snapshot, const void *buffer, size_t nr)
{
  if(write(snapshot, buffer, nr) == -1)
  {
    perror("Nu s-a putut scrie in snapshot\n");
    exit(-1);
  }
}


char *tip_fisier(struct stat statbuf, char *case_stat)
{
  switch(statbuf.st_mode & S_IFMT)
  {
    case S_IFBLK: strcpy(case_stat,"block device"); break;
    case S_IFCHR: strcpy(case_stat,"character device"); break;
    case S_IFDIR: strcpy(case_stat,"directory"); break;
    case S_IFIFO: strcpy(case_stat,"FIFO/pipe"); break;
    case S_IFLNK: strcpy(case_stat,"symlink"); break;
    case S_IFREG: strcpy(case_stat,"regular file"); break;
    case S_IFSOCK: strcpy(case_stat,"socket"); break;
   }
  return case_stat;
}


void parcurgere_director(const char *director, int snapshot)
{
  DIR *d;
  struct dirent *dir;
  struct stat statbuf;
  char case_stat[20];

  if(!(d = opendir(director)))
  {
    perror("Nu s-a putut deschide directorul!\n");
    exit(-1);
  }

  write(snapshot,"DIRECTORUL CURENT: ",strlen("DIRECTORUL CURENT: "));
  scriere(snapshot,director,strlen(director));
  write(snapshot,"\n",strlen("\n"));

  while((dir = readdir(d)) != NULL)
  {
    char cale_fisier[strlen(director) + strlen(dir->d_name) + 2];
    snprintf(cale_fisier,sizeof(cale_fisier),"%s/%s",director,dir->d_name);

    if(stat(cale_fisier,&statbuf) == -1)
	{
	  perror("Eroare stat\n");
	  exit(-1);
	}

    strcpy(case_stat,tip_fisier(statbuf,case_stat));
    char i_node[2000];

    sprintf(i_node,"%ld\n",statbuf.st_ino);
    write(snapshot,"Fisier: ",strlen("Fisier: \n"));
    scriere(snapshot,dir->d_name,strlen(dir->d_name));
    write(snapshot,"\nTipul: ",strlen("Tipul: \n"));
    scriere(snapshot,case_stat,strlen(case_stat));
    write(snapshot,"\nI-node number: ",strlen("I-node number: \n"));
    scriere(snapshot,i_node,strlen(i_node));
    write(snapshot,"Last status changed: ",strlen("Last status changed: "));
    scriere(snapshot,ctime(&statbuf.st_ctime),strlen(ctime(&statbuf.st_ctime)));
    write(snapshot,"Last access: ",strlen("Last access: "));
    scriere(snapshot,ctime(&statbuf.st_atime),strlen(ctime(&statbuf.st_atime)));
    write(snapshot,"Last modification: ",strlen("Last modification: "));
    scriere(snapshot,ctime(&statbuf.st_mtime),strlen(ctime(&statbuf.st_mtime)));
    write(snapshot,"\n",strlen("\n"));
  }
  if(closedir(d) == -1)
  {
    perror("Nu s-a putut inchide directorul!\n");
    exit(-1);
  }
}


void creare_snapshot(const char *director, int i)
{
  int snapshot;
  char fisier_snapshot[strlen(director) + strlen("/Snapshot[]_.txt") + 2];

  snprintf(fisier_snapshot,sizeof(fisier_snapshot),"%ssnapshot%d_.txt",director,i);
  snapshot = open(fisier_snapshot, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);

  if(snapshot == -1)
  {
      perror("Nu s-a putut deschide snapshot-ul\n");
      exit(-1);
  }

  parcurgere_director(director,snapshot);
  
  if(close(snapshot) == -1)
  {
    perror("Nu s-a putut inchide snapshot-ul\n");
    exit(-1);
  }

  printf("Snapshot for directory %s created succesfully\n",director);
}


void create_snapshot(const char *director, const char *director_snapshot, int i, char *argv[])
{
  int snapshot;
  char fisier_snapshot[strlen(director_snapshot) + strlen("Director_snapshot[]_.txt") + 2];

  snprintf(fisier_snapshot,sizeof(fisier_snapshot),"%sDirector_snapshot.txt",director_snapshot);
  snapshot=open(fisier_snapshot, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);

  if(snapshot == -1)
  {
    perror("Nu s-a putut deschide snapshot-ul!\n");
    exit(-1);
  }

  parcurgere_director(director,snapshot);

  if(close(snapshot) == -1)
  {
    perror("Nu s-a putut inchide snapshot-ul!\n");
    exit(-1);
  }

  if(!strcmp(argv[3],"-s") == 0)
  {
    printf("Snapshot for directory %s created succesfully\n",director);
  }
}


void creare_proces(pid_t pid, int status, int argc, char *argv[], int i, int vector_pid[])
{
  if(i!=argc-1)
    {
      if((pid=fork())<0)
	  {
	    perror("Procesul fiu nu a putut fi creat!\n");
	    exit(-1);
	  }

      if(pid == 0)
	  {
	    i++;
	    vector_pid[i] = getpid();
	    creare_proces(pid, status, argc, argv, i, vector_pid);
        printf("Child process %d terminated with PID %d and exit code 0.\n", i, vector_pid[i]);
	    exit(0);
	  }
    }

    char *director_snapshot=argv[2];
    char *director=argv[i];
    create_snapshot(director,director_snapshot,i,argv);

    if(i != argc-1)
    {
      wait(&status);
      if(!WIFEXITED(status))
	  {
	    printf("Procesul copil cu id-ul %d din iteratia i=%d s-a incheiat anormal!\n",vector_pid[i],i);
	  }
    }
}


void launch_process(int argc, char *argv[], int i, int vector_pid[])
{
  pid_t pid;
  int status;
  if((pid=fork())<0)
    {
      perror("Child process couldnt be created!\n");
      exit(-1);
    }
  if(pid == 0)
    {
      vector_pid[i] = getpid();
      creare_proces(pid, status, argc, argv, i, vector_pid);
      exit(-1);
    }
  wait(&status);
  if(!WIFEXITED(status))
    {
      printf("Procesul copil cu id-ul %d din iteratia i=%d s-a incheiat anormal!\n",vector_pid[i],i);
    }
}


int main(int argc, char *argv[])
{
  if(strcmp(argv[3],"-s") == 0)
  {
    if(argc < 6 || argc > 15)
    {
      perror("Numarul de argumente trebuie sa fie intre 6 si 15!\n");
      exit(-1);
    }
    int i = 1;
    int vector_pid[argc - i];
    launch_process(argc, argv, i, vector_pid);
  }
  else if(strcmp(argv[1],"-o") == 0)
  {
    if(argc < 4 || argc > 13)
	  {
	    perror("Numarul de argumente trebuie sa fie intre 4 si 13!\n");
	    exit(-1);
	  }
    int i = 3;
    int vector_pid[argc - i];
    launch_process(argc, argv, i, vector_pid);
  }
  else
  {
    if(argc < 2 || argc > 11)
    {
      perror("Numarul de argumente trebuie sa fie intre 2 si 11!\n");
      exit(-1);
    }
    for(int i = 1; i < argc; i++)
    {
      char *director = argv[i];
      creare_snapshot(director, i);
    }
    printf("\n");
  }
  return 0;
}
