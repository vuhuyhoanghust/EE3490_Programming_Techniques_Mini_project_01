#include "check_string.h"

/* Check a blank string*/
bool isBlank(string str)
{
    int count_blk = 0;
    if (str.length() == 0)
        return 1;
    for (int i = 0; i < str.length(); i++)
    {
        if (isblank(str[i]))
            count_blk++;
    }
    if (count_blk == str.length())
        return 1;
    return 0;
}
/* Check a positive integer string*/
bool isPosIntNumber(string s)
{
    return s.find_first_not_of("0123456789 ") == string::npos;
}

/* Check a positive real string*/
bool isPosRealNumber(string s)
{
    size_t found = 0;
    int count_dot = 0;
    while (found != string::npos)
    {
        found = s.find(".", found + 1);
        count_dot++;
        if (count_dot > 2)
        {
            return 0;
        }
    }

    return s.find_first_not_of(".0123456789 ") == string::npos;
}

/* Check a time string*/
bool isTime(string s)
{
    struct tm tm;
    std::istringstream ss(s);
    ss >> std::get_time(&tm, "%Y:%m:%d %H:%M:%S");
    if (ss.fail())
        return 0;
    return 1;
}