#include <SLog/slog.h>
#include "header.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

slog_buff buff;
uint32_t read_le(uint8_t* ptr)
{
	uint32_t temp = 0;
	temp |= ((*ptr++)<<0)&0xFF;
	temp |= ((*ptr++)<<8)&0xFF00;
	temp |= ((*ptr++)<<16)&0xFF0000;
	temp |= ((*ptr++)<<24)&0xFF000000;
	return temp;
}

bool is_copied_ok(slog_entry *entry, slog_buff *buff)
{
	uint8_t args = entry->log_id&0x03;
	
	if(read_le(&(buff->data[buff->head-(args+2)*4]))!=entry->log_id)
	{
		printf("\nError, log_id: %d, should be: %d\n", \
		read_le(&(buff->data[buff->head-(args+2)*4])), entry->log_id);
		return false;
	}
	if(read_le(&(buff->data[buff->head-(args+1)*4]))!=entry->timestamp)
	{
		printf("\nError, timestamp: %d, should be: %d\n", \
		read_le(&(buff->data[buff->head-(args+1)*4])), entry->timestamp);
		return false;
	}
	for(int i=args-1;i>=0;i--)
	{
		if(read_le(&(buff->data[buff->head-args*4+i*4]))!=entry->args[i])
			return false;
	}
	return true;
}

bool buff_data_is_null(slog_buff *buff)
{
	for(int i=0;i<SLOG_BUFF_WORDS;i++)
	{
		if(buff->data[i]!=0x00000000)
			return false;
	}
	return true;
}

void test_slog_clear_buff()
{
	slog_buff buff;
	memset(&buff, 0xff, sizeof(slog_buff));
	slog_clear_buff(&buff);
	TEST_CHECK_P(buff.head==0, "buff.head should be zero.");
	TEST_CHECK_P(buff.data_lost==0, "buff.data_lost should be zero.");
}

void test_slog_push_entry_success()
{
	// Empty buffer
	for(int i=0;i<4;i++)
	{
		memset(&buff, 0x00, sizeof(slog_buff));
		slog_entry e = {0x0100+i, 0x002, {0x003, 0x004, 0x005}};
		slog_push_entry(&e, &buff);
		TEST_CHECK_P(buff.head==2+i, "Empty buffer, %i args - head should be updated.", i);
		TEST_CHECK_P(buff.data_lost==0, "Empty buffer, %i args - data_lost should be zero.", i);
		TEST_CHECK_P(is_copied_ok(&e, &buff), "Empty buffer, %i args - data should be copied.", i);
	}
	
	// Not empty buffer
	for(int i=0;i<4;i++)
	{
		memset(&buff, 0x00, sizeof(slog_buff));
		buff.head = 2;
		slog_entry e = {0x0100+i, 0x002, {0x003, 0x004, 0x005}};
		slog_push_entry(&e, &buff);
		TEST_CHECK_P(buff.head==4*2+4*i+2, "Not empty buffer, %i args - head should be updated.", i);
		TEST_CHECK_P(buff.data_lost==0, "Not empty buffer, %i args - data_lost should be zero.", i);
		TEST_CHECK_P(is_copied_ok(&e, &buff), "Not empty buffer, %i args - data should be copied.", i);
	}
	
	// Almost full buffer
	for(int i=0;i<4;i++)
	{
		memset(&buff, 0x00, sizeof(slog_buff));
		buff.head = SLOG_BUFF_WORDS-(2+i)-1;
		slog_entry e = {0x0100+i, 0x002, {0x003, 0x004, 0x005}};
		slog_push_entry(&e, &buff);
		TEST_CHECK_P(buff.head==SLOG_BUFF_WORDS-1, \
		"Almost full buffer, %i args - head should be updated.", i);
		TEST_CHECK_P(buff.data_lost==0, "Almost full buffer, %i args - data_lost should be zero.", i);
		TEST_CHECK_P(is_copied_ok(&e, &buff), "Almost full buffer, %i args - data should be copied.", i);
	}
	
	// To full
	for(int i=0;i<4;i++)
	{
		memset(&buff, 0x00, sizeof(slog_buff));
		buff.head = SLOG_BUFF_WORDS-(2+i);
		slog_entry e = {0x0100+i, 0x002, {0x003, 0x004, 0x005}};
		slog_push_entry(&e, &buff);
		TEST_CHECK_P(buff.head==SLOG_BUFF_WORDS, \
		"To full, %i args - head should be updated.", i);
		TEST_CHECK_P(buff.data_lost==0, "To full, %i args - data_lost should be zero.", i);
		TEST_CHECK_P(is_copied_ok(&e, &buff), "To full, %i args - data should be copied.", i);
	}
}

void test_slog_push_entry_fail()
{
	// Bufer empty, data_lost=1
	for(int i=0;i<4;i++)
	{
		memset(&buff, 0x00, sizeof(slog_buff));
		buff.data_lost = 1;
		slog_entry e = {0x0100+i, 0x002, {0x003, 0x004, 0x005}};
		slog_push_entry(&e, &buff);
		TEST_CHECK_P(buff.head==0, \
		"Bufer empty, data_lost=1, %i args - head should not be updated.", i);
		TEST_CHECK_P(buff.data_lost==1, \
		"Bufer empty, data_lost=1, %i args - data_lost should be set.", i);
		TEST_CHECK_P(buff_data_is_null(&buff), \
		"Bufer empty, data_lost=1, %i args - data should not be copied.", i);
	}
	
	// Bufer full, data_lost=0
	for(int i=0;i<4;i++)
	{
		memset(&buff, 0x00, sizeof(slog_buff));
		buff.head = SLOG_BUFF_WORDS;
		slog_entry e = {0x0100+i, 0x002, {0x003, 0x004, 0x005}};
		slog_push_entry(&e, &buff);
		TEST_CHECK_P(buff.head==SLOG_BUFF_WORDS, \
		"Bufer full, data_lost=0, %i args - head should not be updated.", i);
		TEST_CHECK_P(buff.data_lost==1, \
		"Bufer full, data_lost=0, %i args - data_lost should be set.", i);
		TEST_CHECK_P(buff_data_is_null(&buff), \
		"Bufer full, data_lost=0, %i args - data should not be copied.", i);
	}
	
	// Bufer almost full, data_lost=0
	for(int i=0;i<4;i++)
	{
		memset(&buff, 0x00, sizeof(slog_buff));
		buff.head = SLOG_BUFF_WORDS-(2+i)+1;
		slog_entry e = {0x0100+i, 0x002, {0x003, 0x004, 0x005}};
		slog_push_entry(&e, &buff);
		TEST_CHECK_P(buff.head==SLOG_BUFF_WORDS-(2+i)+1, \
		"Bufer almost full, data_lost=0, %i args - head should not be updated.", i);
		TEST_CHECK_P(buff.data_lost==1, \
		"Bufer almost full, data_lost=0, %i args - data_lost should be set.", i);
		TEST_CHECK_P(buff_data_is_null(&buff), \
		"Bufer almost full, data_lost=0, %i args - data should not be copied.", i);
	}
}

