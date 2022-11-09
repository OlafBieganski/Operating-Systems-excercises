#Sprawozdanie lista 5

##Zadanie 1

Polecenie strace pozwala na analize kodu programu oraz pokazuje wywołania systemowe i sygnały w procesie. Program można uruchomić pod nadzorem strace lub podłączyć się do działającego już procesu. Poniżej przykładowe opcje wywołania strace:

```bash
strace -p $PID # podłącz się do działającego procesu
strace $name -e trace=$syscall # śledź tylko wybrane wywołania systemowe
strace $name -e signal=set # śledź tylko wybrane sygnały
```

##Zadanie 2

```c
#include <stdio.h>

int main(){
    printf("Hello world!");
    return 0;
}
```
Powyższy program uruchomiono pod nadzorem programu strace. W celu wyświetlenia napisu została wywołana funkcja systemowa write(). Jest to jedna z podstawowych funkcji jądra systemów Unix. Przepisuje ona dane z bufora zadeklarowanego przez użytkownika do wybranego pliku, czyli np. jak w tym przypadku stdout.

```console
write(1, "Hello world!", 12Hello world!)            = 12
```

W bibliotece systemowej funkcja zadeklarowana jest w następujący sposób:

```c
ssize_t write(int fd, const void *buf, size_t nbytes);
```
Gdzie fd to deskryptor pliku do, którego mają być wpisane dane (1 odpowiada stdout), *buf to wskaxnik do tablicy znaków, czyli naszego napisu "Hello world!", a nbytes to ilośc bajtów pamięci do zapisania. Funkcja zwraca ilośc bajtów poprawnie zapisanych do pliku. W tym przypadku jest to 12 co odpowiada liczbie znaków w naszym napisie, a więc funkcja zadziałała poprawnie.

##Zadanie 3

```console
olaf@LAPTOP-K5EHOLM7:~/scr-laboratorium/lista_5$ strace -e trace=desc ./a.out
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=60346, ...}) = 0
mmap(NULL, 60346, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7f83ac2b8000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\360A\2\0\0\0\0\0"..., 832) = 832
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
pread64(3, "\4\0\0\0\20\0\0\0\5\0\0\0GNU\0\2\0\0\300\4\0\0\0\3\0\0\0\0\0\0\0", 32, 848) = 32
pread64(3, "\4\0\0\0\24\0\0\0\3\0\0\0GNU\0\237\333t\347\262\27\320l\223\27*\202C\370T\177"..., 68, 880) = 68
fstat(3, {st_mode=S_IFREG|0755, st_size=2029560, ...}) = 0
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f83ac2b6000
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
pread64(3, "\4\0\0\0\20\0\0\0\5\0\0\0GNU\0\2\0\0\300\4\0\0\0\3\0\0\0\0\0\0\0", 32, 848) = 32
pread64(3, "\4\0\0\0\24\0\0\0\3\0\0\0GNU\0\237\333t\347\262\27\320l\223\27*\202C\370T\177"..., 68, 880) = 68
mmap(NULL, 2037344, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7f83ac0c4000
mmap(0x7f83ac0e6000, 1540096, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x22000) = 0x7f83ac0e6000
mmap(0x7f83ac25e000, 319488, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x19a000) = 0x7f83ac25e000
mmap(0x7f83ac2ac000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1e7000) = 0x7f83ac2ac000
mmap(0x7f83ac2b2000, 13920, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7f83ac2b2000
close(3)                                = 0
fstat(1, {st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0), ...}) = 0
write(1, "Hello world!", 12Hello world!)            = 12
+++ exited with 0 +++
```

##Zadanie 4