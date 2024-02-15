
/* function expand(s1,s2) that expands shorthand notations like a-z in
the string s1 into the equivalent complete list abc...xyz in s2 */

#include <stdio.h>
#define MAXLINE 100

int mgetline(char s[], int lim);
void expand(char s1[], char s2[]);

int main() 
{
    char s1[MAXLINE], s2[MAXLINE];
    mgetline(s1, MAXLINE);  //user input is stored in string s1
    expand(s1, s2);
    printf("%s", s2);
    return 0;
}

int mgetline(char s[], int lim)   //get input from the user, make sure not to give empty spaces
{
    int i, c;
    for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
    {
	    s[i] = c;
    }
    
    if (c == '\n')
    {		
        s[i++] = c;
    }

    s[i] = '\0';
}

void expand(char s1[], char s2[])  //function that expands shorthand notations 
{
	int i, j, c;
        i = j = 0;

    while ((c = s1[i]) != '\0')
    {   
	    i++;
            if (s1[i] == '-' && s1[i + 1] >= c) 
	    {
            	i++;
            	while (c < s1[i])
		{
                	s2[j] = c;
			j++;
			c++;
		}
            } 
	    else
            	s2[j++] = c;

    s2[j] = '\0';
    }
}








/* function expand(s1,s2) that expands shorthand notations like a-z in
the string s1 into the equivalent complete list abc...xyz in s2 */

#include <stdio.h>
#define MAXLINE 100

int mgetline(char s[], int lim);
void expand(char s1[], char s2[]);

int main() 
{
    char s1[MAXLINE], s2[MAXLINE];
    mgetline(s1, MAXLINE);  //user input is stored in string s1
    expand(s1, s2);
    printf("%s", s2);
    return 0;
}

int mgetline(char s[], int lim)   //get input from the user, make sure not to give empty spaces
{
    int i, c;
    for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
    {
	    s[i] = c;
    }
    
    if (c == '\n')
    {		
        s[i++] = c;
    }

    s[i] = '\0';
}

void expand(char s1[], char s2[])  //function that expands shorthand notations 
{
	int i, j, c;
        i = j = 0;

    while ((c = s1[i]) != '\0')
    {   
	    i++;
            if (s1[i] == '-' && s1[i + 1] >= c) 
	    {
            	i++;
            	while (c < s1[i])
		{
                	s2[j] = c;
			j++;
			c++;
		}
            } 
	    else
            	s2[j++] = c;

    s2[j] = '\0';
    }
}







