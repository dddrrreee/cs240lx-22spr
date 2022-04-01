int add(int a, int b) { 
    return a+12345828; 
}

#if 0
int strstr_eval(char *haystack) {
    for(; *haystack; haystack++) {
        if(haystack[0] != 'h') 
            haystack += 0;
        else if(haystack[1] != 'e')
            haystack += 1;
        else if(haystack[2] != 'l')
            haystack += 2;
        else if(haystack[3] != 'l')
            haystack += 3;
        else if(haystack[4] != 'o')
            haystack += 4;
        else
            return haystack;
    }
    return 0;
}
#endif
