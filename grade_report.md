## COMP0019 CW5 Grade Report

Graded at: 2020-04-19 20:49:41.106250

Graded for revision:  158717d16b77e79727c165579bfbaf28617fb7a1

### Output


    CLEAN 
      COMPILE sh0019.c
    sh0019.c: In function ‘redir’:
    sh0019.c:190:12: warning: unused variable ‘prev’ [-Wunused-variable]
       command* prev = c;
                ^
    sh0019.c:189:7: warning: unused variable ‘status’ [-Wunused-variable]
       int status = 0;
           ^
    sh0019.c: In function ‘iterativePipeline’:
    sh0019.c:242:14: warning: unused variable ‘prev’ [-Wunused-variable]
         command* prev = c;
                  ^
    sh0019.c: In function ‘iterativeRedir’:
    sh0019.c:307:14: warning: unused variable ‘prev’ [-Wunused-variable]
         command* prev = c;
                  ^
    sh0019.c: In function ‘run_list’:
    sh0019.c:420:11: warning: unused variable ‘pid’ [-Wunused-variable]
         pid_t pid = 0;
               ^
    sh0019.c: In function ‘iterativePipeline’:
    sh0019.c:249:11: warning: ignoring return value of ‘pipe’, declared with attribute warn_unused_result [-Wunused-result]
               pipe(fd[i]);
               ^
      COMPILE helpers.c
      LINK sh0019 
    
    Test SIMPLE1: passed
    Test SIMPLE2: passed
    Test SIMPLE3: passed
    Test SIMPLE4: passed
    Test BG1: passed
    Test BG2: passed
    Test BG3: passed
    Test LIST1: passed
    Test LIST2: passed
    Test LIST3: passed
    Test LIST4: passed
    Test LIST5: passed
    Test LIST6: passed
    Test LIST7: passed
    Test LIST8: passed
    Test COND1: passed
    Test COND2: passed
    Test COND3: passed
    Test COND4: passed
    Test COND5: passed
    Test COND6: passed
    Test COND7: passed
    Test COND8: FAILED in 0.304 sec
        command  `sleep 0.2 && echo Second & sleep 0.1 && echo First`
        expected `First Second`
        got      `Second First`
    Test COND9: passed
    Test COND10: passed
    Test COND11: passed
    Test PIPE1: passed
    Test PIPE2: passed
    Test PIPE3: passed
    Test PIPE4: passed
    Test PIPE5: passed
    Test PIPE6: passed
    Test PIPE7: passed
    Test PIPE8: passed
    Test PIPE9: passed
    Test PIPE10: passed
    Test PIPE11: passed
    Test PIPE12: passed
    Test PIPE13: passed
    Test PIPE14: passed
    Test PIPE15: passed
    Test PIPE16: passed
    Test PIPE17: passed
    Test PIPE18: passed
    Test PIPE19: passed
    Test PIPE20: passed
    Test PIPE21: passed
    Test PIPE22: passed
    Test PIPE23: FAILED in 0.310 sec
        command  `sleep 0.2 | wc -c | sed s/0/Second/ & sleep 0.1 | wc -c | sed s/0/First/`
        expected `First Second`
        got      `Second First`
    Test ZOMBIE1: FAILED in 0.114 sec
        command  `sleep 0.05 & \n sleep 0.1 \n ps T`
        expected ``
        got      `2898 pts/0 Z 0:00 [sleep] <defunct>`
    Test ZOMBIE2: FAILED in 0.157 sec
        command  `sleep 0.05 & sleep 0.05 & sleep 0.05 & sleep 0.05 & \n sleep 0.07 \n sleep 0.07 \n ps T`
        expected ``
        got      `2908 pts/0 Z 0:00 [sleep] <defunct> 2909 pts/0 Z 0:00 [sleep] <defunct> 2910...`
    Test REDIR1: passed
    Test REDIR2: passed
    Test REDIR3: passed
    Test REDIR4: passed
    Test REDIR5: passed
    Test REDIR6: passed
    Test REDIR7: passed
    Test REDIR8: passed
    Test REDIR9: passed
    Test REDIR10: passed
    Test REDIR11: passed
    Test REDIR12: passed
    Test REDIR13: passed
    Test REDIR14: passed
    Test REDIR15: passed
    Test REDIR16: FAILED in 0.005 sec
        command  `echo Ignored | cat < lower.txt | tr A-Z a-z`
        expected `lower`
        got      `LOWER`
    Test INTR1: passed
    Test INTR2: passed
    Test INTR3: FAILED in 0.101 sec
        command  `sleep 0.3 && echo yes & sleep 0.2 && echo no`
        expected `yes`
        got      ``
    Test INTR4: passed
    Test INTR5: passed
    Test CD1: passed
    Test CD2: passed
    Test CD3: passed
    Test CD4: passed
    Test CD5: passed
    Test CD6: passed
    Test CD7: passed
    Test CD8: passed
    Test ADVPIPE1: FAILED in 10.217 sec
        command  `yes | head -n 5`
        expected `y y y y y`
        got      ``
      timeout after 10.00s
    Test ADVBGCOND1: passed
    Test ADVBGCOND2: FAILED in 0.260 sec
        command  `echo first && sleep 0.1 && echo third & sleep 0.05 ; echo second ; sleep 0.1 ; echo fourth`
        expected `first second third fourth`
        got      `first third second fourth`
    
    75 of 83 tests passed


### Marking

Total score: (90/100)

