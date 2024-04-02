#include "stdio.h"
#include "Drivers/keyboard.h"

int *printf_number(int *argp, int length, bool sign, int radix);

void putchar(char c)
{
    terminal_putchar(c);
}

void puts(char* str)
{
    terminal_write_string(str);
}

char getchar()
{
    char c = 0;

    // wait for key-press
    while((c = key_pop()) == 0) {}

    return c;
}

/*
  Get an input string from the terminal
*/
char* getline() {
  
  char c = 0;
  size_t size = 0;
  
  char* line = NULL;

  // getchar() until newline charcater is encountered
  while ((c = getchar()) != '\n') 
  {
    if(c == '\b')
    {
      if(size)
      {
        line = realloc(line, --size);
        putchar('\b');
      }
    }
    else
    {
        putchar(c);
        line = realloc(line, ++size);
        line[size - 1] = c;
    }
  }
  
  line = realloc(line, size + 1);
  line[size] = '\0';

  putchar('\n');
  return line;
}


void printf(const char *fmt, ...)
{
    int *argp = (int *)&fmt;
    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;
    int radix = 10;
    bool sign = false;

    argp++;

    while (*fmt)
    {
        switch (state)
        {
        case PRINTF_STATE_NORMAL:
            switch (*fmt)
            {
            case '%':
                state = PRINTF_STATE_LENGTH;
                break;
            default:
                putchar(*fmt);
                break;
            }
            break;

        case PRINTF_STATE_LENGTH:
            switch (*fmt)
            {
            case 'h':
                length = PRINTF_LENGTH_SHORT;
                state = PRINTF_STATE_LENGTH_SHORT;
                break;
            case 'l':
                length = PRINTF_LENGTH_LONG;
                state = PRINTF_STATE_LENGTH_LONG;
                break;
            default:
                goto PRINTF_STATE_SPEC_;
            }
            break;

        case PRINTF_STATE_LENGTH_SHORT:
            if (*fmt == 'h')
            {
                length = PRINTF_LENGTH_SHORT_SHORT;
                state = PRINTF_STATE_SPEC;
            }
            else
                goto PRINTF_STATE_SPEC_;
            break;

        case PRINTF_STATE_LENGTH_LONG:
            if (*fmt == 'l')
            {
                length = PRINTF_LENGTH_LONG_LONG;
                state = PRINTF_STATE_SPEC;
            }
            else
                goto PRINTF_STATE_SPEC_;
            break;

        case PRINTF_STATE_SPEC:
        PRINTF_STATE_SPEC_:
            switch (*fmt)
            {
            case 'c':
                putchar((char)*argp);
                argp++;
                break;

            case 's':
                  terminal_write_string(*(const char **)argp);
                  argp++;
                break;

            case '%':
                putchar('%');
                break;

            case 'd':
            case 'i':
                radix = 10;
                sign = true;
                argp = printf_number(argp, length, sign, radix);
                break;

            case 'u':
                radix = 10;
                sign = false;
                argp = printf_number(argp, length, sign, radix);
                break;

            case 'X':
            case 'x':
            case 'p':
                radix = 16;
                sign = false;
                argp = printf_number(argp, length, sign, radix);
                break;

            case 'o':
                radix = 8;
                sign = false;
                argp = printf_number(argp, length, sign, radix);
                break;

            // ignore invalid spec
            default:
                break;
            }

            // reset state
            state = PRINTF_STATE_NORMAL;
            length = PRINTF_LENGTH_DEFAULT;
            radix = 10;
            sign = false;
            break;
        }

        fmt++;
    }
}

const char g_HexChars[] = "0123456789abcdef";

int *printf_number(int *argp, int length, bool sign, int radix)
{
    char buffer[32];
    unsigned long long number;
    int number_sign = 1;
    int pos = 0;

    // process length
    switch (length)
    {
    case PRINTF_LENGTH_SHORT_SHORT:
    case PRINTF_LENGTH_SHORT:
    case PRINTF_LENGTH_DEFAULT:
        if (sign)
        {
            int n = *argp;
            if (n < 0)
            {
                n = -n;
                number_sign = -1;
            }
            number = (unsigned long long)n;
        }
        else
        {
            number = *(unsigned int *)argp;
        }
        argp++;
        break;

    case PRINTF_LENGTH_LONG:
        if (sign)
        {
            long int n = *(long int *)argp;
            if (n < 0)
            {
                n = -n;
                number_sign = -1;
            }
            number = (unsigned long long)n;
        }
        else
        {
            number = *(unsigned long int *)argp;
        }
        argp += 2;
        break;

    case PRINTF_LENGTH_LONG_LONG:
        if (sign)
        {
            long long int n = *(long long int *)argp;
            if (n < 0)
            {
                n = -n;
                number_sign = -1;
            }
            number = (unsigned long long)n;
        }
        else
        {
            number = *(unsigned long long int *)argp;
        }
        argp += 4;
        break;
    }

    // convert number to ASCII
    do
    {
        uint32_t rem = number % radix;
        number /= radix;

        buffer[pos++] = g_HexChars[rem];
    } while (number > 0);

    // add sign
    if (sign && number_sign < 0)
        buffer[pos++] = '-';

    // print number in reverse order
    while (--pos >= 0)
        putchar(buffer[pos]);

    return argp;
}
