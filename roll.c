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
int *rolls;

// Merge two subarrays L and M into arr
void merge(int arr[], int p, int q, int r)
{

    // Create L ← A[p..q] and M ← A[q+1..r]
    int n1 = q - p + 1;
    int n2 = r - q;

    int L[n1], M[n2];

    for (int i = 0; i < n1; i++)
        L[i] = arr[p + i];
    for (int j = 0; j < n2; j++)
        M[j] = arr[q + 1 + j];

    // Maintain current index of sub-arrays and main array
    int i = 0, j = 0, k = p;

    // Until we reach either end of either L or M, pick larger among
    // elements L and M and place them in the correct position at A[p..r]
    while (i < n1 && j < n2)
    {
        if (L[i] <= M[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = M[j];
            j++;
        }
        k++;
    }

    // When we run out of elements in either L or M,
    // pick up the remaining elements and put in A[p..r]
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2)
    {
        arr[k] = M[j];
        j++;
        k++;
    }
}

// Divide the array into two subarrays, sort them and merge them
void mergeSort(int arr[], int l, int r)
{
    if (l < r)
    {

        // m is the point where the array is divided into two subarrays
        int m = l + (r - l) / 2;

        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        // Merge the sorted subarrays
        merge(arr, l, m, r);
    }
}

void recv(void)
{
    struct msg_buf mb;
    int i = 0;

    msgrcv(qid, (void *)&mb, STRSIZE, 2, 0);

    n = atoi(mb.msgstr);
    rolls = (int *)malloc(sizeof(int) * n);

    for (i = 0; i < n; i++)
    {
        msgrcv(qid, (void *)&mb, STRSIZE, 2, 0);
        rolls[i] = atoi(mb.msgstr);
    }

    return;
}

void send()
{
    struct msg_buf mb;
    int r, i = 0;
    char str[STRSIZE];
    mb.msgtype = 4;

    for (i = 0; i < n; i++)
    {
        sprintf(str, "%d", rolls[i]);
        strcpy(mb.msgstr, str);

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
    mergeSort(rolls, 0, n - 1); // sorting

    printf("Sending sorted data...\n\n");
    send(); // sending the roll nos in a sorted order

    printf("Complete!\n\n");

    return 0;
}
