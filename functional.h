#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include <stdbool.h>

#define BIG_ENOUGH (1 << 30)
/*valoare prin intermediul careia aloc dinamic o dimensiune*/
#define VAL_ORIENTATIVA 20

typedef char boolean;

typedef struct {
	/*Pointer to a contiguous block of memory where the elements of the array
	are stored*/
	void *data;

	/*Size in bytes of a single element of the array */
	int elem_size;

	/*Number of elements in the array */
	int len;

	/*Pointer to a function that frees the memory of a
	single element of the array, if NULL, no action is taken*/
	void (*destructor)(void *elem);

} array_t;

/**
 * @brief Applies the function func to each element of the array,
 * It does not modify the original list
 *
 * @param func A function that takes a pointer to an element of the
 * array and does something with it
 * @param list The array to be iterated
 */
void for_each(void (*func)(void *), array_t list);

/**
 * @brief Creates a new list with the results of applying a given
 * function to each element of the list, it frees the data in the original list.
 *
 * @param func   A function that takes two pointers, the first void *
 * will be the element of the new array,
 * the second void will be the element of the array.
 * @param new_list_elem_size  The size of a single element of the new array.
 * @param new_list_destructor A function that frees the memory of a single
 * element of the new array, if NULL, no action is taken
 * @param list   The array to be iterated.
 * @return       A new array with the results of applying the function to
 * each element of the original array.
 */
array_t map(void (*func)(void *, void *),
			int new_list_elem_size,
			void (*new_list_destructor)(void *),
			array_t list);

/**
 * @brief Creates a new list containing only the elements of the original
 * array that satisfy a given function
 * It frees the data in the original list.
 *
 * @param func  A function that takes a pointer to an element of the array
 * and returns a boolean.
 * @param list  The array to be iterated.
 * @return      A new list with the elements of the original array that
 * satisfy the given function.
 */
array_t filter(boolean(*func)(void *elem), array_t list);

/**
 * @brief Applies a given function to each element of the array and acculator,
 * starting with the first element.
 * It does not modify the original array.
 *
 * @param func   A function that takes two pointers, the first pointer
 * will be the acumulator, the second pointer will be an element of the array
 * @param acc    The initial value of the accumulator.
 * @param list   The array to be iterated.
 * @return       Last value of the accumulator.
 */
void *reduce(void (*func)(void *, void *), void *acc, array_t list);

/**
 * @brief Applies a given function over multiple arrays.
 * It iterates over the arrays in parallel, applying the function to
 * the elements at the same index.
 * It iterates until the shortest array is exhausted.
 * It does not modify the original arrays.
 *
 * @param func   A function pointer representing the function to be applied.
 * It takes an array of pointers to the elements of the arrays, of size varg_c.
 * @param varg_c Number of arrays to be iterated.
 * @param ...    The arrays to be iterated.
 */
void for_each_multiple(void (*func)(void **), int varg_c, ...);

/**
 * @brief Applies a given function over multiple arrays, creating a new
 * array with the results.
 * It iterates over the arrays in parallel, applying the function to
 * the elements at the same index.
 * It iterates until the shortest array is exhausted.
 * It frees the data in the original arrays.
 *
 * @param func   A function pointer representing the function to be applied.
 * First void * will be the element of the new array, the rest will be the
 * elements of the arrays, of size varg_c.
 * @param new_list_elem_size  The size of a single element of the new array.
 * @param new_list_destructor A function that frees the memory of a single
 * element of the new array, if NULL, no action is taken
 * @param varg_c Number of arrays to be iterated.
 * @param ...    The arrays to be iterated.
 */
array_t map_multiple(void (*func)(void *, void **),
					 int new_list_elem_size,
					 void (*new_list_destructor)(void *),
					 int varg_c, ...);

/**
 * @brief Reduces multiple arrays into a single value.
 * It iterates over the arrays in parallel, applying the function to the
 * elements at the same index.
 * It iterates until the shortest array is exhausted.
 * It does not modify the original arrays.
 *
 * @param func A function pointer representing the function to be applied.
 * First argument is the accumulator, the second is a array of pointers to the
 * elements of the arrays, of size varg_c.
 * @param acc The initial value of the accumulator.
 * @param varg_c Number of arrays to be iterated.
 * @param ... The arrays to be iterated.
 * @return Last value of the accumulator.
 */
void *reduce_multiple(void (*func)(void *, void **),
					  void *acc, int varg_c, ...);

#endif // FUNCTIONAL_H
