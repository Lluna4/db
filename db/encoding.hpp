#include <iostream>
#include <random>
#include <time.h>

int INDEX = 0;

static int	ft_intlen(int n)
{
	int	ret;

	ret = 1;
	while (n >= 10)
	{
		ret++;
		n = n / 10;
	}
	return (ret);
}

static char* ft_make_ret(int n, int sign)
{
	int		len;
	char* ret;

	len = ft_intlen(n) + sign;
	ret = (char *)calloc(len + 1, sizeof(char));
	if (!ret)
		return (0);
	len--;
	while (len >= 0)
	{
		ret[len] = (n % 10) + '0';
		n = n / 10;
		len--;
	}
	if (sign == 1)
		ret[0] = '-';
	return (ret);
}

char* ft_itoa(int n)
{
	char* ret;
	int		sign;

	sign = 0;
	if (n == -2147483648)
	{
		ret = (char *)malloc(12 * sizeof(char));
		if (!ret)
			return (0);
		memcpy(ret, "-2147483648", 12);
		return (ret);
	}
	if (n < 0)
	{
		n *= -1;
		sign = 1;
	}
	return (ft_make_ret(n, sign));
}

char* ft_strjoin(char const* s1, char const* s2)

{
	char* ret;
	int		n;

	n = -1;
	if (*s1 == '\0' && *s2 == '\0')
		return (_strdup(""));
	ret = (char *)calloc(strlen(s1) + strlen(s2) + 1, sizeof(char));
	if (!ret)
		return (0);
	while (*s1 != '\0')
	{
		n++;
		ret[n] = *s1;
		s1++;
	}
	while (*s2 != '\0')
	{
		n++;
		ret[n] = *s2;
		s2++;
	}
	return (ret);
}

char* generate_key()
{
	srand((unsigned int)time(NULL));
	int num;
	char* a = (char*)calloc(4, sizeof(char));
	while (strlen(a) < 1024)
	{
		num = rand();
		a = ft_strjoin(ft_itoa(num), a);
		srand(num);
	}
	return (a);
}

char *encode_text(char* text, char *a)
{
	int index = 0;
	char* encoded_text = (char*)calloc(strlen(text) + 1, sizeof(char*));
	while (text[index])
	{
		encoded_text[index] = text[index] + a[index];
		index++;
	}
	return (encoded_text);
}

char* decode_text(char* encoded_text, char* a)
{
	int index = 0;
	char* decoded_text = (char*)calloc(strlen(encoded_text) + 1, sizeof(char*));
	while (encoded_text[index])
	{
		decoded_text[index] = encoded_text[index] - a[index];
		index++;
	}
	return (decoded_text);
}


static int	ft_wordcount(char* a, char ch)
{
	int	n;

	n = 1;
	if (!*a)
		return (0);
	while (*a)
	{
		if (*a == ch)
		{
			n++;
			while (*a == ch)
				a++;
			if (!*a)
				n -= 1;
		}
		if (*a)
			a++;
	}
	return (n);
}

static int	ft_charcount(char* a, char ch)
{
	int	len;

	len = 0;
	while (*a != ch)
	{
		if (*a == '\0')
			break;
		len++;
		a++;
	}
	return (len);
}

char** ft_freeall(char** s)
{
	while (*s)
	{
		free(*s);
		s++;
	}
	free(s);
	return ((char**)(_strdup(" ")));
}

char* ft_substr(char const* s, unsigned int start, size_t len)
{
	char* a;

	if (start >= strlen(s))
		return (_strdup(""));
	if (strlen(&s[start]) < len)
		len = strlen(&s[start]);
	a = (char *)calloc(len + 1, sizeof(char));
	if (!a)
		return (0);
	strncpy_s(a, sizeof(a), &s[start], len + 1);
	return (a);
}

static char** ft_test(char* s, char c, char** ret, int n)
{
	int		pointer;
	char* buff;

	pointer = -1;
	while (*s)
	{
		while (*s == c && *s)
			s++;
		n = ft_charcount(s, c);
		if (n <= 0)
		{
			if (*s == '\0')
				break;
			return (ft_freeall(ret));
		}
		pointer++;
		buff = ft_substr(s, 0, n);
		if (!buff)
			return (ft_freeall(ret));
		ret[pointer] = buff;
		if (*s)
			s = s + n;
	}
	return (ret);
}

char** ft_split(char const* s, char c)
{
	char** ret;
	int		n;

	if (!s)
		return (NULL);
	while (*s == c && *s)
		s++;
	n = ft_wordcount((char*)s, c);
	ret = (char **)calloc(n + 1, sizeof(char*));
	if (!ret)
		return (NULL);
	return (ft_test((char*)s, c, ret, n));
}

static char* ft_test(char* a, size_t len2, size_t s, char* b)
{
	unsigned int n;

	while (len2 > (strlen(b) - 1) && *a != '\0' && (len2 - strlen(a)) < s)
	{
		n = (unsigned int)(strlen(b) - 1);
		a = strchr(a, b[0]);
		if (a == NULL)
			return (0);
		a = a + (strlen(b) - 1);
		if ((len2 - (strlen(a) - 1)) > (unsigned int)s)
			break;
		while (n != 0)
		{
			if (*a != b[n])
			{
				break;
			}
			a--;
			n--;
		}
		if (n == 0)
			return ((char*)a);
	}
	return (0);
}

char* ft_strnstr(const char* a, const char* b, size_t size)
{
	unsigned int	len2;

	len2 = (unsigned int)strlen(a);
	if ((unsigned int)size > strlen(a) || (int)size < 0)
		size = strlen(a);
	if (*b == '\0')
		return ((char*)a);
	return (ft_test((char*)a, (size_t)len2, (size_t)size, (char*)b));
}

