#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/msg.h>

#define STRSIZE 50

struct msg_buf
{
    long int msgtype;
    char msgstr[STRSIZE];
};

int qid, n;
char **names;

void Merge(char *arr[], int low, int mid, int high) // Merging the Array Function
{
    int nL = mid - low + 1;
    int nR = high - mid;

    char **L = malloc(sizeof(char *) * nL);
    char **R = malloc(sizeof(char *) * nR);
    int i;
    for (i = 0; i < nL; i++)
    {
        L[i] = malloc(sizeof(arr[low + i]));
        strcpy(L[i], arr[low + i]);
    }
    for (i = 0; i < nR; i++)
    {
        R[i] = malloc(sizeof(arr[mid + i + 1]));
        strcpy(R[i], arr[mid + i + 1]);
    }
    int j = 0, k;
    i = 0;
    k = low;
    while (i < nL && j < nR)
    {
        if (strcmp(L[i], R[j]) < 0)
            strcpy(arr[k++], L[i++]);
        else
            strcpy(arr[k++], R[j++]);
    }
    while (i < nL)
        strcpy(arr[k++], L[i++]);
    while (j < nR)
        strcpy(arr[k++], R[j++]);
}

void MergeSort(char *arr[], int low, int high) // Main MergeSort function
{
    if (low < high)
    {
        int mid = (low + high) / 2;
        MergeSort(arr, low, mid);
        MergeSort(arr, mid + 1, high);
        Merge(arr, low, mid, high);
    }
}

void recv(void)
{
    struct msg_buf mb;
    int i = 0;

    msgrcv(qid, (void *)&mb, STRSIZE, 1, 0);

    n = atoi(mb.msgstr);
    names = (char **)malloc(sizeof(char *) * n);

    for (i = 0; i < n; i++)
    {
        names[i] = (char *)malloc(sizeof(char) * STRSIZE);
        msgrcv(qid, (void *)&mb, STRSIZE, 1, 0);

        strcpy(names[i], mb.msgstr);
    }

    return;
}

void send()
{
    struct msg_buf mb;
    int r, i = 0;
    mb.msgtype = 3;

    for (i = 0; i < n; i++)
    {
        strcpy(mb.msgstr, names[i]);

        r = msgsnd(qid, (void *)&mb, STRSIZE, 0);
        if (r == -1)
        {
            perror("Error sending message: ");
        }
    }

    return;
}

int main(void)
{
    qid = msgget((key_t)123456, 0777);
    if (qid == -1)
    {
        printf("\nError: message queue cration failed!\n");
        return 1;
    }

    printf("Receiving data...\n\n");
    recv(); // receiving the names

    printf("Sorting data...\n\n");
    MergeSort(names, 0, n - 1); // sorting

    printf("Sending sorted data...\n\n");
    send(); // sending the names in a sorted order

    printf("Complete!\n\n");

    return 0;
}
