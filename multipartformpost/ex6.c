
/* version of itoa that accepts three arguments instead of two. The third
argument is a minimum field width */


#include <stdio.h>
#include <string.h>

#define MAXLIMIT 100

void itoa(int n, char arr[], int width);
void reverse(char arr[]);

int main(void) 
{
    int number, width;
    char str[MAXLIMIT];
    number = 23874;
    width = 15;
    itoa(number, str, width);
    printf("%s", str);
    return 0;
}

void itoa(int n, char arr[], int width)  // converts integer to string
{
    int i, sign;
    sign = n;
    if (sign < 0)
        n = -n;
    i = 0;
    do 
    {
        arr[i++] = (n % 10) + '0';
    } 
    while ((n /= 10) > 0);
    
    if (sign < 0)
        arr[i++] = '-';
        
    while (i < width)
        arr[i++] = ' '; 
        
    arr[i] = '\0';
    reverse(arr);
}

void reverse(char arr[])   // reverse the string
{
    int i, j, c;
    for (i = 0, j = strlen(arr) - 1; i < j; i++, j--)
        c = arr[i], arr[i] = arr[j], arr[j] = c;
}



/* version of itoa that accepts three arguments instead of two. The third
argument is a minimum field width */


#include <stdio.h>
#include <string.h>

#define MAXLIMIT 100

void itoa(int n, char arr[], int width);
void reverse(char arr[]);

int main(void) 
{
    int number, width;
    char str[MAXLIMIT];
    number = 23874;
    width = 15;
    itoa(number, str, width);
    printf("%s", str);
    return 0;
}

void itoa(int n, char arr[], int width)  // converts integer to string
{
    int i, sign;
    sign = n;
    if (sign < 0)
        n = -n;
    i = 0;
    do 
    {
        arr[i++] = (n % 10) + '0';
    } 
    while ((n /= 10) > 0);
    
    if (sign < 0)
        arr[i++] = '-';
        
    while (i < width)
        arr[i++] = ' '; 
        
    arr[i] = '\0';
    reverse(arr);
}

void reverse(char arr[])   // reverse the string
{
    int i, j, c;
    for (i = 0, j = strlen(arr) - 1; i < j; i++, j--)
        c = arr[i], arr[i] = arr[j], arr[j] = c;
}



/* version of itoa that accepts three arguments instead of two. The third
argument is a minimum field width */


#include <stdio.h>
#include <string.h>

#define MAXLIMIT 100

void itoa(int n, char arr[], int width);
void reverse(char arr[]);

int main(void) 
{
    int number, width;
    char str[MAXLIMIT];
    number = 23874;
    width = 15;
    itoa(number, str, width);
    printf("%s", str);
    return 0;
}

void itoa(int n, char arr[], int width)  // converts integer to string
{
    int i, sign;
    sign = n;
    if (sign < 0)
        n = -n;
    i = 0;
    do 
    {
        arr[i++] = (n % 10) + '0';
    } 
    while ((n /= 10) > 0);
    
    if (sign < 0)
        arr[i++] = '-';
        
    while (i < width)
        arr[i++] = ' '; 
        
    arr[i] = '\0';
    reverse(arr);
}

void reverse(char arr[])   // reverse the string
{
    int i, j, c;
    for (i = 0, j = strlen(arr) - 1; i < j; i++, j--)
        c = arr[i], arr[i] = arr[j], arr[j] = c;
}


