
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define BUFSIZE (sizeof(unsigned long) * 8 + 1)

char * my_itoa(int num, char *str, int base);
int my_strlen(const char* a);
int show_s (char const*format, int dlinna);
int formatting (const char* format, int *ind, va_list *args);
int my_printf (char * restrict format, ...);
char* my_ltoa (unsigned long num, char* str, unsigned long base);
char * pre (void);


int main () { 

    int a =3;


    printf("My name is %s, I am %d years old. My nana is %x and my mom is %o years old FINALLY: %p ", "Dayana", 10, 0xfa, 32, &a);
    my_printf("My name is %s, I am %d years old. My nana is %x and my mom is %o years old FINALLY: %p \n", "Dayana", 10, 0xfa, 32, &a);

    return 0;
}

char* my_itoa (int num, char* str, int base) {
    int ind = 0;
    int cp = num;

    while (cp != 0) {
        cp /= base; 
        ind++;
    }

    *(str + ind) = '\0';
    ind--;
    
    if (base >= 16) {
        while (num != 0) {
            if (num % base > 9) {
                *(str + ind)=(num % base) + 87;            
            } else {
                *(str + ind) = (num % base) + '0';
            }
            num /= base;
            ind--;
        }

        return str;
    }

    if (base < 16) {

        while (num != 0) {

            *(str + ind) = (num % base) + '0';
            num /= base;
            ind--;
    }
        return str;
    }

    return str;
}

int my_strlen(const char* a) 
{
 
    int count = 0;

    while (*a != 0) { 
     count++; 
     a++;
    }
     return count;
}

int show_s (char const*format, int dlinna) {
    return write(1, format, dlinna);
}

int formatting (const char* format, int *ind, va_list *args) {
    int result =0;
    (*ind) ++;
    char *tmp = (char*)malloc(10*sizeof(char));
    char *tmp1 = (char*)malloc(28*sizeof(char));

    if (format[*ind]==0) {
        return 0;
    }

    switch (format[*ind]) {

        case 's': {
            char* each = va_arg(*args, char *);
            result+=write(1, each, my_strlen(each));
            break; }
        case 'c': {
            char * each = va_arg(*args, char *);
            result+=write(1, each, 1);
            break; }           
        case 'd': {
            int each_i = va_arg(*args, int);
            result+=write(1, my_itoa(each_i, tmp, 10), my_strlen(my_itoa(each_i, tmp, 10)));
            break; }
        case 'x': {
            int each_i = va_arg(*args, int);
            result+=write(1, my_itoa(each_i, tmp, 16), my_strlen(my_itoa(each_i, tmp, 16)));
            break; }
        case 'o': {
            int each_i = va_arg(*args, int);
            result+=write(1, my_itoa(each_i, tmp, 8), my_strlen(my_itoa(each_i, tmp, 8)));
            break;
        }
        case 'u': {
            unsigned int each_i = va_arg(*args, unsigned int);
            result+=write(1, my_itoa(each_i, tmp, 10), my_strlen(my_itoa(each_i, tmp, 10)));
            break;
        }        
        case 'p': {
            unsigned long point = va_arg(*args,  unsigned long);
            result+=write(1, pre(), 3);
            result+=write(1, my_ltoa(point, tmp1, 16), 13); 
            break; 
        }
        default:
            break;
    }

    free(tmp);
    free(tmp1);
    return result;

}


int my_printf (char * restrict format, ...) {
    va_list(args);
    int ind = 0;
    int last_p = 0;
    int result =0;

    va_start(args, format);

    while (format[ind]!='\0') {
    
        if(format[ind]=='%') {
            result +=show_s(format+last_p, ind-last_p);
            result += formatting(format, &ind, &args);
            last_p=ind+1;
        }
    
        ind++;


    }
    result += show_s(format+last_p, ind-last_p);
    va_end(args);
    return result;
}

char* my_ltoa (unsigned long num, char* str, unsigned long base) {
    int i = 2;
    long cp;
    char *tail, *head = str, buf[BUFSIZE];

    tail = &buf[BUFSIZE - 1];
    *tail-- = '\0';
    cp = num;

    if (cp) {
        for (i = 1; cp; ++i){
        
        ldiv_t r;

            r = ldiv(cp, base);
            *tail-- = (char)(r.rem + ((9L < r.rem)?
            ('a' - 10L) : '0'));
            cp = r.quot;
        }
    }    
    else  *tail-- = '0';

    memcpy(head, ++tail, i);
    return str;
}

char* pre(void) {

    char *str = "0x";

    return str;

}