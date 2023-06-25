#include <iostream>

char* ft_substr(char const* s, unsigned int start, size_t len)
{
	char* a;

	if (start >= strlen(s))
		return (_strdup(""));
	if (strlen(&s[start]) < len)
		len = strlen(&s[start]);
	a = calloc(len + 1, sizeof(char));
	if (!a)
		return (0);
	strncpy(a, &s[start], len + (size_t)1);
	return (a);
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
	ret = calloc(n + 1, sizeof(char*));
	if (!ret)
		return (NULL);
	return (ft_test((char*)s, c, ret, n));
}
