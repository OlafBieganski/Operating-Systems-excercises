# Sprawozdanie lista nr 3

## Zadanie 1
```bash
olaf@LAPTOP-K5EHOLM7:~/scr-laboratorium/lista_4$ top
top - 19:01:36 up  1:08,  0 users,  load average: 0.15, 0.03, 0.01
Tasks:  27 total,   2 running,  25 sleeping,   0 stopped,   0 zombie
%Cpu(s): 16.7 us,  0.1 sy,  0.0 ni, 83.3 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st
MiB Mem :  12261.5 total,  11645.5 free,    344.6 used,    271.4 buff/cache
MiB Swap:   4096.0 total,   4096.0 free,      0.0 used.  11675.0 avail Mem

  PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND
  905 olaf      20   0    2360    580    512 R 100.0   0.0   0:05.78 a.out
  746 olaf      20   0  997980 129708  37220 S   0.7   1.0   0:04.64 node
    1 root      20   0     904    528    468 S   0.0   0.0   0:00.02 init
    7 root      20   0     896     88     20 S   0.0   0.0   0:00.00 init
    8 root      20   0     896     88     20 S   0.0   0.0   0:01.27 init
    9 olaf      20   0   10788   5788   3348 S   0.0   0.0   0:00.13 bash
  129 root      20   0    1252    360     20 S   0.0   0.0   0:00.00 init
  130 root      20   0    1260    360     20 S   0.0   0.0   0:00.00 init
  131 olaf      20   0    2612    596    528 S   0.0   0.0   0:00.00 sh
  132 olaf      20   0    2612    532    460 S   0.0   0.0   0:00.00 sh
  137 olaf      20   0    2612    592    524 S   0.0   0.0   0:00.00 sh
  141 olaf      20   0  945384  86056  35148 S   0.0   0.7   0:09.07 node
  161 olaf      20   0  624636  47796  32144 S   0.0   0.4   0:00.38 node
  644 root      20   0     904     96     20 S   0.0   0.0   0:00.00 init
  645 root      20   0     904     96     20 S   0.0   0.0   0:00.00 init
  646 olaf      20   0   10168   5128   3364 S   0.0   0.0   0:00.02 bash
  715 root      20   0     904     96     20 S   0.0   0.0   0:00.00 init
  716 root      20   0     904     96     20 S   0.0   0.0   0:00.08 init
  717 olaf      20   0  589640  41768  26464 S   0.0   0.3   0:00.23 node
  724 root      20   0     904     96     20 S   0.0   0.0   0:00.00 init
  725 root      20   0     904     96     20 S   0.0   0.0   0:00.09 init
  726 olaf      20   0  583572  35928  28344 S   0.0   0.3   0:00.31 node
  733 olaf      20   0  966576  45092  32140 S   0.0   0.4   0:00.16 node
olaf@LAPTOP-K5EHOLM7:~/scr-laboratorium/lista_4$ kill -2 905
olaf@LAPTOP-K5EHOLM7:~/scr-laboratorium/lista_4$ ps
  PID TTY          TIME CMD
    9 pts/0    00:00:00 bash
  907 pts/0    00:00:00 ps
[1]+  Interrupt               ./a.out
olaf@LAPTOP-K5EHOLM7:~/scr-laboratorium/lista_4$ gcc inf.c
olaf@LAPTOP-K5EHOLM7:~/scr-laboratorium/lista_4$ ls -l
total 24
-rwxr-xr-x 1 olaf olaf 16696 Nov  8 18:49 a.out
-rw-r--r-- 1 olaf olaf   213 Nov  8 18:34 inf.c
olaf@LAPTOP-K5EHOLM7:~/scr-laboratorium/lista_4$ ./a.out &
[1] 696
olaf@LAPTOP-K5EHOLM7:~/scr-laboratorium/lista_4$ top
top - 18:50:11 up 56 min,  0 users,  load average: 0.00, 0.00, 0.00
Tasks:  26 total,   1 running,  25 sleeping,   0 stopped,   0 zombie
%Cpu(s):  0.0 us,  0.1 sy,  0.0 ni, 99.9 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st
MiB Mem :  12261.5 total,  11659.6 free,    353.4 used,    248.5 buff/cache
MiB Swap:   4096.0 total,   4096.0 free,      0.0 used.  11666.5 avail Mem

  PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND
  696 olaf      20   0    2360    508    444 S   0.3   0.0   0:00.08 a.out
    1 root      20   0     904    528    468 S   0.0   0.0   0:00.02 init
    7 root      20   0     896     88     20 S   0.0   0.0   0:00.00 init
    8 root      20   0     896     88     20 S   0.0   0.0   0:01.22 init
    9 olaf      20   0   10788   5788   3348 S   0.0   0.0   0:00.12 bash
  129 root      20   0    1252    360     20 S   0.0   0.0   0:00.00 init
  130 root      20   0    1260    360     20 S   0.0   0.0   0:00.00 init
  131 olaf      20   0    2612    596    528 S   0.0   0.0   0:00.00 sh
  132 olaf      20   0    2612    532    460 S   0.0   0.0   0:00.00 sh
  137 olaf      20   0    2612    592    524 S   0.0   0.0   0:00.00 sh
  141 olaf      20   0  943792  83692  35148 S   0.0   0.7   0:06.59 node
  152 root      20   0    1260    360     20 S   0.0   0.0   0:00.00 init
  153 root      20   0    1260    368     20 S   0.0   0.0   0:00.22 init
  154 olaf      20   0  589664  46352  29340 S   0.0   0.4   0:00.59 node
  161 olaf      20   0  624636  46996  32144 S   0.0   0.4   0:00.32 node
  191 root      20   0    1260    360     20 S   0.0   0.0   0:00.00 init
  192 root      20   0    1260    368     20 S   0.0   0.0   0:00.74 init
  193 olaf      20   0  585508  43136  29572 S   0.0   0.3   0:01.82 node
  200 olaf      20   0 1000056 137640  37096 S   0.0   1.1   0:10.27 node
  213 olaf      20   0 1760088  43140  13704 S   0.0   0.3   0:03.84 cpptools
  268 olaf      20   0  835504  43220  31864 S   0.0   0.3   0:00.36 node
  304 olaf      20   0  902608  20724   9264 S   0.0   0.2   0:01.25 cpptools-srv
  644 root      20   0     904     96     20 S   0.0   0.0   0:00.00 init
```

```c
#include <time.h>

int main(){
    int i = 0;
    struct timespec rmtp, rqtp;
    rqtp.tv_sec = 0;
    rqtp.tv_nsec = 10000000;
    while(1){
        i++;
        //nanosleep(&rqtp, &rmtp);
    }
}
```

>Po zastosowaniu w programie inf.c funkcji nanosleep() udało się znacząco zredukować zużycie CPU przez proces. W oby przypadkach pojawia się on na początku listy ponieważ na komputerze nie były uruchomione w tym czasie inne programy potrzebujące dużej mocy obliczeniowej. Jeżeli by tak było program pojawiłby się niżej na liście. W celu zmiany częstotliwości odświeżania należy wywołać polecenie top w formacie
```bash
top -u [seconds].[tenths of seconds]
#czyli dla 1.1 s
top - u 1.1
```

## Zadanie 2

Poniżej widoczny jest terminal, z którego wysyłano sygnały do programu.

```sh
olaf@LAPTOP-K5EHOLM7:~$ ps -u
USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
olaf         9  0.0  0.0  10788  5836 pts/0    Ss   17:53   0:00 -bash
olaf       131  0.0  0.0   2612   596 pts/1    Ss+  18:03   0:00 sh -c $VSC
olaf       132  0.0  0.0   2612   532 pts/1    S+   18:03   0:00 sh /mnt/c/U
olaf       137  0.0  0.0   2612   592 pts/1    S+   18:03   0:00 sh /home/ol
olaf       141  0.1  0.6 943284 86036 pts/1    Sl+  18:03   0:13 /home/olaf/
olaf       161  0.0  0.3 624636 48460 pts/1    Sl+  18:03   0:00 /home/olaf/
olaf       717  0.0  0.3 589896 48396 pts/2    Ssl+ 18:50   0:01 /home/olaf/
olaf       726  0.0  0.3 590572 47880 pts/3    Ssl+ 18:50   0:05 /home/olaf/
olaf       733  0.0  0.3 966576 46336 pts/1    Sl+  18:50   0:00 /home/olaf/
olaf       746  0.5  1.2 1015256 154724 pts/1  Sl+  18:50   0:37 /home/olaf/
olaf       800  0.0  0.4 591860 55132 pts/1    Sl+  18:50   0:02 /home/olaf/
olaf       827  0.1  0.3 1539716 45668 pts/1   Sl+  18:59   0:08 /home/olaf/
olaf       864  0.0  0.1 5096912 19016 pts/1   Sl+  18:59   0:00 /home/olaf/
olaf      1292  0.0  0.2 5096912 26904 pts/1   Sl+  20:00   0:01 /home/olaf/
olaf      1431  0.0  0.0  10168  5104 pts/5    Ss   20:37   0:00 -bash
olaf      1476  0.0  0.0   2492   576 pts/0    S+   20:43   0:00 ./a.out
olaf      1477  0.0  0.0  10616  3380 pts/5    R+   20:43   0:00 ps -u
olaf@LAPTOP-K5EHOLM7:~$ kill -12 1476
olaf@LAPTOP-K5EHOLM7:~$ kill -SIGUSR1 1476
olaf@LAPTOP-K5EHOLM7:~$ kill -SIGTERM 1476
olaf@LAPTOP-K5EHOLM7:~$ ps -u
USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
olaf         9  0.0  0.0  10788  5836 pts/0    Ss   17:53   0:00 -bash
olaf       131  0.0  0.0   2612   596 pts/1    Ss+  18:03   0:00 sh -c $VSC
olaf       132  0.0  0.0   2612   532 pts/1    S+   18:03   0:00 sh /mnt/c/U
olaf       137  0.0  0.0   2612   592 pts/1    S+   18:03   0:00 sh /home/ol
olaf       141  0.1  0.6 943592 87232 pts/1    Sl+  18:03   0:13 /home/olaf/
olaf       161  0.0  0.3 624636 48460 pts/1    Sl+  18:03   0:00 /home/olaf/
olaf       717  0.0  0.3 589896 48396 pts/2    Ssl+ 18:50   0:01 /home/olaf/
olaf       726  0.0  0.3 590572 47880 pts/3    Ssl+ 18:50   0:05 /home/olaf/
olaf       733  0.0  0.3 966576 46592 pts/1    Sl+  18:50   0:00 /home/olaf/
olaf       746  0.5  1.2 1015256 154400 pts/1  Sl+  18:50   0:38 /home/olaf/
olaf       800  0.0  0.4 591860 55132 pts/1    Sl+  18:50   0:02 /home/olaf/
olaf       827  0.1  0.3 1539716 45668 pts/1   Sl+  18:59   0:08 /home/olaf/
olaf       864  0.0  0.1 5096912 19016 pts/1   Sl+  18:59   0:00 /home/olaf/
olaf      1292  0.0  0.2 5096912 26904 pts/1   Sl+  20:00   0:01 /home/olaf/
olaf      1431  0.0  0.0  10168  5104 pts/5    Ss   20:37   0:00 -bash
olaf      1478  0.1  0.0   2492   512 pts/0    S+   20:45   0:00 ./a.out
olaf      1479  0.0  0.0  10616  3284 pts/5    R+   20:45   0:00 ps -u
olaf@LAPTOP-K5EHOLM7:~$ kill -SIGALRM 1478
```

Poniżej widoczny jest terminal, w którym wykonywał sie program.

```console
olaf@LAPTOP-K5EHOLM7:~/scr-laboratorium/lista_4$ ./a.out
Blocking SIGUSR2.
Signal SIGUSR2 unblocked.
Blocking SIGUSR2.
Signal SIGUSR2 received.
Signal SIGUSR2 unblocked.
Blocking SIGUSR2.
Signal SIGUSR2 unblocked.
Blocking SIGUSR2.
Program will continue.
Signal SIGUSR2 unblocked.
Blocking SIGUSR2.
Signal SIGUSR2 unblocked.
Blocking SIGUSR2.
Signal SIGUSR2 unblocked.
Blocking SIGUSR2.
Program terminated.
olaf@LAPTOP-K5EHOLM7:~/scr-laboratorium/lista_4$ ./a.out
Blocking SIGUSR2.
Signal SIGUSR2 unblocked.
Blocking SIGUSR2.
Signal SIGUSR2 unblocked.
Blocking SIGUSR2.
Signal SIGUSR2 unblocked.
Blocking SIGUSR2.
Signal SIGUSR2 unblocked.
Blocking SIGUSR2.
Signal SIGUSR2 unblocked.
Blocking SIGUSR2.
Signal SIGUSR2 unblocked.
Blocking SIGUSR2.
^C
olaf@LAPTOP-K5EHOLM7:~/scr-laboratorium/lista_4$
```

##Zadanie 3