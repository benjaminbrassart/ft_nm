/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/29 14:53:39 by bbrassar          #+#    #+#             */
/*   Updated: 2024/08/29 15:53:16 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/** this is a working function for symbol name comparison, GNU nm style. */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int _cmp_name(const void *b1, const void *b2)
{
	char const *s1 = *(char const **)b1;
	char const *s2 = *(char const **)b2;

	size_t i1;
	size_t i2;

	i1 = 0;
	i2 = 0;

	while (1) {
		while (s1[i1] != '\0' && !isalnum(s1[i1])) {
			i1++;
		}

		while (s2[i2] != '\0' && !isalnum(s2[i2])) {
			i2++;
		}

		if (toupper(s1[i1]) != toupper(s2[i2])) {
			return toupper(s1[i1]) - toupper(s2[i2]);
		}

		if (s1[i1] == '\0' && s2[i2] == '\0') {
			break;
		}

		i1++;
		i2++;
	}

	i1 = 0;
	i2 = 0;

	while (1) {
		while (s1[i1] != '\0' && !isalnum(s1[i1])) {
			i1++;
		}

		while (s2[i2] != '\0' && !isalnum(s2[i2])) {
			i2++;
		}

		if (s1[i1] != s2[i2]) {
			return isupper(s1[i1]) - isupper(s2[i2]);
		}

		if (s1[i1] == '\0' && s2[i2] == '\0') {
			break;
		}

		i1++;
		i2++;
	}

	return strcasecmp(s1, s2);
}

int main(void)
{
	char const *strings[] = {
		"B",
		"_b",
		"_B",
		"__b",
		"b",
		"__A",
		"_a",
		"__B",
		"_A",
		"a",
		"__a",
		"A",
	};

	size_t membsz = sizeof(strings[0]);
	size_t nmemb = sizeof(strings) / membsz;

	qsort(strings, nmemb, membsz, _cmp_name);

	for (size_t i = 0; i < nmemb; i += 1) {
		printf("%s\n", strings[i]);
	}
}
