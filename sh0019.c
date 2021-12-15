#include "sh0019.h"
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>


// struct command
//    Data structure describing a command. Add your own stuff.

typedef struct command command;
struct command {
    int argc;      // number of arguments
    char** argv;   // arguments, terminated by NULL
    pid_t pid;     // process ID running this command, -1 if none
    int* type;
    command* next;
    int status;
    int fd0;
    int fd1;
    int executed;
};


// command_alloc()
//    Allocate and return a new command structure.

static command* command_alloc(void) {
    command* c = (command*) malloc(sizeof(command));
    c->argc = 0;
    c->argv = NULL;
    c->pid = -1;
    c->type = NULL;
    c->next = NULL;
    c->status = 0;
    c->fd0 = 0;
    c->fd1 = 0;
    c->executed = 0;
    return c;
}


// command_free(c)
//    Free command structure `c`, including all its words.

static void command_free(command* c) {
    for (int i = 0; i != c->argc; ++i) {
        free(c->argv[i]);
    }
    free(c->argv);
    free(c);
}


// command_append_arg(c, word)
//    Add `word` as an argument to command `c`. This increments `c->argc`
//    and augments `c->argv`.

static void command_append_arg(command* c, char* word, int type) {
    c->type = (int*) realloc(c->type, sizeof(int) * (c->argc + 1));
    c->type[c->argc] = type;
    if(!type){
        c->argv = (char**) realloc(c->argv, sizeof(char*) * (c->argc + 2));
        c->argv[c->argc] = word;
        c->argv[c->argc + 1] = NULL;
        ++c->argc;
    }
}

// COMMAND EVALUATION

// start_command(c, pgid)
//    Start the single command indicated by `c`. Sets `c->pid` to the child
//    process running the command, and returns `c->pid`.
//
//    PART 1: Fork a child process and run the command using `execvp`.
//    PART 5: Set up a pipeline if appropriate. This may require creating a
//       new pipe (`pipe` system call), and/or replacing the child process's
//       standard input/output with parts of the pipe (`dup2` and `close`).
//       Draw pictures!
//    PART 7: Handle redirections.
//    PART 8: The child process should be in the process group `pgid`, or
//       its own process group (if `pgid == 0`). To avoid race conditions,
//       this will require TWO calls to `setpgid`.

// void recursivePipe(command* current, int fd1, int fd2){
//   int pfds[] = {fd1, fd2};
//   current -> pid = fork();
//   fprintf(stdout, "%d\n", pfds[0]);
//   if (current -> pid == 0 && pfds[0] == 0){
//       fprintf(stdout, "hello\n");
//       pipe(pfds);
//       recursivePipe(current -> next, pfds[0], pfds[1]);
//
//   }else if (current -> pid == 0 && pfds[0] != 0){
//     //we will always enter this else statement as there are always two commands in a pipe
//       if (current -> next){
//           close(pfds[1]);
//           dup2(pfds[0], 0);
//           close(pfds[0]);
//           int p[2];
//           pipe(p);
//           recursivePipe(current ->next, p[0], p[1]);
//       }else{
//           close(pfds[1]);
//           dup2(pfds[0], 0);
//           close(pfds[0]);
//           execvp(*(current->argv), current->argv);
//       }
//   }else{
//     //we are not in the child process
//       close(pfds[0]);
//       dup2(pfds[1], 1);
//       close(pfds[1]);
//       execvp(*(current->argv), current->argv);
//
//   }
//
// }
void handler(int sig) {
    (void) sig;
    int olderrno = errno;
    printf("\nsh0019[%d]$ ", getpid());
    fflush(stdout);
    errno = olderrno;
}

void handler2(int sig){
    (void) sig;
    printf("000\n");
}

// command* iterativePipe(command* current){
//     command* next = current->next;
//     command* nextnext;
//     //int mainPid = getpid();
//     //signal(SIGINT, handler2);
//     int status = 0;
//     current->pid = fork();
//     if(current->pid == 0){
//         int pfds[2];
//         pipe(pfds);
//         current -> fd0 = pfds[0];
//         current -> fd1 = pfds[1];
//         while(next){
//             next -> pid = fork();
//             if (next -> pid == 0){
//                 close(current -> fd1);
//                 dup2(current -> fd0, 0);
//                 close(current -> fd0);
//                 nextnext = next->next;
//                 if(nextnext && *(next->type + next->argc) == TOKEN_PIPE){
//                     pipe(pfds);
//                     next -> fd0 = pfds[0];
//                     next -> fd1 = pfds[1];
//                 }else if(*(next->type + next->argc) != TOKEN_PIPE){
//                     execvp(*(next->argv), next->argv);
//                 }
//             }else{
//                 close(current -> fd0);
//                 dup2(current -> fd1, 1);
//                 close(current -> fd1);
//                 execvp(*(current->argv), current->argv);
//             }
//             current = next;
//             next = nextnext;
//         }
//     }
//     waitpid(current -> pid, &status, 0);
//     while(next){
//         next -> status = status;
//         if(*(next->type + next->argc) == TOKEN_PIPE){
//             next -> status = status;
//             next = next -> next;
//         }else{
//           fprintf(stdout, "%d\n", next->status);
//             return next;
//         }
//     }
// }
void redir(command* c){
  command* temp = c;
  int numOfFiles = 0; // start at one to account for the last file
  while(*(temp->type + temp->argc) == TOKEN_REDIRECTION_ || *(temp->type + temp->argc) == TOKEN_REDIRECTION_2 || *(temp->type + temp->argc) == _TOKEN_REDIRECTION){
    numOfFiles++;
    temp = temp -> next;
  }
  int fd[numOfFiles];
  int status = 0;
  command* prev = c;
  command* current = c;
  command* next = c -> next;
  //printf("n:%d\n", numOfFiles);

    for(int i = 0; i<numOfFiles; i++){
        //current->pid = fork();
        //printf("%d\n", current->pid);
        //if(current->pid == 0){
        if(*(current->type + current->argc) == TOKEN_REDIRECTION_){
          fd[i] = open(*(next->argv), O_WRONLY | O_CREAT, 0666);
          if (fd[i] == -1) {
            perror(strerror(errno));
            exit(EXIT_FAILURE);
          }
          dup2(fd[i], 1);
        }else if(*(current->type + current->argc) == _TOKEN_REDIRECTION){
          fd[i] = open(*(next->argv), O_RDONLY);
          dup2(fd[i], 0);
          if (fd[i] == -1) {
            printf("No such file or directory\n");
            exit(EXIT_FAILURE);
          }
        }else if(*(current->type + current->argc) == TOKEN_REDIRECTION_2){
          fd[i] = open(*(next->argv), O_WRONLY | O_CREAT, 0666);
          dup2(fd[i], 2);
          if (fd[i] == -1) {
            perror(strerror(errno));
            exit(EXIT_FAILURE);
          }
        }
        if(*(next->type + next->argc) == TOKEN_REDIRECTION_ || *(next->type + next->argc) == TOKEN_REDIRECTION_2 || *(next->type + next->argc) == _TOKEN_REDIRECTION){
          *(current->type + current->argc) = *(next->type + next->argc);
        }
        if(next->next){
          //printf("fsdfs\n");
          next = next -> next;
        }
    }
}
command* iterativePipeline(command* c){
    command* temp = c;
    while(*(temp->type + temp->argc) == TOKEN_REDIRECTION_ || *(temp->type + temp->argc) == TOKEN_REDIRECTION_2 || *(temp->type + temp->argc) == _TOKEN_REDIRECTION){
      temp = temp -> next;
    }
    int numOfFiles = 0;
    while(*(temp->type + temp->argc) == TOKEN_PIPE){
      numOfFiles++;
      temp = temp -> next;
    }
    int fd[numOfFiles][2];
    int status = 0;
    command* prev = c;
    command* current = c;
    command* next = c -> next;
    //printf("%d\n", numOfFiles);
    pid_t pid = fork();
    if(pid == 0){
      for(int i = 0; i<numOfFiles; i++){
          pipe(fd[i]);
      }
      for(int i = 0; i<numOfFiles; i++){
          current->pid = fork();
          if(current->pid ==0){
              close(fd[i][0]);
              dup2(fd[i][1], 1);
              close(fd[i][1]);
              execvp(*(current->argv), current->argv);
          }else{
              waitpid(current->pid, &status, 0);
              //printf("%s\n", *(next->argv));
              close(fd[i][1]);
              dup2(fd[i][0], 0); // now 0 aka STDIN comes from the pipe
              close(fd[i][0]);

              //this is the parent process so changes made here will be seen by future forks
              while(*(current->type + current->argc) == TOKEN_REDIRECTION_ || *(current->type + current->argc) == TOKEN_REDIRECTION_2 || *(current->type + current->argc) == _TOKEN_REDIRECTION){
                current = next;
                next = next ->next;
              }
              current = next;
              next = next -> next;
              //before we execute this instruction we need to setup the funnel for the next instruction
              //execvp(*(next->argv), next->argv);
              if(*(current->type + current->argc) == TOKEN_REDIRECTION_ || *(current->type + current->argc) == TOKEN_REDIRECTION_2 || *(current->type + current->argc) == _TOKEN_REDIRECTION){
                redir(current);
              }
          }
      }
      execvp(*(current->argv), current->argv);
    }else{
      //printf("%d\n", numOfFiles);
      waitpid(pid, &status, 0);
    }
    //printf("%s\n", *(current->argv));
    if(*(temp->type + temp->argc) == TOKEN_AND && !status){
        temp -> status = status;
    }else if(*(temp->type + temp->argc) == TOKEN_AND && status){
        temp -> status = status;
    }else if(*(temp->type + temp->argc) == TOKEN_OR && !status){
        temp -> status = !status;
    }else if(*(temp->type + temp->argc) == TOKEN_OR && status){
        temp -> status = !status;
    }
    return temp;


}
command* iterativeRedir(command* c){
    command* temp = c;
    int numOfFiles = 0; // start at one to account for the last file
    while(*(temp->type + temp->argc) == TOKEN_REDIRECTION_ || *(temp->type + temp->argc) == TOKEN_REDIRECTION_2 || *(temp->type + temp->argc) == _TOKEN_REDIRECTION){
      numOfFiles++;
      temp = temp -> next;
    }
    int fd[numOfFiles];
    int status = 0;
    command* prev = c;
    command* current = c;
    command* next = c -> next;
    //printf("n:%d\n", numOfFiles);
    pid_t pid = fork();
    if(pid == 0){
      for(int i = 0; i<numOfFiles; i++){
          //current->pid = fork();
          //printf("%d\n", current->pid);
          //if(current->pid == 0){
            if(*(current->type + current->argc) == TOKEN_REDIRECTION_){
              fd[i] = open(*(next->argv), O_WRONLY | O_CREAT, 0666);
              if (fd[i] == -1) {
                perror(strerror(errno));
                exit(EXIT_FAILURE);
              }
              dup2(fd[i], 1);
            }else if(*(current->type + current->argc) == _TOKEN_REDIRECTION){
              fd[i] = open(*(next->argv), O_RDONLY);
              dup2(fd[i], 0);
              if (fd[i] == -1) {
                printf("No such file or directory\n");
                exit(EXIT_FAILURE);
              }
            }else if(*(current->type + current->argc) == TOKEN_REDIRECTION_2){
              fd[i] = open(*(next->argv), O_WRONLY | O_CREAT, 0666);
              dup2(fd[i], 2);
              if (fd[i] == -1) {
                perror(strerror(errno));
                exit(EXIT_FAILURE);
              }
            }
            if(*(next->type + next->argc) == TOKEN_REDIRECTION_ || *(next->type + next->argc) == TOKEN_REDIRECTION_2 || *(next->type + next->argc) == _TOKEN_REDIRECTION){
              *(current->type + current->argc) = *(next->type + next->argc);
            }
            if(*(next->type + next->argc) == TOKEN_PIPE){
                next = iterativePipeline(current);
                _exit(next->status);
            }
            if(next->next){
              //printf("fsdfs\n");
              next = next -> next;
            }
        }
        execvp(*(current->argv), current->argv);

    }else{
      waitpid(pid, &status, 0);
    }

    while(*(temp->type + temp->argc) == TOKEN_PIPE){
      temp = temp->next;
    }
    if(*(temp->type + temp->argc) == TOKEN_AND && !status){
        temp -> status = status;
    }else if(*(temp->type + temp->argc) == TOKEN_AND && status){
        temp -> status = status;
    }else if(*(temp->type + temp->argc) == TOKEN_OR && !status){
        temp -> status = !status;
    }else if(*(temp->type + temp->argc) == TOKEN_OR && status){
        temp -> status = !status;
    }

    return temp;

}

command* start_command(command* c, pid_t pgid, int pip) {
    (void) pgid;
    // Your code here!
    command* current = c;
    if (pip == 1){
        command* next = iterativePipeline(current);
        //fprintf(stdout, "%d\n", *(next->type + next->argc));
        return next;
    }else if(pip == 2){
        command* next = iterativeRedir(current);
        return next;
    }else{
        c->pid = fork();
        setpgid(c->pid, c->pid);
        if (c->pid == 0) {
          //fprintf(stdout,"%d\n", c->pid);
          execvp(*(c->argv), c->argv);
        }
    }

    return c;
}


// run_list(c)
//    Run the command list starting at `c`.
//
//    PART 1: Start the single command `c` with `start_command`,
//        and wait for it to finish using `waitpid`.
//    The remaining parts may require that you change `struct command`
//    (e.g., to track whether a command is in the background)
//    and write code in run_list (or in helper functions!).
//    PART 2: Treat background commands differently.
//    PART 3: Introduce a loop to run all commands in the list.
//    PART 4: Change the loop to handle conditionals.
//    PART 5: Change the loop to handle pipelines. Start all processes in
//       the pipeline in parallel. The status of a pipeline is the status of
//       its LAST command.
//    PART 8: - Choose a process group for each pipeline.
//       - Call `claim_foreground(pgid)` before waiting for the pipeline.
//       - Call `claim_foreground(0)` once the pipeline is complete.

void run_list(command* c) {
    command* current = c;
    command* prev = c;
    int status = 0;
    pid_t pid = 0;
    //start_command(current, 0);
    while(current){
        int pip = 0;
        if ((WIFSIGNALED(status) != 0) && (WTERMSIG(status) == SIGINT)){
          break;
        }
        if (!strcmp(current->argv[0], "cd")) {
            if(!chdir(current->argv[1])){
              current = current->next;
              continue;
            }
        }
        if(*(current->type + current->argc) == TOKEN_PIPE){
          if((*(prev->type + prev->argc) == TOKEN_AND || *(prev->type + prev->argc) == TOKEN_OR) && prev -> status != 0){
            while(*(current->type + current->argc) == TOKEN_PIPE){
              //because pipes have higher precedence, if there was an and that returned false on the right side and the pipe was on the left side, we don't need to execute any pipes
              prev = current;
              current = current -> next;
            }
            prev = current;
            current = current -> next;
            continue;
          }
          pip = 1;
          prev = start_command(current, 0, pip);
          //prev->status = 0;
          current = prev -> next;
          continue;
        }
        if(*(current->type + current->argc) == TOKEN_REDIRECTION_ || *(current->type + current->argc) == _TOKEN_REDIRECTION || *(current->type + current->argc) == TOKEN_REDIRECTION_2){
            pip = 2;
            prev = start_command(current, 0, pip);
            current = prev -> next;
            continue;
        }

        if (*(current->type + current->argc) == TOKEN_BACKGROUND){
          if (!(prev -> status)){
            pip = 0;
            start_command(current, 0, pip);
          }
            //fprintf(stdout, "copied\n");
        }else{
            pip = 0;
            if (!(prev -> status)){
                //printf("Im here for the token miss\n");
                start_command(current, 0, pip);
                claim_foreground(current->pid);
                waitpid(current->pid, &status, 0);
                claim_foreground(0);
                current -> status = status;
                //fprintf(stdout, "yy:%d\n", current);

                // if (current -> status && *(current->type + current->argc) == TOKEN_AND){
                //     current -> next = NULL;
                // }
                if (current -> status && *(current->type + current->argc) == TOKEN_OR){
                    current -> status = 0;
                }
                else if (!(current -> status) && *(current->type + current->argc) == TOKEN_OR){
                    current -> status = 1;
                }
                if (current -> next){
                    prev = current;
                }
                // fprintf(stdout, "s:%d\n", current->status);
            }else{
                if(*(current->type + current->argc) == TOKEN_OR && *(prev->type + prev->argc) == TOKEN_AND){
                //if the previous token was an AND and we are in this "else" branch then it means the first part evaluated to false
                //this means the whole thing is false. if the whole thing is false and we have an OR operation next, then we must
                //run the next instruction to see if we get a true value.
                    current -> status = 0;
                    prev = current;
                }
                else if(*(current->type + current->argc) == TOKEN_AND && *(prev->type + prev->argc) == TOKEN_OR){
                //if the previous token was an OR and we are in this "else" branch then it means the first part evaluated to true
                //when it evaluates to true we don't run the second instruction. because either way we get true. if the next instruction
                // is AND we set this instruction to be a true value and run the next one.
                    current -> status = 0;
                    prev = current;
                }
                else if(*(current->type + current->argc) == TOKEN_OR && *(prev->type + prev->argc) == TOKEN_OR){
                    //If the previouse token was an OR and we are in this "else" branch then it means the first part evaluated to true
                    current -> status = 1; // we aren't going to run the next instruction
                    prev = current;
                }
                else if(*(current->type + current->argc) == TOKEN_AND && *(prev->type + prev->argc) == TOKEN_AND){
                    //If the previouse token was an AND and we are in this "else" branch then it means the first part evaluated to false
                    current -> status = 1; // we aren't going to run the next instruction
                    prev = current;
                }
            }
            // else if (prev -> status && current -> next){
            //
            //
            // }
        }
        current = current -> next;

    }


    //fprintf(stderr, "run_command not done yet\n");
}


// eval_line(c)
//    Parse the command list in `s` and run it via `run_list`.

void eval_line(const char* s) {
    int prevtype = 0;
    int type;
    char* token;
    // Your code here!

    // build the command
    command* first = command_alloc();
    command* current = first;
    while ((s = parse_shell_token(s, &type, &token)) != NULL) {
      if(prevtype){
        current -> next = command_alloc();
        current = current -> next;
      }
      if(type == 1){
        if(!strcmp(token, ">")){
           type = TOKEN_REDIRECTION_;
        }else if(!strcmp(token, "2>")){
           type = TOKEN_REDIRECTION_2;
        }else{
           type = _TOKEN_REDIRECTION;
        }
      }
      command_append_arg(current, token, type);
      prevtype = type;
    }
    // fprintf(stderr, "%s\n", *(c->argv + c->argc -1));
    // execute it
    if (first->argc) {
        run_list(first);
    }
    command_free(first);
}


int main(int argc, char* argv[]) {
    FILE* command_file = stdin;
    int quiet = 0;

    // Check for '-q' option: be quiet (print no prompts)
    if (argc > 1 && strcmp(argv[1], "-q") == 0) {
        quiet = 1;
        --argc, ++argv;
    }

    // Check for filename option: read commands from file
    if (argc > 1) {
        command_file = fopen(argv[1], "rb");
        if (!command_file) {
            perror(argv[1]);
            exit(1);
        }
    }

    // - Put the shell into the foreground
    // - Ignore the SIGTTOU signal, which is sent when the shell is put back
    //   into the foreground
    claim_foreground(0);
    set_signal_handler(SIGTTOU, SIG_IGN);
    signal(SIGINT, handler);

    char buf[BUFSIZ];
    int bufpos = 0;
    int needprompt = 1;

    while (!feof(command_file)) {
        // Print the prompt at the beginning of the line
        if (needprompt && !quiet) {
            printf("sh0019[%d]$ ", getpid());
            fflush(stdout);
            needprompt = 0;
        }

        // Read a string, checking for error or EOF
        if (fgets(&buf[bufpos], BUFSIZ - bufpos, command_file) == NULL) {
            if (ferror(command_file) && errno == EINTR) {
                // ignore EINTR errors
                clearerr(command_file);
                buf[bufpos] = 0;
            } else {
                if (ferror(command_file)) {
                    perror("sh0019");
                }
                break;
            }
        }

        // If a complete command line has been provided, run it
        bufpos = strlen(buf);
        if (bufpos == BUFSIZ - 1 || (bufpos > 0 && buf[bufpos - 1] == '\n')) {
            eval_line(buf);
            bufpos = 0;
            needprompt = 1;
        }

        // Handle zombie processes and/or interrupt requests
        // Your code here!
    }

    return 0;
}

        //fprintf(stdout, "%d\n", *(current->type + current->argc));
        // command* current = c;
        // command* next = c -> next;
        // command* temp = next -> next;
        // current -> pid = fork();
        // if (current->pid == 0){
        //     int pfds[2];
        //     pipe(pfds);
        //     next -> pid = fork();
        //     if (next -> pid == 0){
        //
        //         close(pfds[1]);
        //         dup2(pfds[0], 0);
        //         close(pfds[0]);
        //         int p[2];
        //         pipe(p);
        //         if (temp != NULL){
        //           temp -> pid = fork();
        //           if (temp -> pid == 0){
        //               close(p[1]);
        //               dup2(p[0], 0);
        //               close(p[0]);
        //               execvp(*(temp->argv), temp->argv);
        //           }else{
        //               close(p[0]);
        //               dup2(p[1], 1);
        //               close(p[1]);
        //               execvp(*(next->argv), next->argv);
        //           }
        //         }else{
        //           execvp(*(next->argv), next->argv);
        //         }
        //     }else{
        //         close(pfds[0]);
        //         dup2(pfds[1], 1);
        //         close(pfds[1]);
        //         execvp(*(current->argv), current->argv);
        //     }
        // }

        // int pfds[2];
        // pipe(pfds);
        // command* current = c;
        // command* next = c -> next;
        //
        // current -> pid = fork();
        // //execvp(*(c->argz), c->argz);
        // if (current->pid == 0){
        //     next -> pid = fork();
        //     if (next -> pid == 0){
        //         close(pfds[1]);
        //         dup2(pfds[0], 0);
        //         close(pfds[0]);
        //         execvp(*(next->argv), next->argv);
        //     }else{
        //         close(pfds[0]);
        //         dup2(pfds[1], 1);
        //         close(pfds[1]);
        //         execvp(*(current->argv), current->argv);
        //     }
        // }
