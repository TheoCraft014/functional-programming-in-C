#include "functional.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void for_each(void (*func)(void *), array_t list)
{
	if (!list.data) {
		printf("lista nula\n");
		return;
	}

	/*prin tipul de variabila char iterez cate un byte deoarece
	dimensiunea unui char este de un byte*/
	unsigned char *iterator = (unsigned char *)list.data;
	int i;

	for (i = 0; i < list.len; i++)
		func(iterator + i * list.elem_size);
}

array_t map(void (*func)(void *, void *),
			int new_list_elem_size,
			void (*new_list_destructor)(void *),
			array_t list)
{
	if (!list.data) {
		printf("lista nula");
		return list;
	}

	array_t newlist;
	/*initializez noua lista*/
	newlist.len = list.len;
	newlist.elem_size = new_list_elem_size;
	newlist.destructor = new_list_destructor;
	newlist.data = malloc(newlist.len * newlist.elem_size);

	if (!list.data) {
		printf("probleme alocare newlist\n");
		return list;
	}
	/*parcurg fiecare element al celor doua liste, modific datele
	din lista noua in functie de datele din lista veche prin intermediul
	functiei date ca parametru*/
	unsigned char *i1 = (unsigned char *)list.data;
	unsigned char *i2 = (unsigned char *)newlist.data;
	int i;
	for (i = 0; i < list.len; i++)
		func(i2 + i * newlist.elem_size, i1 + i * list.elem_size);

	/*eliberez lista veche din memorie*/
	if (list.destructor)
		for_each(list.destructor, list);

	free(list.data);

	return newlist;
}

array_t filter(boolean(*func)(void *), array_t list)
{
	if (!list.data)
		return list;

	/*retine indexul noii liste*/
	unsigned int index = 0;

	array_t newlist;
	/*initializez lista*/
	newlist.len = 0;
	newlist.elem_size = list.elem_size;
	newlist.destructor = list.destructor;
	newlist.data = malloc(VAL_ORIENTATIVA * newlist.elem_size);

	/*elementele listei au aceeasi dimensiune*/
	size_t size = list.elem_size;

	/*parcurg fiecare element al celor doua liste, modific datele
	din lista noua in functie de datele din lista veche prin intermediul
	functiei date ca parametru*/
	unsigned char *iterator = (unsigned char *)list.data;
	unsigned char *iterator2 = (unsigned char *)newlist.data;
	int i;

	for (i = 0; i < list.len; i++) {
		if (func(iterator + i * list.elem_size) == 1) {
			/*copiez in noua lista memoria din prima lista*/
			memcpy(iterator2 + index * size, iterator + i * size, size);
			/*incrementez indexul*/
			index++;

			/*realoc memorie daca e cazul*/
			if (index >= VAL_ORIENTATIVA) {
				int val = index + VAL_ORIENTATIVA;
				newlist.data = realloc(newlist.data, val * newlist.elem_size);
			}
		}
	}

	/*eliberez lista veche din memorie*/
	for (i = 0; i < list.len; i++)
		if (func(iterator + i * list.elem_size) == 0)
			if (list.destructor)
				list.destructor(iterator + i * list.elem_size);

	free(list.data);

	/*ivaloarea finala a indexului va fi lungimea listei*/
	newlist.len = index;
	return newlist;
}

void *reduce(void (*func)(void *, void *), void *acc, array_t list)
{
	if (!list.data)
		return NULL;

	unsigned char *iterator = (unsigned char *)list.data;

	int i;
	for (i = 0; i < list.len; i++) {
		func(acc, iterator);
		iterator += list.elem_size;
	}

	return acc;
}

void for_each_multiple(void(*func)(void **), int varg_c, ...)
{
	int count;

	// lista de parametri
	va_list args;
	va_start(args, varg_c);

	/*aloc memorie pt un vector de liste ce va memora toate
	listele date ca parametru*/
	array_t *liste = (array_t *)malloc(varg_c * sizeof(array_t));

	for (count = 0; count < varg_c; count++) {
		array_t lista = va_arg(args, array_t);

		liste[count].len = lista.len;
		liste[count].destructor = lista.destructor;
		liste[count].elem_size = lista.elem_size;
		liste[count].data = malloc(lista.len * lista.elem_size);
		memcpy(liste[count].data, lista.data, lista.len * lista.elem_size);
	}

	/*aflam lungimea listei cu dimensiunea cea mai mica*/
	int min_index = BIG_ENOUGH;
	for (count = 0; count < varg_c; count++)
		if (min_index > liste[count].len)
			min_index = liste[count].len;

	int index;
	/*vector de tip void ce retine toate datele listelor de
	pe pozitia count*/
	void **elemente = (void **)malloc(varg_c * sizeof(void *));

	for (count = 0; count < min_index; count++) {
		for (index = 0; index < varg_c; index++) {
			unsigned char *iterator = (unsigned char *)liste[index].data;

			elemente[index] = malloc(liste[index].elem_size);
			size_t size = liste[index].elem_size;
			memcpy(elemente[index], iterator + count * size, size);
		}
		/*aplic functia asupra vectorului de date*/
		func(elemente);

		for (index = 0; index < varg_c; index++)
			free(elemente[index]);
	}

	/*eliberez din memorie*/
	free(elemente);

	for (index = 0; index < varg_c; index++) {
		if (liste[index].destructor)
			for_each(liste[index].destructor, liste[index]);
		free(liste[index].data);
	}
	free(liste);

	va_end(args);
}

array_t map_multiple(void (*func)(void *, void **),
					 int new_list_elem_size,
					 void (*new_list_destructor)(void *),
					 int varg_c, ...)
{
	int count;

	// lista de parametri
	va_list args;
	va_start(args, varg_c);

	/*aloc memorie pt un vector de liste ce va memora toate
	listele date ca parametru*/
	array_t *liste = (array_t *)malloc(varg_c * sizeof(array_t));

	for (count = 0; count < varg_c; count++) {
		array_t lista = va_arg(args, array_t);

		liste[count].len = lista.len;
		liste[count].destructor = lista.destructor;
		liste[count].elem_size = lista.elem_size;
		liste[count].data = malloc(lista.len * lista.elem_size);
		memcpy(liste[count].data, lista.data, lista.len * lista.elem_size);
		/*eliberez din memorie lista data ca parametru*/
		free(lista.data);
	}

	/*aflam lungimea listei cu dimensiunea cea mai mica*/
	int min_index = BIG_ENOUGH;
	for (count = 0; count < varg_c; count++)
		if (min_index > liste[count].len)
			min_index = liste[count].len;

	array_t newlist;
	/*initializez noua lista*/
	newlist.destructor = new_list_destructor;
	newlist.len = min_index;
	newlist.elem_size = new_list_elem_size;
	newlist.data = malloc(newlist.len * newlist.elem_size);

	int index;
	/*vector de tip void ce retine toate datele listelor de
	pe pozitia count*/
	void **elemente = (void **)malloc(varg_c * sizeof(void *));
	unsigned char *iterator2 = (unsigned char *)newlist.data;

	for (count = 0; count < newlist.len; count++) {
		for (index = 0; index < varg_c; index++) {
			unsigned char *iterator = (unsigned char *)liste[index].data;

			elemente[index] = malloc(liste[index].elem_size);
			size_t size = liste[index].elem_size;
			memcpy(elemente[index], iterator + count * size, size);
		}
		/*aplic functia asupra vectorului de date*/
		func(iterator2 + count * newlist.elem_size, elemente);

		for (index = 0; index < varg_c; index++)
			free(elemente[index]);
	}

	va_end(args);

	/*eliberez din memorie*/
	free(elemente);
	for (index = 0; index < varg_c; index++) {
		if (liste[index].destructor)
			for_each(liste[index].destructor, liste[index]);

	free(liste[index].data);
	}

	free(liste);

	return newlist;
}

void *reduce_multiple(void(*func)(void *, void **), void *acc, int varg_c, ...)
{
	int count;

	// lista de parametri
	va_list args;
	va_start(args, varg_c);

	/*aloc memorie pt un vector de liste ce va memora toate
	listele date ca parametru*/
	array_t *liste = (array_t *)malloc(varg_c * sizeof(array_t));

	for (count = 0; count < varg_c; count++) {
		array_t lista = va_arg(args, array_t);

		liste[count].len = lista.len;
		liste[count].destructor = lista.destructor;
		liste[count].elem_size = lista.elem_size;
		liste[count].data = malloc(lista.len * lista.elem_size);
		memcpy(liste[count].data, lista.data, lista.len * lista.elem_size);
	}

	/*aflam lungimea listei cu dimensiunea cea mai mica*/
	int min_index = BIG_ENOUGH;
	for (count = 0; count < varg_c; count++)
		if (min_index > liste[count].len)
			min_index = liste[count].len;

	int index;
	/*vector de tip void ce retine toate datele listelor de
	pe pozitia count*/
	void **elemente = (void **)malloc(varg_c * sizeof(void *));

	for (count = 0; count < min_index; count++) {
		for (index = 0; index < varg_c; index++) {
			unsigned char *iterator = (unsigned char *)liste[index].data;

			elemente[index] = malloc(liste[index].elem_size);
			size_t size = liste[index].elem_size;
			memcpy(elemente[index], iterator + count * size, size);
		}
		/*aplic functia asupra vectorului de date*/
		func(acc, elemente);

		for (index = 0; index < varg_c; index++)
			free(elemente[index]);
	}

	/*eliberez din memorie*/
	free(elemente);
	for (index = 0; index < varg_c; index++) {
		if (liste[index].destructor)
			for_each(liste[index].destructor, liste[index]);
	free(liste[index].data);
	}

	free(liste);

	va_end(args);

	return acc;
}
