#include "log.h"
#include <stdio.h>

static char *convertFloat(char *ptr, float num) {
    sprintf(ptr, "%f", num);
    return ptr;
}

static char *convert(unsigned int num, int base) { 
    static char Representation[]= "0123456789ABCDEF";
    static char buffer[50]; 
    char *ptr; 

    ptr = &buffer[49]; 
    *ptr = '\0'; 
    
    do 
    { 
        *--ptr = Representation[num%base]; 
        num /= base; 
    }while(num != 0); 

    return(ptr); 
}

void LOG(char *format, ...) {
#ifdef DEBUG
    char *traverse; 
    unsigned int i; 
    float f;
    char *s; 

    va_list arg; 
    va_start(arg, format); 

    for(traverse = format; *traverse != '\0'; traverse++) 
    {
        if ( *traverse != '%') 
        { 
	    putchar(*traverse);
        } else {

            traverse++; 
   
            switch(*traverse) 
            { 
                case 'c' : i = va_arg(arg,int);     //Fetch char argument
                            putchar(i);
                            break; 
   
                case 'd' : i = va_arg(arg,int);         //Fetch Decimal/Integer argument
                            if(i<0) 
                            { 
                                i = -i;
                                putchar('-'); 
                            } 
                            puts(convert(i,10));
                            break; 
                case 'u': i = va_arg(arg, int);	    // Fetch Unsigned int argument
   	                 fputs(convert(i, 10), stdout);
   	         	break;
                case 'f': f = va_arg(arg, double);
   	                 char ptr[sizeof(float) * 8];
   	         	convertFloat(ptr, f);
   	                 fputs(ptr, stdout);
   	         	break;
                case 'o': i = va_arg(arg,unsigned int); //Fetch Octal representation
                            fputs(convert(i,8), stdout);
                            break; 
   
                case 's': s = va_arg(arg,char *);       //Fetch string
                            fputs(s, stdout); 
                            break; 
   
                case 'x': i = va_arg(arg,unsigned int); //Fetch Hexadecimal representation
                            fputs(convert(i,16), stdout);
                            break; 
            }
        }
    } 
  
    va_end(arg);
    putchar('\n');
    fflush(stdout);
    return; 
#else 
    return;
#endif 
}
