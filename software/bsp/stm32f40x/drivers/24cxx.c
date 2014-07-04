//#include "stm32f10x.h"
//#include "includes.h"
#include "soft_i2c.h"
#include "finsh.h"
#include "24cxx.h"
#include "color.h"

#define OP_WRITE 0xa0          //写命令
#define OP_READ  0xa1           // 读命令

#define write_delay   6000

#define page_size 16
#define EE_MEM_SIZE 8192//FM24CL64BG

void write_byte(uint16_t addr, uint8_t write_data)
// 在指定地址addr处写入数据write_data
{
   start();
   I2C_tx(OP_WRITE);
   I2C_tx(addr>>8);
   I2C_tx(addr);
   I2C_tx(write_data);
   stop();
//   ee_delay(write_delay);          // 写入等待,某些EEPROM要等待一段时间才存贮完成.
}
/**********************************************************/
uint8_t read_current()
// 在当前地址读取
{
   uint8_t read_data;
   start();
   I2C_tx(OP_READ);
   read_data = I2C_rx();
   send_NACK();
   stop();
   return read_data;
}

void read_seq24cxx(uint8_t *read_data,uint8_t num)//连续读
{
	uint8_t i = 0;
	start();
	I2C_tx(OP_READ);
	for(i=0;i<num;i++)
	{
		*read_data++ = I2C_rx();
		if(i == (num-1))
		{
			send_NACK();
		}
		else
		{
			send_ACK();
		}
	}
	stop();
}

/**********************************************************/
uint8_t read_random(uint16_t random_addr)
// 在指定地址读取
{
   start();
   I2C_tx(OP_WRITE);
   I2C_tx(random_addr>>8);
   I2C_tx(random_addr);
   return(read_current());
}

void write_page(uint16_t addr,uint8_t *write_data)
// 在指定地址addr处写入数据write_data
{
	uint8_t i=0;
//	uint8_t *p = (uint8_t *)write_data;

	start();
	I2C_tx(OP_WRITE);
	I2C_tx(addr>>8);
	I2C_tx(addr);
	for(i = page_size; i > 0;i--)
	{
		I2C_tx(*write_data);
		write_data++;
	}
	stop();
//	ee_delay(write_delay);          // 写入等待,某些EEPROM要等待一段时间才存贮完成.
}

void read_page(uint16_t addr,uint8_t *read_data)
// 在指定地址读取
{	
	start();
	I2C_tx(OP_WRITE);
	I2C_tx(addr>>8);
	I2C_tx(addr);
	read_seq24cxx(read_data,page_size);
}

/**********************************************************/
uint8_t write_bytes(uint16_t addr,void *write_data,uint16_t num)
// 在指定地址addr处写入数据write_data
{
	uint8_t *p = (uint8_t *)write_data;

	if (addr >= EE_MEM_SIZE)
	{
		rt_kprintf("\033[47;31m parameter 'addr' is error!!!\n");
			return 0;		
	}
	else
	{
		if((addr + num) > EE_MEM_SIZE || ( num > EE_MEM_SIZE))
		{
			rt_kprintf("\033[47;31m parameter 'num' is error!!!\n");
			num = EE_MEM_SIZE - addr;
		}
	}

	i2c_lock();
	while (num >= page_size)
	{
		write_page(addr, p);
		addr += page_size;
		num -= page_size;
		p += page_size;  
	}
	
	while (num)
	{
		write_byte(addr++, *p++);
		num--;
	}
	i2c_unlock();
	return 1;
}

uint8_t read_bytes(uint16_t addr,void *read_data,uint16_t num)
// 在指定地址读取
{
	uint8_t *p = (uint8_t *)read_data;

	if (addr >= EE_MEM_SIZE)
	{
		rt_kprintf("\033[47;31m parameter 'addr' is error!!!\n");
			return 0;		
	}
	else
	{
		if((addr + num) > EE_MEM_SIZE || ( num > EE_MEM_SIZE))
		{
			rt_kprintf("\033[47;31m parameter 'num' is error!!!\n");
			num = EE_MEM_SIZE - addr;
		}
	}
	
	i2c_lock();
	while (num >= page_size)
	{
		read_page(addr, p);
		addr += page_size;
		num -= page_size;
		p += page_size;  
	}
	
	while (num)
	{
		*p++ = read_random(addr++);
		num--;
	}
	i2c_unlock();
	return 1;
}


#define DEBUG_24CXX 1
#if DEBUG_24CXX == 1
uint8_t read[page_size] = {0};
uint8_t write[page_size] = {0};

uint8_t  test_eeprom(void)
{
	uint8_t i = 0, err = 0;
	uint16_t j=0;
	
	for(j = 0; j < (EE_MEM_SIZE/page_size) ; j++)
	{
		rt_kprintf("test page %d!\n",j);
		for(i =0;i<page_size;i++)
		{
			write[i] = rand();
			read[i]= 0;
		}
		write_bytes(j*page_size,write,page_size);//
		read_bytes(j*page_size,read,page_size);//有些EEPROM 有页的概念,一次读写的最大数不能超过一页的字节数,而且一次读写只能读同一页.

		for(i =0;i<page_size;i++)
		{
			if(write[i] != read[i])
			{
				err = 1;
			}
		}
		if(err) rt_kprintf(red"test page %d read write error!\n"no_color,j);
	}
		
	if(err) rt_kprintf(red"test read write error!\n"no_color);
	else	rt_kprintf("test read write success!\n");
	return err;
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(test_eeprom, test eeprom);
#endif
#endif
//
void clear_eeprom(void)
{
	uint8_t clsnum[page_size] = {0};
	uint16_t i = 0,j = 0;
	for(i = 0; i< EE_MEM_SIZE/page_size; i++)
	{
		write_bytes(page_size*i, clsnum,page_size);
	}
	for(i = 0; i< EE_MEM_SIZE/page_size; i++)
	{
		read_bytes(i*page_size,clsnum ,page_size);
		for(j = 0; j< 16; j++)
		{
			if(clsnum[j] != 0)
			{
				rt_kprintf("clear eeprom error!\n");
			}
		}
	}
	
	rt_kprintf("clear eeprom success!\n");
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(clear_eeprom, clear_eeprom);
#endif


