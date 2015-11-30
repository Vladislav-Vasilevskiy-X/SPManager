#ifndef MMEMORY_EXPORTS
#define MMEMORY_API __declspec(dllexport)
#else
#define MMEMORY_API __declspec(dllimport)
#endif

#include<stdlib.h>
#include<stdio.h>
#include<sys/timeb.h>

#define TRUE 1
#define FALSE 0
#define PHYSICAL_MEMORY_SIZE 1<<19
#define VIRTUAL_MEMORY_SIZE 1<<21
#define PHYSICAL_PAGES_NUMBER (PHYSICAL_MEMORY_SIZE/pageSize)
#define MAX_VIRTUAL_MEMORY_SIZE numberOfPages*pageSize

typedef struct block
{
	size_t size;
	unsigned int address;
	char isUse;
	unsigned int number;
} block;

//typedef struct block {
//	struct block * next;
//	struct block * prev;
//	size_t size;
//	unsigned int address;
//	char isUse;
//} block;

typedef struct page {
	unsigned long offset;
	char isUse;
	unsigned long timesUsed;
} page;

typedef char * VA;				//Тип описывающий адрес блока

char* memory = NULL;			//Физическая память

unsigned int numberOfPages;		//Число страниц
size_t pageSize;				//Размер страницы
unsigned int bitsForPageSize;	//Чсило бит для размера страницы

page * pageTable = NULL;		//Таблица страниц
block * blockTable = NULL;		//Указатель на первый блок

unsigned int numberOfBlocks;
unsigned int currentBlocksSize;

FILE * swap = NULL;				//Файл подкачки

/**
	@func	_init
	@brief	Инициализация модели менеджера памяти

	@param	[in] n		количество страниц
	@param	[in] szPage	размер страницы

	@return	код ошибки
	@retval	0	успешное выполнение
	@retval	-1	неверные параметры
	@retval	1	неизвестная ошибка
**/
MMEMORY_API int _init(int n, int szPage);

/**
	@func	_malloc
	@brief	Выделяет блок памяти определенного размера

	@param	[out] pointer		адресс блока
	@param	[in]  size	размер блока

	@return	код ошибки
	@retval	0	успешное выполнение
	@retval	-1	неверные параметры
	@retval	-2	нехватка памяти
	@retval	1	неизвестная ошибка
**/
MMEMORY_API int _malloc(VA* pointer, size_t size);

/**
	@func	_read
	@brief	Чтение информации из блока памяти

	@param	[in] pointer		адресс блока
	@param	[in] ptrBuffer	адресс буфера куда копируется инфомация
	@param	[in] bufferSize	размер буфера

	@return	код ошибки
	@retval	0	успешное выполнение
	@retval	-1	неверные параметры
	@retval	-2	доступ за пределы блока
	@retval	1	неизвестная ошибка
**/
MMEMORY_API int _read(VA pointer, void * ptrBuffer, size_t bufferSize);

/**
	@func	_write
	@brief	Запись информации в блок памяти

	@param	[in] pointer		адресс блока
	@param	[in] ptrBuffer		адресс буфера куда копируется инфомация
	@param	[in] bufferSize		размер буфера

	@return	код ошибки
	@retval	0	успешное выполнение
	@retval	-1	неверные параметры
	@retval	-2	доступ за пределы блока
	@retval	1	неизвестная ошибка
**/
MMEMORY_API int _write(VA pointer, void * ptrBuffer, size_t bufferSize);

/**
	@func	_free
	@brief	Удаление блока памяти

	@param	[in] pointer	адресс блока

	@return	код ошибки
	@retval	0	успешное выполнение
	@retval	-1	неверные параметры
	@retval	1	неизвестная ошибка
**/
MMEMORY_API int _free(VA pointer);
