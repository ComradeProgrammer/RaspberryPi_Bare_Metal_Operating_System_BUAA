#include <os_printf.h>
extern int get_el();
void printel(){
    printf("Current exception level switched to: %d \r\n",get_el());
}