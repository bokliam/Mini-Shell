/************************************************
  Liam Bok
  Project #5
************************************************/

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <pthread.h>

using namespace std;

/* Exit flag for while loop */
bool flag = false;

/* Char array used to collect input */
char special[128];

/* Char array used to keep copy of original input */
char full[128];

/*--------------------------------------------------------
Name: sequence

Purpose: Create command line sequence
Receive: Integer representing the count, starting at 1
--------------------------------------------------------*/
void sequence(int i)
{
  cout << "[" << i << " " << getlogin() << "]";
}


/*-------------------------------------------------------
Name: curTime

Purpose: Gives the current date and time
-------------------------------------------------------*/
void curTime()
{
  /* Get current time based off system timer */
  time_t currentTime;
  time(&currentTime);
  
  /* Print current date and local time based off system timer */
  printf(ctime(&currentTime));
}


/*-------------------------------------------------------
Name: clearWhiteSpace

Purpose: Clear the white space in a given char*
Receive: Char* to clear all white space from
Return: Char* with all white space cleared
-------------------------------------------------------*/
char* clearWhiteSpace(char *string)
{
  /* Initialize index variables and new char array */
  unsigned int x;
  unsigned int y;
  char* final = string; 
  
  /* Loop through original string and move all chars to new string */
  for (x = 0, y = 0; x < strlen(string); x++, y++)
  {
    /* Add char to new char array */
    if (string[x] != ' ')
    {
      final[y] = string[x];
    }

    /* Otherwise maintain proper index of char array */
    else
    {
      y--;
    }
  }

  /* Null terminate char array and return */
  final[y] = 0;
  return final;
}


/*-------------------------------------------------------
Name: append

Purpose: Appends characters from one char* to another
Receive: 2 char*, first is destination, second is appended
-------------------------------------------------------*/
void append(char* str1, char* str2)
{
  /* Get lengths of each char array and initialize indexer */
  int len1 = strlen(str1);
  int len2 = strlen(str2);
  int x;
  
  /* Loop through each character in str2 and add to str1 */
  for (x = 0; x < len2; x++)
  {
    str1[(len1+x)] = str2[x];
  }

  /* Get new length of str1 and null terminate it */
  int len = strlen(str1);
  str1[len] = 0;
}


/*-------------------------------------------------------
Name: command

Purpose: Run external command passed in
Receive: void* line (command)
-------------------------------------------------------*/
void* command(void* line)
{
  
  char* cmd;
  cmd = (char*) line;

  system(cmd);
  return 0; 
}

/*-------------------------------------------------------
Name: newProcess

Purpose: Run new thread process
Receive: void* line (command)
-------------------------------------------------------*/
void* newProcess(void* line)
{
    char* input;
    input = (char*) line;

    int length = strlen(input);
    strncpy(full, input, length+1);
    char* tok = strtok(input, " ");

    /* Terminate the shell process */
    if ( (strcmp(tok, "halt") == 0) || (strcmp(tok, "halt\n") == 0) )
    {
      flag = true;
      return 0;
    }
  
    /* Display a list of all valid built-in commands */
    else if ( (strcmp(tok, "help") == 0) || (strcmp(tok, "help\n") == 0) )
    {
      printf("Built-in commands are the following: \n"\
          "halt - terminate shell process\n"\
          "help - display built-in commands\n"\
          "date - display current date and time\n"\
          "env - display environment variables\n"\
          "path - display current search path\n"\
          "cwd - display absolute pathname of current working directory\n"\
          "cd - manage current working directory\n"\
          "set - manage environment variables\n"\
          "import - read commands from a file\n");
      
      return 0;
    }
    
    /* Display date and time */
    else if ( (strcmp(tok, "date") == 0) || (strcmp(tok, "date\n") == 0) )
    {
      curTime();
      return 0;
    }

    /* Display user environment variables */
    else if ( (strcmp(tok, "env") == 0) || (strcmp(tok, "env\n") == 0) )
    {
      for (int i = 0; environ[i] != NULL; i++)
      {
        cout << environ[i] << '\n';
        
      }
      return 0;
    }
    
    /* Display current search path */
    else if ( (strcmp(tok, "path") == 0) || (strcmp(tok, "path\n") == 0) )
    {
      for (int i = 0; environ[i] != NULL; i++)
      {
        if (strcmp(environ[i], "PATH") == 0)
        {
          printf("%s", environ[i]+5);
          cout << "\n";
        }
      }
      return 0;
    }
    
    /* Display absolute pathname of current working directory */
    else if ( (strcmp(tok, "cwd") == 0) || (strcmp(tok, "cwd\n") == 0) )
    {
      char dir[200];
      if (getcwd(dir, 200) != NULL)
      {
        cout << dir << "\n";
      }

      else
      {
        perror("Error: ");
      }
      return 0;
    }

    /* Change working directory to current users home directory */
    else if ( (strcmp(tok, "cd") == 0) || (strcmp(tok, "cd\n") == 0) )
    {
      chdir("..");
      chdir(getlogin());

      /* Change PWD variable */
      char dir[200];
      getcwd(dir, 200);
      char const* pwd;
      pwd = "PWD";
      setenv(pwd, dir, 1);

      return 0;
    }
    
    /* Change working directory to home directory of \
       current user if no user is specified */
    else if ( (strcmp(tok, "cd") == 0) && (length == 4) )
    {
      chdir("..");
      chdir(getlogin());

      /* Change PWD variable */
      char dir[200];
      getcwd(dir, 200);
      char const* pwd = "PWD";
      setenv(pwd, dir, 1);

      return 0;
    }


    /* Change working directory to pathname supplied by user or \
       home of specified user*/
    else if ( (strcmp(tok, "cd") == 0) && (length >= 3) )
    {
      /* Get next token */
      tok = strtok(NULL, " ");
      
      while (tok != NULL)
      {
        if (tok[0] != '~')
        {
          chdir(tok);
        
          /* Change PWD variable */
          char dir[200];
          getcwd(dir, 200);
          char const* pwd = "PWD";
          setenv(pwd, dir, 1);
      
          break;
        }
        /* Change directory to home of specified user */
        else
        {
          chdir("/user");
          char* temp = tok + 1;
          chdir(temp);
          
          /* Change PWD variable */
          char dir[200];
          getcwd(dir, 200);
          char const* pwd = "PWD";
          setenv(pwd, dir, 1);
          
          break;
        }
      }
      return 0;
    }

    /* Set or Remove environment variable */    
    else if ( (strcmp(tok, "set") == 0) || (strcmp(tok, "set\n") == 0) )
    {
      /* Get next token */
      tok = strtok(NULL, " ");

      char* var = tok;
      
      tok = strtok(NULL, " ");
      
      if (tok != NULL)
      {
        char* val = tok;
        setenv(var, val, 1);
      }

      else
      {
        unsetenv(var);
      }
      return 0;
    }
    /* Execute external command */
    else
    {
      pthread_t tid; 
      int cmd;

      cmd = pthread_create(&tid, NULL, command, (void *) full);
      pthread_join(tid, NULL);

      if (cmd != 0)
      {
        printf("Error; Command not recognized. Enter valid command"\
            " and try again.");
      }
      return 0;
    }
  
}

/*-----------------------------------------------------
Name: main

Purpose: Serves as basic command-line interpreter
Receive: argument count, char* of arguments, \
  char* of environment variables
Return: 0 on successful program halt
-------------------------------------------------------*/
int main(int argc, char *argv[], char *envp[])
{
  /* Counter for sequence function */
  int i = 1;

  /* Run while flag is false (True when 'halt' inputted) */    
  while (flag == false)
  {
    /* Command sequence counter */
    sequence(i);
    i++;
    
    /* Get input for 128 bytes, save copy, and tokenize */ 
    cin.getline(special, 128);
    int length = strlen(special);
    strncpy(full, special, length+1);
    char* tok = strtok(special, " ");

    /* Terminate the shell process */
    if ( (strcmp(tok, "halt") == 0) && (length <= 4) )
    {
      flag = true;
    }
  
    /* Display a list of all valid built-in commands */
    else if ( (strcmp(tok, "help") == 0) && (length <= 4) )
    {
      printf("Built-in commands are the following: \n"\
          "halt - terminate shell process\n"\
          "help - display built-in commands\n"\
          "date - display current date and time\n"\
          "env - display environment variables\n"\
          "path - display current search path\n"\
          "cwd - display absolute pathname of current working directory\n"\
          "cd - manage current working directory\n"\
          "set - manage environment variables\n"\
          "import - read commands from a file\n");

    }
    
    /* Display date and time */
    else if ( (strcmp(tok, "date") == 0) && (length <= 4) )
    {
      curTime();
    }

    /* Display user environment variables */
    else if ( (strcmp(tok, "env") == 0) && (length <= 3) )
    {
      for (int i = 0; environ[i] != NULL; i++)
      {
        cout << environ[i] << '\n';
        
      }
    }
    
    /* Display current search path */
    else if ( (strcmp(tok, "path") == 0) && (length <= 4) )
    {
      int cnt;
      
      for (cnt = 0; envp[cnt] != NULL; cnt++)
      {
        if (strncmp(envp[cnt], "PATH", 4) == 0)
        {
          printf("%s", envp[cnt]+5);
          cout << "\n";
        }
      }
    }
    
    /* Display absolute pathname of current working directory */
    else if ( (strcmp(tok, "cwd") == 0) && (length <= 3) )
    {
      char dir[200];
      if (getcwd(dir, 200) != NULL)
      {
        cout << dir << "\n";
      }

      else
      {
        perror("Error: ");
      }
    }

    /* Change working directory to current users home directory */
    else if ( (strcmp(tok, "cd") == 0) && (length == 2) )
    {
      chdir("..");
      chdir(getlogin());

      /* Change PWD variable */
      char dir[200];
      getcwd(dir, 200);
      char const* pwd = "PWD";
      setenv(pwd, dir, 1);
    }
    
    /* Change working directory to home directory of \
       current user if no user is specified */
    else if ( (strcmp(tok, "cd") == 0) && (length == 4) )
    {
      chdir("..");
      chdir(getlogin());

      /* Change PWD variable */
      char dir[200];
      getcwd(dir, 200);
      char const* pwd = "PWD";
      setenv(pwd, dir, 1);
    }


    /* Change working directory to pathname supplied by user or \
       home of specified user*/
    else if ( (strcmp(tok, "cd") == 0) && (length >= 3) )
    {
      /* Get next token */
      tok = strtok(NULL, " ");
      
      while (tok != NULL)
      {
        if (tok[0] != '~')
        {
          chdir(tok);
        
          /* Change PWD variable */
          char dir[200];
          getcwd(dir, 200);
          char const* pwd = "PWD";
          setenv(pwd, dir, 1);
      
          break;
        }
        /* Change directory to home of specified user */
        else
        {
          chdir("/user");
          char* temp = tok + 1;
          chdir(temp);
          
          /* Change PWD variable */
          char dir[200];
          getcwd(dir, 200);
          char const* pwd = "PWD";
          setenv(pwd, dir, 1);
          
          break;
        }
      }
    }

    /* Set or Remove environment variable */    
    else if ( (strcmp(tok, "set") == 0) )
    {
      /* Get next token */
      tok = strtok(NULL, " ");

      char* var = tok;
      
      tok = strtok(NULL, " ");
      
      if (tok != NULL)
      {
        char* val = tok;
        setenv(var, val, 1);
      }

      else
      {
        unsetenv(var);
      }
    }   
    
    /* Import and read commands from file */
    else if ( (strcmp(tok, "import") == 0) )
    {
      /* Get next token */
      tok = strtok(NULL, " ");

      char* line = NULL;
      size_t buf = 0;
      FILE* fp = fopen(tok, "r");
      ssize_t num;
      pthread_t tid;
      int cmd;

      /* File successfully opend */
      if (fp != NULL)
      {
        /* Get each line from file */
        while ( (num = getline(&line, &buf, fp)) != -1 )
        {
          /* Output each command read from file */
          cout << line;

          /* Create thread */
          cmd = pthread_create(&tid, NULL, newProcess, (void *) line);
          pthread_join(tid, NULL);
          
          if (cmd != 0)
          {
            printf("Error; Command not recognized");
          }
          
          i++; 
          
        }
        fclose(fp);
      }
      
      /* Error opening file */
      else
      {
        cout << "Error opening file. Check filename and try again." << "\n";
      }

    }

    /* Execute external command */
    else
    {
      pthread_t tid; 
      int cmd;
      
      /* Create thread */
      cmd = pthread_create(&tid, NULL, command, (void *) full);
      pthread_join(tid, NULL);

      if (cmd != 0)
      {
        printf("Error; Command not recognized. Enter valid command"\
            " and try again.");
      }
    }

  }
  return 0;
}
