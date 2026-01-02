#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define SLEEP_MS(ms) Sleep(ms)
#else
#include <unistd.h>
#define SLEEP_MS(ms) usleep((ms)*1000)
#endif

int main(void) {
    char buf[9];
    
    for (;;) {
        time_t t = time(NULL);
        struct tm *lt = localtime(&t);
        
        if (lt == NULL || strftime(buf, sizeof(buf), "%H:%M:%S", lt) == 0) {
            return 1;
        }
        
        printf("\rTime: %s   ", buf);
        fflush(stdout);
        SLEEP_MS(1000);
    }
}
