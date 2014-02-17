
/*
 * While we find nice hex chars, build an int.
 * Return number of chars processed.
 */
 
#define GDB_BYTE_ALIGN(x) ((((x) + sizeof(long)-1)/sizeof(long)) * sizeof(long))
#define GDB_ADDR_ALIGN(addr) ((addr)& 0xfffffffc)
 
const char hexchars[256]="0123456789abcdef";
 
static inline int hex(unsigned char ch)
{
	if (ch >= 'a' && ch <= 'f')
		return ch-'a'+10;
	if (ch >= '0' && ch <= '9')
		return ch-'0';
	if (ch >= 'A' && ch <= 'F')
		return ch-'A'+10;
	return -1;
}

static inline int gethexnum(unsigned char **ptr)
{
    int numChars = 0;
    int hexValue;
    while (**ptr)
    {
        hexValue = hex(**ptr);
        if (hexValue < 0)
        {
            break;
        }
        numChars++;
        (*ptr)++;
    }
    return (numChars);
}

static inline int hex2intbycnt(const char *ptr, int *intValue, unsigned char numChars)
{
	int count = 0;
	int hexValue;

	*intValue = 0;
	

	while (*ptr && numChars) {
		hexValue = hex(*ptr);
		if (hexValue < 0)
			break;

		*intValue = (*intValue << 4) | hexValue;
		count ++;
		numChars--;

		ptr++;
	}

	return count;
}

static inline int hex2long(char **ptr, long *longValue)
{
	int numChars = 0;
	int hexValue;

	*longValue = 0;

	while (**ptr) {
		hexValue = hex(**ptr);
		if (hexValue < 0)
			break;

		*longValue = (*longValue << 4) | hexValue;
		numChars ++;

		(*ptr)++;
	}

	return numChars;
}

static inline int hex2longlong(unsigned char **ptr, long long *longValue)
{
    int numChars = 0;
    int hexValue;

    *longValue = 0;

    while (**ptr)
    {
        hexValue = hex(**ptr);
        if (hexValue < 0)
            break;

        *longValue = (*longValue << 4) | hexValue;
        numChars++;

        (*ptr)++;
    }

    return numChars;
}

/*
 * While we find nice hex chars, build an int.
 * Return number of chars processed.
 */
static inline int hex2int(char **ptr, int *intValue)
{
	int numChars = 0;
	int hexValue;

	*intValue = 0;

	while (**ptr) {
		hexValue = hex(**ptr);
		if (hexValue < 0)
			break;

		*intValue = (*intValue << 4) | hexValue;
		numChars ++;

		(*ptr)++;
	}

	return (numChars);
}

static inline int hex2byte(u8 **ptr, u8 * chValue)
{
    int numChars = 0;
    int hexValue;

    *chValue = 0;

    while (**ptr && (numChars < 2))
    {
        hexValue = hex(**ptr);
        if (hexValue < 0)
            break;

        *chValue = (*chValue << 4) | (char) hexValue;
        numChars++;

        (*ptr)++;
    }

    return (numChars);
}

static inline unsigned char* int2hex(unsigned int value, unsigned char *buf)
{
    *buf++ = hexchars[(value >> 28) & 0xf];
    *buf++ = hexchars[(value >> 24) & 0xf];
    *buf++ = hexchars[(value >> 20) & 0xf];
    *buf++ = hexchars[(value >> 16) & 0xf];
    *buf++ = hexchars[(value >> 12) & 0xf];
    *buf++ = hexchars[(value >> 8) & 0xf];
    *buf++ = hexchars[(value >> 4) & 0xf];
    *buf++ = hexchars[value & 0xf];
    *buf = 0;
    return (buf);
}

static inline unsigned char* long2hex(long value, unsigned char *buf)
{
    *buf++ = hexchars[(value >> 28) & 0xf];
    *buf++ = hexchars[(value >> 24) & 0xf];
    *buf++ = hexchars[(value >> 20) & 0xf];
    *buf++ = hexchars[(value >> 16) & 0xf];
    *buf++ = hexchars[(value >> 12) & 0xf];
    *buf++ = hexchars[(value >> 8) & 0xf];
    *buf++ = hexchars[(value >> 4) & 0xf];
    *buf++ = hexchars[value & 0xf];
    *buf = 0;
    return (buf);
}

static inline u8 * mem2hex(u8 *mem, u8 *buf, int count, int may_fault)
{
    int loop1, loop2, offset;
    int align_cnt = GDB_BYTE_ALIGN(count);
    unsigned long align_addr = GDB_ADDR_ALIGN((unsigned long)mem);
    unsigned long tmp_l = 0;
    u8 * base = buf;

    if ((unsigned long) mem & 0x3)/*unaligned address*/
    {
        offset = ((unsigned long) mem & 0x3);
        loop1 = sizeof(long) - offset;
        tmp_l = *(long*) align_addr;
        align_addr += 4;
        while (loop1-- > 0)
        {
            unsigned char ch = (tmp_l >> (offset * sizeof(char) * 8)) & 0xff;
            offset++;
            *buf++ = hexchars[ch >> 4];
            *buf++ = hexchars[ch & 0xf];
        }
    }

    loop2 = align_cnt / sizeof(long);
    while (loop2-- > 0)
    {
        tmp_l = *(long*) align_addr;
        align_addr += 4;

        *buf++ = hexchars[(tmp_l >> 4) & 0xf];
        *buf++ = hexchars[tmp_l & 0xf];
        *buf++ = hexchars[(tmp_l >> 12) & 0xf];
        *buf++ = hexchars[(tmp_l >> 8) & 0xf];
        *buf++ = hexchars[(tmp_l >> 20) & 0xf];
        *buf++ = hexchars[(tmp_l >> 16) & 0xf];
        *buf++ = hexchars[(tmp_l >> 28) & 0xf];
        *buf++ = hexchars[(tmp_l >> 24) & 0xf];
    }
    
    buf = base + count * 2;
    /*clear the redundant buffer*/
    *buf = 0;
    memset(buf, 0, (align_cnt - count) << 1);
    
    return buf;
}

/*
 * convert the hex array pointed to by buf into binary to be placed in mem
 * return a pointer to the character AFTER the last byte written
 * may_fault is non-zero if we are reading from arbitrary memory, but is currently
 * not used.
 */
static inline u8 __user *hex2mem_user(const u8 *from, u8 __user *to, u32 *tmp_buf, const int count, int may_fault)
{

    int align_cnt = GDB_BYTE_ALIGN(count);
    int loop = align_cnt / sizeof(long);
    u32 *p = tmp_buf;

    while (loop-- > 0)
    {
        unsigned long tmp_l = 0;

        tmp_l |= hex(*from++) << 4;
        tmp_l |= hex(*from++);
        tmp_l |= hex(*from++) << 12;
        tmp_l |= hex(*from++) << 8;
        tmp_l |= hex(*from++) << 20;
        tmp_l |= hex(*from++) << 16;
        tmp_l |= hex(*from++) << 28;
        tmp_l |= hex(*from++) << 24;

        *p++ = tmp_l;
    }
    
    copy_to_user(to, tmp_buf, count);

    return to + count;
} 

static inline u8 *hex2mem(const u8 *from, u8 *to, const int count, int may_fault)
{

    int align_cnt = GDB_BYTE_ALIGN(count);
    int loop = align_cnt / sizeof(long);

    if ((count == align_cnt) && (((long) to & 3) == 0))
    {
        while (loop-- > 0)
        {
            unsigned long tmp_l = 0;

            tmp_l |= hex(*from++) << 4;
            tmp_l |= hex(*from++);
            tmp_l |= hex(*from++) << 12;
            tmp_l |= hex(*from++) << 8;
            tmp_l |= hex(*from++) << 20;
            tmp_l |= hex(*from++) << 16;
            tmp_l |= hex(*from++) << 28;
            tmp_l |= hex(*from++) << 24;

            *(unsigned long *) to = tmp_l;
            to += 4;
        }
    }
    else
    {
        int i;
        for (i = 0; i < count; i++)
        {
            unsigned char ch = hex(*from++) << 4;
            ch |= hex(*from++);
            *to++ = ch;
        }
    }

    return to;
}


/*
 * Copy the binary array pointed to by buf into mem.  Fix $, #, and
 * 0x7d escaped with 0x7d.  Return a pointer to the character after
 * the last byte written.
 */
static inline u8 __user *ebin2mem_user(const u8 *from, u8 __user *to, u32 *tmp_buf, const int count, int may_fault)
{
    int i;
    u8 *p = (u8 *)tmp_buf;
    
    for (i = 0; i < count; i++)
    {
        if (*from == 0x7d)
        {
            *p++ = *(++from) ^ 0x20;
        }
        else
        {
            *p++ = *from;
        }
        from++;
    } 
    
    copy_to_user(to, tmp_buf, count);
    
    return (to + count);
}

static inline u8 *ebin2mem(u8 *from, u8 *to, int count, int may_fault)
{
    int i, unaligned_cnt = 0;
    int tmp_cnt = 0;

    if ((unsigned long) to & 0x3)
    {
        unaligned_cnt = sizeof(unsigned long) - ((unsigned long) to & 0x3);
        tmp_cnt = min(unaligned_cnt, count);

        for (; tmp_cnt > 0; tmp_cnt--, from++)
        {
            if (*from == 0x7d)
                *to++ = *(++from) ^ 0x20;
            else
                *to++ = *from;
        }

        count = count - tmp_cnt;
    }

    unaligned_cnt = count % 4;
    count = count / 4;

    for (; count > 0; count--)
    {
        unsigned long tmp_l = 0;

        for (i = 0; i < 4; i++)
        {
            if (*from == 0x7d)
            {
                tmp_l |= ((*(++from) ^ 0x20) << (i * 8));
            }
            else
            {
                tmp_l |= (*from << (i * 8));
            }
            from++;
        }
        *(unsigned long *) to = tmp_l;
        to += 4;
    }

    for (; unaligned_cnt > 0; unaligned_cnt--, from++)
    {
        if (*from == 0x7d)
            *to++ = *(++from) ^ 0x20;
        else
            *to++ = *from;
    }

    return to;
}

