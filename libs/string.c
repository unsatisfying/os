
#include "types.h"
#include "string.h"

inline void *memcpy(void *dst, const void *src,
                    size_t n) // n表示总字节数
{
  if (dst == NULL || src == NULL)
    return NULL;
  long *pdst;
  const long *psrc;
  int flag = (src < dst) && ((char *)src + n > (char *)dst);
  size_t count = n >> 2;
  if (count)
  {
    if (flag) //从后往前拷贝
    {
      pdst = (long *)((char *)dst + n - 4);
      psrc = (const long *)((const char *)src + n - 4);
      while (count--)
      {
        *pdst = *psrc;
        pdst--;
        psrc--;
      }
    }
    else
    {
      pdst = (long *)dst;
      psrc = (const long *)src;
      while (count--)
      {
        *pdst = *psrc;
        pdst++;
        psrc++;
      }
    }
  }
  count = n & 0x3;
  if (count)
  {
    if (flag)
    {
      char *pcdst = (char *)dst + n - 1;
      const char *pcsrc = (const char *)src + n - 1;
      while (count--)
      {
        *pcdst = *pcsrc;
        pcdst--;
        pcsrc--;
      }
    }
    else
    {
      char *pcdst = (char *)dst;
      const char *pcsrc = (const char *)src;
      while (count--)
      {
        *pcdst = *pcsrc;
        pcdst++;
        pcsrc++;
      }
    }
  }
  return dst;
}

inline void *memset(void *dst, uint8_t c, uint32_t n)
{
  uint8_t *pdst = dst;
  while (n--)
  {
    *pdst++ = c;
  }
  return dst;
}

inline void *memsetw(void *dst, uint16_t c, uint32_t n)
{
  uint16_t *pdst = dst;
  while (n--)
  {
    *pdst++ = c;
  }
  return dst;
}

inline void bzero(void *dst, uint32_t n) { memset(dst, 0, n); }

inline int strcmp(const char *str1, const char *str2)
{
  while (*str1 && *str2 && *str1 == *str2)
  {
    ++str1;
    ++str2;
  }
  return *str1 - *str2;
}

inline char *strcpy(char *dst, const char *src)
{
  if ((dst == NULL) || (src == NULL))
    return NULL;

  char *ret = dst;
  while ((*dst++ = *src++) != '\0')
    ;
  return ret;
}

char *strcat(char *dst, const char *src)
{
  if ((dst == NULL) || (src == NULL))
    return NULL;
  char *cp = dst;

  while (*cp != '\0')
    ++cp;

  while ((*cp++ = *src++) != '\0')
  {
    //
  }

  return dst;
}

inline uint32_t strlen(const char *str)
{
  uint32_t ans = 0;
  while (*str++ != '\0')
    ++ans;
  return ans;
}