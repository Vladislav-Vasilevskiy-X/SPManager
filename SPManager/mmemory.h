#ifndef MMEMORY_H_
#define MMEMORY_H_

#include<stdlib.h>
#include<stdio.h>
#include<sys/timeb.h>

#define TRUE 1
#define FALSE 0
#define PHYSICAL_MEMORY_SIZE 1<<20
#define PHYSICAL_PAGES_NUMBER (PHYSICAL_MEMORY_SIZE/pageSize)
#define MAX_VIRTUAL_MEMORY_SIZE numberOfPages*pageSize
#define VIRTUAL_MEMORY_SIZE 1<<21

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

typedef char * VA;				//��� ����������� ����� �����

char* memory = NULL;			//���������� ������

unsigned int numberOfPages;		//����� �������
size_t pageSize;				//������ ��������
unsigned int bitsForPageSize;	//����� ��� ��� ������� ��������

page * pageTable = NULL;		//������� �������
block * blockTable = NULL;		//��������� �� ������ ����

unsigned int numberOfBlocks;
unsigned int currentBlocksSize;

FILE * swap = NULL;				//���� ��������

/**
	@func	_init
	@brief	������������� ������ ��������� ������

	@param	[in] n		���������� �������
	@param	[in] szPage	������ ��������

	@return	��� ������
	@retval	0	�������� ����������
	@retval	-1	�������� ���������
	@retval	1	����������� ������
**/
int _init(int n, int szPage);

/**
	@func	_malloc
	@brief	�������� ���� ������ ������������� �������

	@param	[out] pointer		������ �����
	@param	[in]  size	������ �����

	@return	��� ������
	@retval	0	�������� ����������
	@retval	-1	�������� ���������
	@retval	-2	�������� ������
	@retval	1	����������� ������
**/
int _malloc(VA* pointer, size_t size);

/**
	@func	_read
	@brief	������ ���������� �� ����� ������

	@param	[in] pointer		������ �����
	@param	[in] ptrBuffer	������ ������ ���� ���������� ���������
	@param	[in] bufferSize	������ ������

	@return	��� ������
	@retval	0	�������� ����������
	@retval	-1	�������� ���������
	@retval	-2	������ �� ������� �����
	@retval	1	����������� ������
**/
int _read(VA pointer, void * ptrBuffer, size_t bufferSize);

/**
	@func	_write
	@brief	������ ���������� � ���� ������

	@param	[in] pointer		������ �����
	@param	[in] ptrBuffer		������ ������ ���� ���������� ���������
	@param	[in] bufferSize		������ ������

	@return	��� ������
	@retval	0	�������� ����������
	@retval	-1	�������� ���������
	@retval	-2	������ �� ������� �����
	@retval	1	����������� ������
**/
int _write(VA pointer, void * ptrBuffer, size_t bufferSize);

/**
	@func	_free
	@brief	�������� ����� ������

	@param	[in] pointer	������ �����

	@return	��� ������
	@retval	0	�������� ����������
	@retval	-1	�������� ���������
	@retval	1	����������� ������
**/
int _free(VA pointer);

#endif