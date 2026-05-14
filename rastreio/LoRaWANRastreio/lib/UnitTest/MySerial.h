#ifndef SERIAL_FAKE_H
#define SERIAL_FAKE_H

#include <Arduino.h>
#include <vector>  // Required for std::vector
#include <sstream> // Required for std::stringstream
class StringSerial : public StdioSerial
{
public:
    std::vector<char> stringbuffer;
    size_t idx = 0;
    u_int32_t delay = 0;

    StringSerial() : StdioSerial(), stringbuffer() {}

    int read()
    {
        int ch = peek();
        if (ch != -1)
        {
            idx++;
        }
        return ch;
    }

    int peek() override
    {
        if(delay > 0) {
            delay--;
            return -1;
        }

        if (idx >= stringbuffer.size())
        {
            return -1;
        }
        else
        {
            char c = stringbuffer[idx];
            if (c == (char)127)
            {
                delay = 50;
                idx++;
                return -1;
            }
            return (int)c;
        }
    }

    void __feed(const char *s)
    {
        for (size_t i = 0; s[i] != '\0'; i++)
        {
            stringbuffer.push_back(s[i]);
        }
    }

    void __clear()
    {
        stringbuffer.clear();
        idx = 0;
        delay = 0;
    }

    void __delay()
    {
        stringbuffer.push_back((char)127);
    }
};
StringSerial Serial1;

#endif
