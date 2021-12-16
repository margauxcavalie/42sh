void reverse(char *s)
{
    if (s)
    {
        int len = 0;
        for (int i = 0; s[i] != '\0'; i++)
        {
            len++;
        }
        int start_index = 0;
        int end_index = len - 1;
        while (start_index <= end_index)
        {
            int temp = s[start_index];
            s[start_index] = s[end_index];
            s[end_index] = temp;
            start_index++;
            end_index--;
        }
    }
}

char *my_itoa(int value, char *s)
{
    if (value == 0)
    {
        *s = '0';
        return s;
    }
    int is_negative = 0;
    int number = value;
    if (value < 0)
    {
        is_negative = 1;
        number = -value;
    }
    int index = 0;
    while (number != 0)
    {
        s[index] = (number % 10) + '0';
        index++;
        number /= 10;
    }
    if (is_negative)
    {
        s[index] = '-';
    }
    reverse(s);
    return s;
}
