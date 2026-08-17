#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

// Данный алгоритм сжимает строки схожих символов.
// Например: aabaa -> результат a2ba2
// Если значение буфера будет = nullptr - то результат будет уже не указатель на buffer а длина символов требуемый минимум для buffer.
size_t compressDups(char * buffer, size_t n, const char* input)
{
    int l,i,k,c;
    size_t ret = 0;
    if(buffer != NULL && n < 1 || input == NULL || (l = strlen(input)) == 0)
    {
        return ret;
    }

    i = 0;
    k = 0;
    if(buffer == NULL)
    {
        while(i < l)
        {
            c = 0;
            k = input[i];
            while(i < l && input[i] == k)
            {
                ++c;
                ++i;
            }
            ret += 1u+(c == 1 ? 0 : c < 10 ? 1 : c < 100 ? 2 : c < 1000 ? 3 : c < 10000 ? 4 : 0);
        }
    }
    else
    {
        while(i < l)
        {
            c = 0;
            buffer[ret] = input[i];
            while(i < l && input[i] == buffer[ret])
            {
                ++c;
                ++i;
            }
            ++ret;

            if(c > 1)
            {
                k = snprintf(NULL, 0, "%d", c);
                if(ret+k > n)
                    return 0;
                ret += snprintf((buffer + ret), n-ret+1, "%d", c);
            }
        }
        if(ret<n)
            buffer[ret]='\0';
    }

    return ret;
}

int main(void) 
{
    const char *tests[] = {
        "aabcccccaaa",
        "abcdef",
        "ABBC",
        "aaaaaaaaaaaaaaaaaaaaabaaaaaaaaaa"
    };
    int LL = 1;
    char * buffer = (char*)malloc(LL);

    for(int i = 0; i < 4; ++i)
    {
        memset(buffer,0,LL);
        int nReq = compressDups(NULL, 0, tests[i]);
        if(nReq > 0 && nReq != LL)
        {
            LL = nReq;
            buffer = reinterpret_cast<char*>(realloc(buffer, LL));
        }
        if(compressDups(buffer, LL, tests[i]) == 0)
        {
            printf("Out of memory!\n");
            break;
        }
        else
        {
            printf("original: %s\nresult: %s\n", tests[i], buffer);
        }
    }
    free(buffer);
    return EXIT_SUCCESS;
}
