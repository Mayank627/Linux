#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>

#define STRSIZE 50

struct msg_buf
{
  long int msgtype;
  char msgstr[STRSIZE];
};

int qid;
int N;
char **roll;
char **names;
struct msg_buf mb;

void send(char *str, long int msgtype)
{
  char n[10];
  int r;

  strcpy(mb.msgstr, str);
  mb.msgtype = msgtype;

  r = msgsnd(qid, (void *)&mb, STRSIZE, 0);
  if (r == -1)
  {
    perror("Error sending message: ");
  }

  return;
}

void input()
{
  int i = 0;
  struct msg_buf mb;
  char *name, *roll;
  name = (char *)malloc(sizeof(char) * STRSIZE);
  roll = (char *)malloc(sizeof(char) * STRSIZE);

  printf("Enter the value of N : ");
  scanf("%d", &N);

  char n[10];
  sprintf(n, "%d", N);

  send(n, 1);
  send(n, 2);

  printf("Enter names and roll nos. (name <space> roll) : \n");

  for (i = 0; i < N; i++)
  {
    scanf("%s %s", name, roll);

    send(name, 1);
    send(roll, 2);
  }
}

void recv(long int msgtype)
{
  struct msg_buf mb;
  int i = 0;

  for (i = 0; i < N; i++)
  {
    msgrcv(qid, (void *)&mb, STRSIZE, msgtype, 0);
    printf("%s", mb.msgstr);

    if (i != N - 1)
      printf(", ");
  }

  return;
}

int main(void)
{
  qid = msgget((key_t)123456, IPC_CREAT | 0777);
  if (qid == -1)
  {
    printf("\nError: cannot use message queue!\n");
    return 1;
  }

  input();

  printf("\nSorted Names : \n");
  recv(3);

  printf("\n\nSorted roll nos : \n");
  recv(4);

  printf("\n\n");

  msgctl(qid, IPC_RMID, 0);
  return 0;
}
