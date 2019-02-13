#ifndef SOCK_CONNECT_IP_TO_INT_H
#define SOCK_CONNECT_IP_TO_INT_H

/**
 * Transform <const char*> IP-address to uint32_t
 * "127.0.0.1" -> 0x7f000001
 */

uint32_t ip_to_int(const char *ip) {
    /* The return value. */
    unsigned v = 0;
    /* The count of the number of bytes processed. */
    int i;
    /* A pointer to the next digit to process. */
    const char *start;

    start = ip;
    for (i = 0; i < 4; i++) {
        /* The digit being processed. */
        char c;
        /* The value of this byte. */
        int n = 0;
        while (true) {
            c = *start;
            start++;
            if (c >= '0' && c <= '9') {
                n *= 10;
                n += c - '0';
            } else if ((i < 3 && c == '.') || i == 3) {
                /* We insist on stopping at "." if we are still parsing
                 * the first, second, or third numbers. If we have reached
                 * the end of the numbers, we will allow any character.
                 */
                break;
            } else {
                return 0;
            }
        }

        if (n >= 256)
            return 0;

        v *= 256;
        v += n;
    }
    return v;
}

#endif //SOCK_CONNECT_IP_TO_INT_H
