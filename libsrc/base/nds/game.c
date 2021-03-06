//game.c
//version 1.0

#define T()                   //   printf("__________%s; %d; %s;\n",__FILE__,__LINE__,__FUNCTION__)          // TRACE
 
//#define printf(...) 

#if 1

#define virt_to_phys(vaddr) ((vaddr) & 0x1fffffff)
#define printk printf

#define irqreturn_t
#define IRQ_HANDLED 0

#define DMA_ID_AUTO  5
#define copy_to_user memcpy
#define copy_from_user memcpy
#define dma_cache_wback _dcache_wback_inv 

#define EFAULT -1
#endif

#define  AUDIO_PRI_FISRT 1

#define enable_cache 1
#define enable_initfpga 1

#include "game.h"
#include "mipsregs.h"
#include "memory.h"

//MODULE_AUTHOR("game");
//MODULE_LICENSE("Dual BSD/GPL");

//#define HTW_NDS_MP4
#define FPGA_FIFO_SIZE 1024

struct MP4_dev* MP4_devices=NULL;
static unsigned char MP4_inc=0;

static u32 cmd_buf32[2];

static u32 nds_iqe_len; 
static int nds_iqe_addr;
static u32 cpu_iqe_num; 
static u32 mm_check; 
 u32 kmalloc_ptr;

 u32 get_kmalloc_ptr(){return kmalloc_ptr;}
 u32 kmalloc_ptr_uncache;

extern unsigned short __brightness_state;
struct rtc * pnds_sys_rtc = NULL;

static int dma_chan;
static struct main_buf *pmain_buf;

static struct nds_iqe_list_st nds_iqe_list[0x10];
static int nds_iqe_list_w;
static int nds_iqe_list_r;
static int _nds_iqe_list_w_nest;

const unsigned int mmdata10[]={
0x59a65077,0x969e2070,0x05a236c5,0xe266f727,0x47c42e3d,0x7702e0c7,0x386e4922,0x6fc94d48,0xc2e67712,0x21cd84c0,0x8cc57bd1,0x64c4b77a,0x4cde0879,0xc1fdc29f,0x24282977,0xea172048,
0x89286382,0xc0cdcda6,0x07795ff0,0xb71e7f79,0xde760be6,0x03a71aaa,0x32ffd611,0xd1893538,0x4cdce727,0x5f51cc6b,0x35e84f48,0xd9f41b30,0xe767921f,0x0f93a56b,0xea5acf79,0x2c16c530,
0xc9e5cfbd,0x67f20c2b,0xd538c21f,0x5befe661,0x2502cad0,0xf90b8ed6,0x7dfdcee1,0x79c37c7c,0x5fe48ae2,0x72a9b1e1,0x5495c852,0x133ef240,0x205e87c0,0xfb428f89,0x39570533,0x0cc40eac,
0x1dc471e0,0xf08ad743,0xc7ee7198,0xeb9b526f,0x3f15b6be,0x8b559e96,0xe84b0510,0x14fbf3f7,0x43fa3c6f,0xaf115a3d,0x5e55d8f5,0x209c0a0a,0xf33cd3bc,0x6cb644f4,0xc3ab45a4,0x8bea517f,
0xa843a8c3,0x141c5a4e,0x8cfb2b1c,0xa78fece4,0x7a3c6854,0x9e95687c,0x6e869daa,0x969e5572,0xffb58b23,0xd3c9a0d2,0x26ffc502,0x0df87997,0x629db680,0xc959aa90,0xba7cd136,0x4078db4b,
0x22b53f84,0xa8164dd6,0x7b01795c,0xf6c2a401,0x00819755,0x79046bd5,0xeae74254,0x8e6a2c0d,0x0c2a257e,0x7caaadf8,0xa4aea7e6,0xf58385f6,0x90f2ed1e,0x0e908aba,0xf8bf6e98,0xe0b0ae4a,
0x39b014b1,0x24dc0c3e,0xbed92dfc,0x1b4b97be,0x40afc581,0x02e58f1c,0x222ff046,0xbeb97ff3,0xd2700850,0x8264ec40,0x42ad9401,0xa3ed3f2c,0x54b76f3d,0x1fc41b0a,0x2d42db97,0x88ef3629,
0xe171d4cf,0x25adafb4,0xa50d10d7,0xd9157376,0xa4ef30fd,0x88e55cbf,0x52608d99,0x1bcc2cde,0xee786b99,0xe8f370d6,0x6e1c3361,0xbc020225,0xf4425f68,0x9375f75b,0x5bd624ef,0x7733a892,
0x097619ef,0x64fb17a8,0x70fec00d,0xcf1b6d30,0x064e7068,0xdb3f8e73,0xfdba3ec9,0x7bef0b47,0x701380fa,0xbf5bf403,0xd398ea7f,0xdfe9059f,0x810a0623,0x991b5466,0x121bc87e,0xcb544ed6,
0xd7ca8714,0x5bdb4a42,0xaf7fdc1f,0x5bdf20a3,0xa8eb5af1,0x8f085cc4,0x3b1e3970,0x346e9e96,0x7b1ee130,0x7f047e1e,0x512d3141,0x9f040c6a,0x494e32b6,0xa316fc7e,0xc9f17d2f,0x3be035b3,
0x0a70cc91,0x9766b1c0,0x9adcc874,0x9eb6c5da,0x744eeded,0xab4f081d,0x908b8ba7,0xa5592e32,0xf1df5fb9,0xa87ba103,0xa4828f2b,0x286795fe,0x19c98698,0x8b80d3df,0xc9a253d6,0x900ec28b,
0x4c935faf,0x05b9080c,0x8eab2674,0x08cde155,0xd3f80f48,0x06319a7b,0x1e5d44d6,0xbe33151f,0x734ec23b,0x6cbf5234,0xe6769150,0x1b5d8f69,0x861890f6,0xc01686d1,0xa525b1e7,0xe4b97ea5,
0x7316b2bc,0x7781a657,0x4aeab51d,0xd00e78bc,0xca9f1d48,0x82ab86b1,0x6a60acde,0xb0bce8e6,0xa4a114ca,0xc71ba28f,0xe95229ab,0x38211680,0xe8027b91,0x3e1c3e72,0xd03d1385,0x4b4307d9,
0x3ca1e4af,0x6a146027,0xab80004d,0x37a8d8cc,0x2d256df8,0x525efb06,0xc4c72cf6,0x2ace633c,0xe291721e,0xfd8ade68,0x33e02470,0xa39e1804,0xb6c4e531,0x63f0c555,0x2a8f96d1,0xb9e97c3f,
0xe37f33d4,0xba17c763,0xe9c586ef,0x76493483,0x2bd6c47b,0xd68fdcfe,0x903cb69c,0x4a55827d,0x891d7db1,0x014a2e25,0x66206c73,0x589f55f3,0xc4808b86,0xb0b8013a,0x715435bd,0x7a6a9c0b,
0x3bc6c48d,0xbb6f2402,0x36e52c30,0x39cafd06,0x09ef1596,0x8e8a4e05,0x5a1c8bc6,0xcc58c398,0x8e37603b,0xa0433f6f,0x86c72c14,0xd3f2d9e6,0x00957ebe,0x621a0a62,0xc31c0923,0x6f99005b

};

#ifdef HTW_MP4_NDS
 //static __DECLARE_SEMAPHORE_GENERIC(MP4_keybd_cnt, 0);
 //extern unsigned long volatile __jiffy_data jiffies;
unsigned int nds_updat_tim = 0;
#endif

static int MP4_open(struct inode *inode, struct file *filp)
{
#if 0
	struct MP4_dev *dev;
#ifndef HTW_MP4_NDS
	if(MP4_inc>0)return -ERESTARTSYS;
#endif	
	MP4_inc++;
 
	dev = container_of(inode->i_cdev, struct MP4_dev, cdev);
	filp->private_data = dev;
	return 0;
#endif	
}

static int MP4_release(struct inode *inode, struct file *filp)
{
#if 0
	MP4_inc--;
	return 0;
#endif	
}

static ssize_t MP4_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos)
{
	return 0;
}

static ssize_t MP4_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos)
{
	return 0;
}

static int dma_nodesc_write(u32 s,u32 d,int len,int enable_iqe)
{
	REG_DMAC_DCCSR(dma_chan) = 0;
	REG_DMAC_DRSR(dma_chan) = DMAC_DRSR_RS_AUTO;
#if enable_cache
	REG_DMAC_DSAR(dma_chan) = (u32)virt_to_phys(s);
#else
	REG_DMAC_DSAR(dma_chan) = s & 0x1fffffff;// (u32)virt_to_phys((void *)s);
#endif
	REG_DMAC_DTAR(dma_chan) = d & 0x1fffffff;
	REG_DMAC_DTCR(dma_chan) = len / 32;
    //if (enable_iqe ==1)
    //{
	//REG_DMAC_DCMD(dma_chan) = DMAC_DCMD_SAI | DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 | DMAC_DCMD_DS_32BYTE | DMAC_DCMD_TIE;
    //}
    //else
    //{
        REG_DMAC_DCMD(dma_chan) = DMAC_DCMD_SAI | DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_16 | DMAC_DCMD_DS_32BYTE ;//| DMAC_DCMD_TIE;
    //}

	REG_DMAC_DCCSR(dma_chan) = DMAC_DCCSR_NDES | DMAC_DCCSR_EN;

    return 0;
}

static void Enable_game_data_interrup(void)
{
__gpio_as_irq_rise_edge(fpga_data_port + nds_data_iqe_pinx);    //data
__gpio_unmask_irq(fpga_data_port + nds_data_iqe_pinx);
}

static void Disenable_game_data_interrup(void)
{
__gpio_as_disenable_irq(fpga_data_port + nds_data_iqe_pinx);

}

static void Enable_game_cmd_interrup(void)
{
__gpio_as_irq_fall_edge(fpga_cmd_port + nds_cmd_iqe_pinx);
__gpio_unmask_irq(fpga_cmd_port + nds_cmd_iqe_pinx);

}

static void Disenable_game_cmd_interrup(void)
{
__gpio_as_disenable_irq(fpga_cmd_port + nds_cmd_iqe_pinx);

}


extern void check_reg(int); 

void ndelay(void)
{

	volatile int i= 5;
	while(i--);

}

typedef void (*pfunc)(void);

static pfunc _escape_key_function = NULL;
static unsigned int _escape_key = -1;
static unsigned int _escape_function_lock = 0;

void regist_escape_key(void (*fun)(void), unsigned int key)
{
	if(NULL == fun || 0 == key)
		return;

	_escape_key_function = fun;
	_escape_key = key;
	_escape_function_lock = 0;
}

void release_escape_key(void)
{
	_escape_key_function = NULL;
	_escape_key = -1;
	_escape_function_lock = 0;
}

static irqreturn_t cmd_line_interrupt(int irq, void *dev_id)
{
    u8 nds_iqe_cmd;
    int i;
    u32 tempw,tempr;
    u8 isc1cmd;
    struct key_buf* keyp;

    u8* cmd_buf=(u8*)&cmd_buf32[0];
//    cmd_buf32[0]=*(vu32*)cpld_fifo_read_ndswcmd_addr;
//    cmd_buf32[1]=*(vu32*)cpld_fifo_read_ndswcmd_addr;

	cmd_buf32[0]=*(vu16*)cpld_fifo_read_ndswcmd_addr + ((*(vu16*)cpld_fifo_read_ndswcmd_addr)<<16);
	cmd_buf32[1]=*(vu16*)cpld_fifo_read_ndswcmd_addr + ((*(vu16*)cpld_fifo_read_ndswcmd_addr)<<16);
    nds_iqe_cmd = cmd_buf[0];

	isc1cmd= mm_check ==0 ? 0: (1<< enable_fix_video_rgb_bit);
    switch ( nds_iqe_cmd )
    {
        case 0xc0://Set NDS's cmd or request a/v data transfer's cmd
            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit) |  (1<<fifo_clear_bit);
            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit);
            *(fpgaport*)cpld_state_addr = 0;

			dma_nodesc_write(nds_iqe_list[nds_iqe_list_r&0xf].c0_addr, cpld_fifo_write_ndsrdata_addr, 512, 0);

            i = nds_iqe_list_w - nds_iqe_list_r;
            if (i != 0 && nds_iqe_list_w > nds_iqe_list_r)
            {
                if (nds_iqe_list[nds_iqe_list_r&0xf].num == buf_c0_set)
                    pmain_buf->buf_st_list[buf_c0_set].isused=0;

                nds_iqe_list_r++;
            }
            else
                dgprintf( "nds_iqe c0 error");

            break;
        case 0xc1://VIDEO 512*n
            isc1cmd = cmd_buf[7] & (( 1 << enable_fix_video_bit) | ( 1 << enable_fix_video_rgb_bit));

        case 0xc2://AUDIO 512*n
            //*(fpgaport*)write_addr_cmp_addr = (0x400-0x380) ;

			SET_ADDR_GROUP(GPIO_ADDR_GROUP1);
			*(fpgaport*)(cpld_base_addr + cpld_base_step) = (0x400-0x380) ; //(0x400-0x300) ;
			SET_ADDR_DEFT();

            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit) |  (1<<fifo_clear_bit) | isc1cmd;
            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit) | isc1cmd;

			nds_iqe_addr = (cmd_buf[1]<<24) + (cmd_buf[2]<<16) +(cmd_buf[3]<<8)+(cmd_buf[4]<<0);
			nds_iqe_len = (cmd_buf[5]<<16) + (cmd_buf[6]<<8) + (cmd_buf[7]<<0);
			if (nds_iqe_len >= 0x800000)
			{//transfer over
				//*(vu32*)cpld_fifo_write_ndsrdata_addr = 0;

				*(vu16*)cpld_fifo_write_ndsrdata_addr = 0;
				*(vu16*)cpld_fifo_write_ndsrdata_addr = 0;

				nds_iqe_len &= 0xff;
				if (nds_iqe_len < buf_max_num) {
					pmain_buf->buf_st_list[nds_iqe_len].isused=0;
					if(nds_iqe_len == buf_audio_0 || nds_iqe_len == buf_audio_1)
						pmain_buf->nds_audio_r += 1;
				}
				else {
					//There are something wrong
					dgprintf( "buf_max_num error %x" ,nds_iqe_len);
					//clear all video and audio queue
					pmain_buf->buf_st_list[buf_video_up_0].isused=0;
					pmain_buf->buf_st_list[buf_video_up_1].isused=0;
					pmain_buf->buf_st_list[buf_video_down_0].isused=0;
					pmain_buf->buf_st_list[buf_video_down_1].isused=0;
					pmain_buf->buf_st_list[buf_audio_0].isused=0;
					pmain_buf->buf_st_list[buf_audio_1].isused=0;
				}

				Disenable_game_data_interrup();
			}
			else
			{//transfer commence
				nds_iqe_len &= ~0xff;
				i = FPGA_FIFO_SIZE;	//1024
				if(nds_iqe_len < FPGA_FIFO_SIZE) {
					i = 512;
					nds_iqe_len = 0;
				}
				else {
					i = FPGA_FIFO_SIZE;
					nds_iqe_len -= i;
				}

                nds_iqe_addr += kmalloc_ptr_uncache;
                dma_nodesc_write(nds_iqe_addr, cpld_fifo_write_ndsrdata_addr, i, 0);
                nds_iqe_addr += i;
				
				if(nds_iqe_len>0)
					Enable_game_data_interrup();
                else
					Disenable_game_data_interrup();
            }

            break;
        case 0xc3://NDS key
            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit) |  (1<<fifo_clear_bit);
            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit);
		    //*(vu32*)cpld_fifo_write_ndsrdata_addr = 0;
			*(vu16*)cpld_fifo_write_ndsrdata_addr = 0;
			*(vu16*)cpld_fifo_write_ndsrdata_addr = 0;

            i= pmain_buf->key_write_num  - pmain_buf->key_read_num;
            if (i!= 0 && ((i& KEY_MASK) ==0))//full
            {                 
                pmain_buf->key_write_num--;                
            }
            else 
            {
#ifdef HTW_NDS_MP4
            os_SemaphorePost(MP4_devices->sem4key);
#endif            
            }

			//capture escape key
			i = (cmd_buf[5]<<8) + cmd_buf[6];
			if(i == _escape_key)
			{
				if(NULL != _escape_key_function && !_escape_function_lock) {
					_escape_function_lock = 1;
					sti();	//enable interrupt nested
					_escape_key_function();
					_escape_function_lock = 0;
				}
			}
			else
			{
	            keyp =(struct key_buf*)(kmalloc_ptr_uncache + pmain_buf->key_buf_offset +
					((pmain_buf->key_write_num & KEY_MASK) * sizeof(struct key_buf)));
	            keyp->x=(cmd_buf[1]<<8) + cmd_buf[2];
	            keyp->y=(cmd_buf[3]<<8) + cmd_buf[4];
	            keyp->key=(cmd_buf[5]<<8) + cmd_buf[6];
	            //keyp->brightness=cmd_buf[7];
				__brightness_state = cmd_buf[7];

	            mm_check =((cmd_buf[7] &( ((1)<<6) | ((1)<<7))) != ( ((1)<<6) | ((1)<<7)))?1:0;

	            pmain_buf->key_write_num++;
			}

            break;
        case 0xc5://NDS timer
            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit) |  (1<<fifo_clear_bit);
            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit);
            //*(vu32*)cpld_fifo_write_ndsrdata_addr = 0;

			*(vu16*)cpld_fifo_write_ndsrdata_addr = 0;

			*(vu16*)cpld_fifo_write_ndsrdata_addr = 0;
            memcpy(&pmain_buf->nds_rtc,&cmd_buf[1],7);

            break;
        case 0xc4://NDS iqe
            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit) |  (1<<fifo_clear_bit);
            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit);
            tempw=nds_iqe_list_w - nds_iqe_list_r;
            //*(vu32*)cpld_fifo_write_ndsrdata_addr = tempw | 0xa5a50000;
			*(vu16*)cpld_fifo_write_ndsrdata_addr = tempw | 0xa5a50000;

			*(vu16*)cpld_fifo_write_ndsrdata_addr = (tempw | 0xa5a50000)>>16;
            if(irq == 0x5a5a5a5a )	break;

            tempw=(cmd_buf[1]<<16) +(cmd_buf[2]<<8)+(cmd_buf[3]<<0);
            tempr=(cmd_buf[4]<<16) +(cmd_buf[5]<<8)+(cmd_buf[6]<<0);
		
            if(cmd_buf[7] == 1)
            {
                if (pmain_buf->nds_video_up_w < tempw)
                    pmain_buf->nds_video_up_w=tempw;
                pmain_buf->nds_video_up_r=tempr;
            }else if(cmd_buf[7] == 2)
            {
                if(pmain_buf->nds_audio_w < tempw)
                {
                    pmain_buf->nds_audio_w=tempw;
                }
                if(pmain_buf->nds_audio_r != tempr)
                {
	                pmain_buf->nds_audio_r=tempr;
#ifdef HTW_MP4_NDS
                    nds_updat_tim = os_TimeGet();
#endif
                }
            }else if(cmd_buf[7] == 3)
            {
                if (pmain_buf->nds_video_down_w < tempw)
                    pmain_buf->nds_video_down_w=tempw;
                pmain_buf->nds_video_down_r=tempr;
            }
#if 1
			else if(cmd_buf[7] == 0xf0)//debug
            {
				//sti();
                cprintf( "nds_video_up_w=%x\n" ,pmain_buf->nds_video_up_w);
                cprintf( "nds_video_up_r=%x\n" ,pmain_buf->nds_video_up_r);
                cprintf( "nds_video_down_w=%x\n" ,pmain_buf->nds_video_down_w);
                cprintf( "nds_video_down_r=%x\n" ,pmain_buf->nds_video_down_r);
                cprintf( "nds_audio_w=%x\n" ,pmain_buf->nds_audio_w);
                cprintf( "nds_audio_r=%x\n" ,pmain_buf->nds_audio_r);
                cprintf( "nds_iqe_list_r=%x\n" ,nds_iqe_list_r);
                cprintf( "nds_iqe_list_w=%x\n" ,nds_iqe_list_w);

                cprintf("PC= %08x\n", read_c0_epc());
            }
#endif
 
#if 0//enable_cache
            {
            int i = nds_iqe_list_w - nds_iqe_list_r;
            if (i != 0 && nds_iqe_list_w > nds_iqe_list_r)
            {
				struct nds_iqe_st *nds_iqe_st_p = (struct nds_iqe_st *)nds_iqe_list[nds_iqe_list_r&0xf].c0_addr;
				dma_cache_wback_inv_my((unsigned long)(nds_iqe_st_p), (unsigned long)512);
				if(nds_iqe_st_p->iqe_cmd == 0xc2)//==0xc0 ????????
				{
		        	dma_cache_wback_inv_my((unsigned long)(nds_iqe_st_p->iqe_cpuaddr + kmalloc_ptr), (unsigned long)nds_iqe_st_p->iqe_len);
				}
			}	
	    }
#endif 
            break;
         case 0x5d://??????

			*(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit) |  (1<<fifo_clear_bit) | (1<<7);
            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit)| (1<<7);
			//*(fpgaport*)cpld_spi_Count_addr = 0x1234;

			SET_ADDR_GROUP(GPIO_ADDR_GROUP1);

			*(fpgaport*)(cpld_base_addr + cpld_base_step*3) = 0x1234;

			SET_ADDR_DEFT();

            dma_nodesc_write( (u32)mmdata10 ,cpld_fifo_write_ndsrdata_addr,1024,0);
            break;

        case 0x5e://??????
			*(fpgaport*)cpld_ctr_addr =(1<<fpga_mode_bit) | (1<<fifo_clear_bit) |BIT(key2_init_enable_bit);
            *(fpgaport*)cpld_ctr_addr =(1<<fpga_mode_bit) | BIT(key2_init_enable_bit);

            //*(fpgaport*)cpld_x0_addr=0;
            //*(fpgaport*)cpld_x1_addr=0;
            //*(fpgaport*)cpld_x4_addr=0;
			SET_ADDR_GROUP(GPIO_ADDR_GROUP2);

			*(fpgaport*)cpld_base_addr = 0;
            *(fpgaport*)(cpld_base_addr + cpld_base_step*1) = 0;

			*(fpgaport*)(cpld_base_addr + cpld_base_step*2) = 0;
            SET_ADDR_DEFT();

            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit) |BIT(fifo_clear_bit);
            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit) |BIT(enable_fix_video_bit) | BIT(enable_fix_video_rgb_bit);
            i= (*(vu16*)cpld_state_addr) & BIT(10);
            if (i!=0)           i= 1;
            else                i=0;
            #define RGB15_T(r,g,b)  (((r)|((g)<<5)|((b)<<10)))
            
            //*(vu32*)cpld_fifo_write_ndsrdata_addr = ((RGB15_T(0x4,i,0x7))<<16) | RGB15_T(0xc,i,0x5);

			*(fpgaport*)cpld_fifo_write_ndsrdata_addr = RGB15_T(0xc,i,0x5);
			*(fpgaport*)cpld_fifo_write_ndsrdata_addr = RGB15_T(0x4,i,0x7);
            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit) | BIT(key2_init_enable_bit);

            //*(fpgaport*)cpld_x0_addr= 0x6bf3;
            //*(fpgaport*)cpld_x1_addr= 0xf0c2;
            //*(fpgaport*)cpld_x4_addr= 0x9252;
			SET_ADDR_GROUP(GPIO_ADDR_GROUP2);
			*(fpgaport*)cpld_base_addr = 0x6bf3;
            *(fpgaport*)(cpld_base_addr + cpld_base_step*1) = 0xf0c2;

			*(fpgaport*)(cpld_base_addr + cpld_base_step*2) = 0x9252;
            SET_ADDR_DEFT();

            *(fpgaport*)cpld_ctr_addr =(1<<fpga_mode_bit);
            break;
            
        default:
            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit) |  (1<<fifo_clear_bit);
            *(fpgaport*)cpld_ctr_addr = (1<<fpga_mode_bit);
            *(fpgaport*)cpld_state_addr = 0;
            //*(vu32*)cpld_fifo_write_ndsrdata_addr = 0;
            *(vu16*)cpld_fifo_write_ndsrdata_addr = 0;

			*(vu16*)cpld_fifo_write_ndsrdata_addr = 0;
            break;
    } 

	return IRQ_HANDLED;
}

static void data_line_interrupt(int arg)
{
	int i;

	u8 nds_iqe_cmd=cmd_buf32[0] & 0xff;
    switch (nds_iqe_cmd)
    {
		case 0xc1://video 
		case 0xc2://audio
			if(nds_iqe_len < FPGA_FIFO_SIZE) {
				i = 512;
				nds_iqe_len = 0;
			}
			else {
				i = FPGA_FIFO_SIZE;
				nds_iqe_len -= FPGA_FIFO_SIZE;
			}

            dma_nodesc_write(nds_iqe_addr, cpld_fifo_write_ndsrdata_addr, i, 0);
            nds_iqe_addr += FPGA_FIFO_SIZE;

            if(nds_iqe_len <= 0)
                Disenable_game_data_interrup();

            break;
    }
	arg = arg;
}

//static
int __do_MP4_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    u32 temp32;
    u32 temp32buf[10];
    struct nds_iqe_st *nds_iqe_st_p;
	int irq_save, nds_iqe_list_w_tmp;

	if(IQE_UPDATE == cmd)
	{
        temp32 = nds_iqe_list_w - nds_iqe_list_r;
		//In fact temp32 > buf_max_num could not happen, if dose this, there are
		//errror when update nds_iqe_list_w & nds_iqe_list_r, of course, wu asume
		//nds_iqe_list_w & nds_iqe_list_r always increasing
        {
            if (((u32)arg) >= buf_max_num) {
                dgprintf( "buf_max_num2 error %x" ,(u32)arg);
                return -EFAULT;
            }

            if (pmain_buf->buf_st_list[arg].isused != 0)
            {
				//before call this function, be sure the queue is idle
				dgprintf( "queue isused error %x" ,pmain_buf->buf_st_list[arg].isused);
				return -EFAULT;
            }

			//write request +1
			irq_save = spin_lock_irqsave();
			_nds_iqe_list_w_nest += 1;
			pmain_buf->buf_st_list[arg].isused = 1;
			nds_iqe_list_w_tmp = nds_iqe_list_w;
        	nds_iqe_list_w++;
			spin_unlock_irqrestore(irq_save);

            nds_iqe_list[nds_iqe_list_w_tmp&0xf].num = arg;
            nds_iqe_st_p = (struct nds_iqe_st *)nds_iqe_list[nds_iqe_list_w_tmp&0xf].c0_addr;

            nds_iqe_st_p->iqe_cmd = pmain_buf->buf_st_list[arg].nds_cmd;
            nds_iqe_st_p->iqe_ndsaddr = 0;
            nds_iqe_st_p->iqe_cpuaddr = pmain_buf->buf_st_list[arg].offset;
            nds_iqe_st_p->iqe_datatype = pmain_buf->buf_st_list[arg].type;
            nds_iqe_st_p->iqe_len = pmain_buf->buf_st_list[arg].use_len;

            cpu_iqe_num++;
            nds_iqe_st_p->iqe_num = cpu_iqe_num;
        }

        if(((nds_iqe_st_p->iqe_cmd&0xff) == 0xc2) || ((nds_iqe_st_p->iqe_cmd&0xff) == 0xc1)) 
        {
            switch ((nds_iqe_st_p->iqe_cmd>>8) & 0xff)
            {
                case VIDEO_UP:
                    pmain_buf->nds_video_up_w++;
                    break;
                case VIDEO_DOWN:
                    pmain_buf->nds_video_down_w++;
                    break;
                case AUDIO_G:                    
					//FIXME: audio queue priority?
					pmain_buf->nds_audio_w++;
                    break;
            }
        }
        else if((nds_iqe_st_p->iqe_cmd&0xff) == 0xc0)
        {
			memcpy((char*)(nds_iqe_st_p), (char*)(nds_iqe_st_p->iqe_cpuaddr + kmalloc_ptr_uncache),512);
        }
        else
        {//this could not happen, 
            dgprintf("iqe_cmd error %x\n",nds_iqe_st_p->iqe_cmd);
            return -EFAULT;
        }

#if enable_cache
        _dcache_wback_inv((unsigned long)(nds_iqe_st_p), (unsigned long)512);
        if(((nds_iqe_st_p->iqe_cmd&0xff) == 0xc2) || ((nds_iqe_st_p->iqe_cmd&0xff) == 0xc1)) 
        {
            _dcache_wback_inv((unsigned long)(nds_iqe_st_p->iqe_cpuaddr + kmalloc_ptr),
				(unsigned long)nds_iqe_st_p->iqe_len);
        }
#endif

		irq_save = spin_lock_irqsave();
		_nds_iqe_list_w_nest -= 1;
		spin_unlock_irqrestore(irq_save);

		if(0 == _nds_iqe_list_w_nest)
		{
			*(fpgaport*)cpld_state_addr = BIT(nds_iqe_out_bit);
		}

		return 0;
    }
    else if(cmd== SET_CTR)
    {
        *(fpgaport*)cpld_ctr_addr = *(fpgaport*)cpld_ctr_addr | arg;
        return 0;
    }
    else if (cmd==CLR_CTR)
    {
        *(fpgaport*)cpld_ctr_addr = *(fpgaport*)cpld_ctr_addr & (~arg);
        return 0;
    }
    else if (cmd==READ_STATE)
    {
        temp32=*(fpgaport*)cpld_state_addr;
        copy_to_user((void*)arg, &temp32, 4);
        return 0;
    }
    else if (cmd==READ_IO)
    {
        copy_from_user(temp32buf,(void*)arg,2*4);
        temp32=*(fpgaport*)temp32buf[0];
        copy_to_user((void*)temp32buf[1], &temp32, 4);
        return 0;
    }
    else if (cmd==WRITE_IO)
    {
        copy_from_user(temp32buf,(void*)arg,2*4);
        *(fpgaport*)temp32buf[0]=temp32buf[1];
        return 0;
    }
    else if (cmd==IQE_READ)//c8
    {
        return -EFAULT;
    }
    else if (cmd==COMPLETE_STATE)
    {
        if (nds_cmd_iqe_pin !=0 ) temp32=1;
        else    temp32=0;
        
		if(nds_iqe_list_w == nds_iqe_list_r)
			temp32 |= 2;

        copy_to_user((void*)arg,&temp32,4);
        return 0;
    }
    else if (cmd==ENABLE_FIXRGB_UPVIDEO)//c8
    {
        if (arg != 0)
        {
            pmain_buf->buf_st_list[buf_video_up_0].nds_cmd = (((1 <<enable_fix_video_bit ) | (1 <<enable_fix_video_rgb_bit ))<<24) | (buf_video_up_0<<16) | (VIDEO_UP <<8) | 0xc1;
            pmain_buf->buf_st_list[buf_video_up_1].nds_cmd = (((1 <<enable_fix_video_bit ) | (1 <<enable_fix_video_rgb_bit ))<<24) | (buf_video_up_1<<16) | (VIDEO_UP <<8) | 0xc1;
        }
        else
        {
            pmain_buf->buf_st_list[buf_video_up_0].nds_cmd = (((1 <<enable_fix_video_bit))<< 24) | (buf_video_up_0<<16) | (VIDEO_UP <<8) | 0xc1;
            pmain_buf->buf_st_list[buf_video_up_1].nds_cmd = (((1 <<enable_fix_video_bit))<< 24) | (buf_video_up_1<<16) | (VIDEO_UP <<8) | 0xc1;
        }

        return 0;
    }
    else if (cmd==ENABLE_FIXRGB_DOWNVIDEO)//c8
    {
        if (arg != 0)
        {
            pmain_buf->buf_st_list[buf_video_down_0].nds_cmd = (((1 <<enable_fix_video_bit ) | (1 <<enable_fix_video_rgb_bit ))<<24) | (buf_video_down_0<<16) | (VIDEO_DOWN <<8) | 0xc1;
            pmain_buf->buf_st_list[buf_video_down_1].nds_cmd = (((1 <<enable_fix_video_bit ) | (1 <<enable_fix_video_rgb_bit ))<<24) |(buf_video_down_1<<16) |  (VIDEO_DOWN <<8) | 0xc1;
        }
        else
        {
            pmain_buf->buf_st_list[buf_video_down_0].nds_cmd = (((1 <<enable_fix_video_bit ) )<<24) | (buf_video_down_0<<16) | (VIDEO_DOWN <<8) | 0xc1;
            pmain_buf->buf_st_list[buf_video_down_1].nds_cmd = (((1 <<enable_fix_video_bit ) )<<24) | (buf_video_down_1<<16) | (VIDEO_DOWN <<8) | 0xc1;
        }

        return 0;
    }
    return -EFAULT;
}

#if 1 //def HTW_NDS_MP4
#define __MP4_ioctl_fb -1
#define __MP4_ioctl_dsp -1
#else

static int __MP4_ioctl_fb(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg)
{
    //int ret = -1;
	        struct fb_var_screeninfo var_tmp;


struct fb_var_screeninfo {
	__u32 xres;			/* visible resolution		*/
	__u32 yres;
	__u32 xres_virtual;		/* virtual resolution		*/
	__u32 yres_virtual;
	__u32 xoffset;			/* offset from virtual to visible */
	__u32 yoffset;			/* resolution			*/

	__u32 bits_per_pixel;		/* guess what			*/
	__u32 grayscale;		/* != 0 Graylevels instead of colors */

	struct fb_bitfield red;		/* bitfield in fb mem if true color, */
	struct fb_bitfield green;	/* else only length is significant */
	struct fb_bitfield blue;
	struct fb_bitfield transp;	/* transparency			*/	

	__u32 nonstd;			/* != 0 Non standard pixel format */

	__u32 activate;			/* see FB_ACTIVATE_*		*/

	__u32 height;			/* height of picture in mm    */
	__u32 width;			/* width of picture in mm     */

	__u32 accel_flags;		/* (OBSOLETE) see fb_info.flags */

	/* Timing: All values in pixclocks, except pixclock (of course) */
	__u32 pixclock;			/* pixel clock in ps (pico seconds) */
	__u32 left_margin;		/* time from sync to picture	*/
	__u32 right_margin;		/* time from picture to sync	*/
	__u32 upper_margin;		/* time from sync to picture	*/
	__u32 lower_margin;
	__u32 hsync_len;		/* length of horizontal sync	*/
	__u32 vsync_len;		/* length of vertical sync	*/
	__u32 sync;			/* see FB_SYNC_*		*/
	__u32 vmode;			/* see FB_VMODE_*		*/
	__u32 rotate;			/* angle we rotate counter clockwise */
	__u32 reserved[5];		/* Reserved for future compatibility */
};
	  struct fb_var_screeninfo var = {.xres = 256,
	                                       .yres = 192,
	                                       .xres_virtual = 256,
	                                       .yres_virtual = 192,
	                                       .xoffset = 0,
	                                       .yoffset = 0,
	                                       .bits_per_pixel = 16,
	                                       .grayscale = 0,
	                                       
	                                       .red.offset = 10,
	                                       .red.length = 5,
	                                       .red.msb_right = 0,
	                                       .green.offset = 5,
	                                       .green.length = 5,
	                                       .green.msb_right = 0,
	                                       .blue.offset = 0,
	                                       .blue.length = 5,
	                                       .blue.msb_right = 0,	
	                                       .transp.offset = 15,
	                                       .transp.length = 0,
	                                       .transp.msb_right = 0,

	                                       .nonstd = 0,
	                                       .activate = FB_ACTIVATE_NOW,

	                                       .height = 256,
	                                       .width = 192,
	                                       .accel_flags = 0,
	                                       
	                                       .pixclock = 0,			/* pixel clock in ps (pico seconds) */
	                                       .left_margin =0,		/* time from sync to picture	*/
	                                       .right_margin =0,		/* time from picture to sync	*/
	                                       .upper_margin =0,		/* time from sync to picture	*/
	                                       .lower_margin =0,
	                                       .hsync_len =0,		/* length of horizontal sync	*/
	                                       .vsync_len =0,		/* length of vertical sync	*/
	                                       .sync =0,			/* see FB_SYNC_*		*/
	                                       .vmode =0,			/* see FB_VMODE_*		*/
	                                       .rotate =0,			/* angle we rotate counter clockwise */
	                                       .reserved = {[0 ... 4] = 0}
	                                      };
struct fb_fix_screeninfo {
	char id[16];			/* identification string eg "TT Builtin" */
	unsigned long smem_start;	/* Start of frame buffer mem */
					/* (physical address) */
	__u32 smem_len;			/* Length of frame buffer mem */
	__u32 type;			/* see FB_TYPE_*		*/
	__u32 type_aux;			/* Interleave for interleaved Planes */
	__u32 visual;			/* see FB_VISUAL_*		*/ 
	__u16 xpanstep;			/* zero if no hardware panning  */
	__u16 ypanstep;			/* zero if no hardware panning  */
	__u16 ywrapstep;		/* zero if no hardware ywrap    */
	__u32 line_length;		/* length of a line in bytes    */
	unsigned long mmio_start;	/* Start of Memory Mapped I/O   */
					/* (physical address) */
	__u32 mmio_len;			/* Length of Memory Mapped I/O  */
	__u32 accel;			/* Indicate to driver which	*/
					/*  specific chip/card we have	*/
	__u16 reserved[3];		/* Reserved for future compatibility */
};

	  struct fb_fix_screeninfo fix = {.id = {"FB Builtin"},
	                                       .smem_start = kmalloc_ptr_uncache&0x1fffffff,
	                                       .smem_len = LEN,
	                                       .type = FB_TYPE_PACKED_PIXELS,
	                                       .type_aux = 0,
	                                       .visual = FB_VISUAL_TRUECOLOR,
	                                       
	                                       .xpanstep = 0,
	                                       .ypanstep = 0,
	                                       .ywrapstep = 0,
	                                       
	                                       .line_length = 256*2,
	                                       .mmio_start  = 0,
	                                       .mmio_len = 0,
	                                       .accel    = 0,
	                                       .reserved = {[0 ... 2] = 0}
	                                      };
	//static struct fb_con2fbmap con2fb ={0,0};
	//static struct fb_cmap_user cmap = {0,0,0,0,0,0};
	//static struct fb_event event;
	void __user *argp = (void __user *)arg;
	//int i;
	

	switch (cmd) {
	case FBIOGET_VSCREENINFO:
		return copy_to_user(argp, &var,
				    sizeof(var)) ? -EFAULT : 0;
	case FBIOPUT_VSCREENINFO:
	        ;
	        //struct fb_var_screeninfo var_tmp;
		if (copy_from_user(&var_tmp, argp, sizeof(var)))
			return -EFAULT;

		if (copy_to_user(argp, &var, sizeof(var)))
			return -EFAULT;
		return 0;
	case FBIOGET_FSCREENINFO:
		return copy_to_user(argp, &fix,
				    sizeof(fix)) ? -EFAULT : 0;
	case FBIOPUTCMAP:

	case FBIOGETCMAP:

	case FBIOPAN_DISPLAY:

	case FBIO_CURSOR:

	case FBIOGET_CON2FBMAP:

	case FBIOPUT_CON2FBMAP:
		
	case FBIOBLANK:

	default:

	    return -EINVAL;
	}

}

static int __MP4_ioctl_dsp(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg)
{
    int ret = -1;
         int ticks;// = (int)(jiffies - nds_updat_tim);
         int time_ms;// = jiffies_to_msecs(ticks);

    switch(cmd){
    case SNDCTL_DSP_GETODELAY:
         ;
         ticks = (int)(jiffies - nds_updat_tim);
         time_ms = jiffies_to_msecs(ticks);
         return put_user(time_ms, (int *)arg); 
    default :
         return -EINVAL;
    }

    return ret;
}

#endif

#define  MP4_KEYBD_WAIT 0x7755
static int __MP4_ioctl_keybd(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg)
{
    unsigned char err_tmp;
    if(cmd == MP4_KEYBD_WAIT){
#ifdef HTW_NDS_MP4
        os_SemaphorePend(MP4_devices->sem4key,0, &err_tmp);
#endif
        
	char* keyp =( char*)(kmalloc_ptr_uncache + pmain_buf->key_buf_offset + ((pmain_buf->key_read_num & KEY_MASK) * sizeof(struct key_buf)));
        pmain_buf->key_read_num++;
        copy_to_user((void*)arg, (void*)keyp, sizeof(struct key_buf));
        return 0;
    }
    return -1;
}

 int MP4_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg)
{
    int ret = -1;
#ifdef HTW_NDS_MP4
    ret = __MP4_ioctl_keybd(inode, filp, cmd, arg);
    /*
    if(ret){
        ret = __MP4_ioctl_fb(inode, filp, cmd, arg);
    }
    if(ret){
        ret = __MP4_ioctl_dsp(inode, filp, cmd, arg);
    }    
    */    
#endif

    if(ret){
    unsigned char tmp;
#ifdef HTW_NDS_MP4
    os_SemaphorePend(MP4_devices->mutex, 0, &tmp);
    if(tmp){
        dgprintf("fail in get MP4_devices->mutex");
        ret = tmp;
        goto out;
    }    
#endif    
        ret = __do_MP4_ioctl(inode, filp, cmd, arg);
#ifdef HTW_NDS_MP4
    os_SemaphorePost(MP4_devices->mutex);
#endif        
    }
out:
    return ret;
}


static loff_t MP4_llseek(struct file *filp, loff_t off, int whence)
{
	return 0;
}


static void gpio_init(void)
{
    //__gpio_as_static_16bit();//cs1, 16-bit data, 4-bit adddress //to FPGA port
    GPIO_ADDR_RECOVER();

#if enable_initfpga
	Disenable_game_cmd_interrup();
#else
	Enable_game_cmd_interrup();
#endif
	__gpio_disable_pull(fpga_cmd_port + nds_cmd_iqe_pinx);         //disable pull

	Disenable_game_data_interrup();
	__gpio_disable_pull(fpga_data_port + nds_data_iqe_pinx);         //disable pull

    INITFPGAPORTTIME();

    //*(fpgaport*)cpld_ctr_addr = BIT(key2_init_enable_bit) ;
    //*(fpgaport*)cpld_x0_addr=0x6bf3;
    //*(fpgaport*)cpld_x1_addr=0xf0c2;
    //*(fpgaport*)cpld_x4_addr=0x9252;
    //*(fpgaport*)cpld_ctr_addr =0;

    *(fpgaport*)cpld_ctr_addr = BIT(key2_init_enable_bit);

    SET_ADDR_GROUP(GPIO_ADDR_GROUP2);

    *(fpgaport*)cpld_base_addr=0x6bf3;
    *(fpgaport*)(cpld_base_addr+cpld_base_step*1)=0xf0c2;
    *(fpgaport*)(cpld_base_addr+cpld_base_step*2)=0x9252;
    SET_ADDR_DEFT();
    *(fpgaport*)cpld_ctr_addr =0;

	udelay(1);
}

unsigned int audio_samples_per_trans = 1024;
unsigned int audio_samples_frequence = 44100;

static int init_nds_var(void)

{
	
	u32 *temp32p;
    int offset;

	//int i;
	u32 getbuf;
	int offsetindex;

	struct video_set *video_setp;

	struct audio_set *audio_setp;



    getbuf= pmain_buf->buf_st_list[buf_c0_set].offset + kmalloc_ptr_uncache;


	temp32p = (u32*)getbuf;


    memset(temp32p,0,512);

    temp32p[0]=0xc0;


	offset = 0x60;

    offsetindex=0x4;


    temp32p[offsetindex/4]=IS_SET_ENABLE_VIDEO;offsetindex+=4;

	temp32p[offsetindex/4]=offset;offsetindex+=4;

	temp32p[offset/4]=0;

    offset+=4;


	temp32p[offsetindex/4]=IS_SET_ENABLE_AUDIO;offsetindex+=4;

    temp32p[offsetindex/4]=offset;offsetindex+=4;

    temp32p[offset/4]=0;

    offset+=4;


    temp32p[offsetindex/4]=IS_SET_ENABLE_KEY;offsetindex+=4;

    temp32p[offsetindex/4]=offset;offsetindex+=4;

	temp32p[offset/4]=0;

	offset+=4;


    temp32p[offsetindex/4]=IS_SET_ENABLE_RTC;offsetindex+=4;

	temp32p[offsetindex/4]=offset;offsetindex+=4;
    temp32p[offset/4]=0;

	offset+=4;
	temp32p[offsetindex/4]=IS_SET_CLEAR_VAR;offsetindex+=4;

	temp32p[offsetindex/4]=offset;offsetindex+=4;

    temp32p[offset/4]=1;

    offset+=4;


    temp32p[offsetindex/4]=IS_SET_AUDIO;offsetindex+=4;

    temp32p[offsetindex/4]=offset;offsetindex+=4;//sizeof(audio_set);

	audio_setp =(struct audio_set *)&temp32p[offset/4];

    audio_setp->sample=audio_samples_frequence;//22050;//44100;

    audio_setp->timer_l_data=32768;

	audio_setp->timer_l_ctr=0;

	//audio_setp->timer_h_data= 1024;//4096;
	audio_setp->timer_h_data= audio_samples_per_trans;

	audio_setp->timer_h_ctr=0;

    //audio_setp->sample_size= 1024;//4096;
	audio_setp->sample_size= audio_samples_per_trans;

    audio_setp->sample_bit=16;

    audio_setp->data_type=0;

    audio_setp->stereo=1;

    offset+=sizeof(struct audio_set);


	temp32p[offsetindex/4]=IS_SET_VIDEO;offsetindex+=4;

    temp32p[offsetindex/4]=offset;offsetindex+=4;//sizeof(audio_set);

	video_setp =(struct video_set *)&temp32p[offset/4];

	video_setp->frequence=25;

    video_setp->timer_l_data=32768;

    video_setp->timer_l_ctr=0;

    video_setp->timer_h_data=32768/25;

    video_setp->timer_h_ctr=0;

    video_setp->width=256;

	video_setp->height=192;

    video_setp->data_type =0;

    video_setp->play_buf=0;

	video_setp->swap=0;

    offset+=sizeof(struct video_set);

	mdelay(400);

	__do_MP4_ioctl(0, 0, IQE_UPDATE, buf_c0_set);
    while(nds_cmd_iqe_pin != 0);//Wait NDS response over

    cmd_line_interrupt(0x5a5a5a5a, 0);
    while(nds_cmd_iqe_pin == 0);

#if 1
	while(nds_cmd_iqe_pin != 0);//c0
	cmd_line_interrupt(0x5a5a5a5a, 0);
#endif	
	Enable_game_cmd_interrup();

	mdelay(2);
	while (pmain_buf->buf_st_list[buf_c0_set].isused == 1);

	__do_MP4_ioctl(0, 0,ENABLE_FIXRGB_UPVIDEO,0);

	__do_MP4_ioctl(0, 0,ENABLE_FIXRGB_DOWNVIDEO,0);

    return 0;
}


static int init_fpga(void)
{
    *(fpgaport*)cpld_ctr_addr = BIT(fifo_clear_bit) | (1<<fpga_mode_bit);
    udelay(200);
    *(fpgaport*)cpld_ctr_addr = 0 | (1<<fpga_mode_bit);

//	audio_samples_per_trans = 1024;
	//audio transfer lenght
    pmain_buf->buf_st_list[buf_audio_0].use_len = audio_samples_per_trans *4;
    pmain_buf->buf_st_list[buf_audio_1].use_len = audio_samples_per_trans *4;

    init_nds_var();
	return 0;
}

 int MP4_init_module(void)
{
	int result;
    u32  virt_addr;
    struct buf_st buf_st_temp;
    int i;

    gpio_init();

#ifdef HTW_NDS_MP4
	MP4_devices = (struct MP4_dev*)malloc(sizeof(struct MP4_dev));
	if (!MP4_devices)
	{
		result = -1;
		goto fail;
	}
	memset(MP4_devices, 0, sizeof(struct MP4_dev));
#endif

	if( request_irq( NDS_CMD_IRQ,cmd_line_interrupt, NULL ) < 0)
	{
		dgprintf(  "[FALLED: Cannot register cmd_line_interrupt!]\n" );
		goto nds_cmd_irq_failed;
	}

	if( request_irq( NDS_DATA_IRQ,data_line_interrupt, NULL ) < 0)
	{
		dgprintf(  "[FALLED: Cannot register data_line_interrupt!]\n" );
		goto nds_data_irq_failed;
	}

	//dma_chan = jz_request_dma(DMA_ID_AUTO, "auto", jz4740_dma_irq1,
	dma_chan = DMA_ID_AUTO;
	dma_request(DMA_ID_AUTO, NULL, NULL, NULL, DMAC_DRSR_RS_AUTO);
	if (dma_chan < 0) {
		dgprintf("Setup irq failed\n");
		return -1;
	}
    //printf("dma_chan=%x\n",dma_chan);

//    kmalloc_ptr = (unsigned int)malloc(LEN+32);
    kmalloc_ptr = (unsigned int)Drv_alloc(LEN+32);

    if (kmalloc_ptr == 0)
    {
        dgprintf(" can't allocate required DMA(OUT) buffers.\n");
        return -1;
    }
    kmalloc_ptr += (32 -1);
    kmalloc_ptr &= ~(32 -1);
 
#if enable_cache
    kmalloc_ptr_uncache= kmalloc_ptr;
#else
    kmalloc_ptr_uncache=(u32)((((unsigned int)virt_to_phys((char*)kmalloc_ptr)) & 0x1fffffff) | 0xa0000000);
#endif

    pmain_buf=(struct main_buf*)kmalloc_ptr_uncache;
    memset((char*)kmalloc_ptr,0,0x6000);
    pmain_buf->key_buf_offset = 0x4000;
    pmain_buf->key_buf_len = 0x2000;
    pmain_buf->key_write_num = 0;
    pmain_buf->key_read_num = 0;

    pnds_sys_rtc = &pmain_buf->nds_rtc;
    buf_st_temp.isused=0;
    buf_st_temp.offset=pmain_buf->key_buf_offset + pmain_buf->key_buf_len;
    buf_st_temp.len=256*192*2;
    buf_st_temp.use_len=256*192*2;
    buf_st_temp.nds_max_len=256*192*2;
    buf_st_temp.nds_cmd=(((1 <<enable_fix_video_bit ) |(1 <<enable_fix_video_rgb_bit ))<<24) | (buf_video_up_0<<16) | (VIDEO_UP <<8) | 0xc1;
    buf_st_temp.type= 0;
    pmain_buf->buf_st_list[buf_video_up_0] = buf_st_temp;

    buf_st_temp.offset=buf_st_temp.offset + buf_st_temp.len;
    buf_st_temp.nds_cmd=(((1 <<enable_fix_video_bit ) |(1 <<enable_fix_video_rgb_bit ))<<24) |  (buf_video_up_1<<16) | (VIDEO_UP <<8) | 0xc1;
    pmain_buf->buf_st_list[buf_video_up_1] = buf_st_temp;

    buf_st_temp.offset=buf_st_temp.offset + buf_st_temp.len;
    buf_st_temp.nds_cmd= (((1 <<enable_fix_video_bit ) |(1 <<enable_fix_video_rgb_bit ))<<24) | (buf_video_down_0<<16) | (VIDEO_DOWN <<8) | 0xc1;
    pmain_buf->buf_st_list[buf_video_down_0] = buf_st_temp;

    buf_st_temp.offset=buf_st_temp.offset + buf_st_temp.len;
    buf_st_temp.nds_cmd= (((1 <<enable_fix_video_bit ) |(1 <<enable_fix_video_rgb_bit ))<<24) | (buf_video_down_1<<16) | (VIDEO_DOWN <<8) | 0xc1;
    pmain_buf->buf_st_list[buf_video_down_1] = buf_st_temp;

    buf_st_temp.offset=buf_st_temp.offset + buf_st_temp.len;
    buf_st_temp.len=48000 *4;
    buf_st_temp.use_len= 4096 * 4 ;
    buf_st_temp.nds_max_len=65536;
    buf_st_temp.nds_cmd= (buf_audio_0<<16) | (AUDIO_G<<8) | 0xc2;
    pmain_buf->buf_st_list[buf_audio_0] = buf_st_temp;

    buf_st_temp.offset=buf_st_temp.offset + buf_st_temp.len;
    buf_st_temp.nds_cmd= (buf_audio_1<<16) | (AUDIO_G<<8) | 0xc2;
    pmain_buf->buf_st_list[buf_audio_1] = buf_st_temp;

    buf_st_temp.offset=buf_st_temp.offset + buf_st_temp.len;
    buf_st_temp.len=512;
    buf_st_temp.use_len=512;
    buf_st_temp.nds_max_len=512;
    buf_st_temp.nds_cmd= (buf_c0_set<<16)|0xc0;
    pmain_buf->buf_st_list[buf_c0_set] = buf_st_temp;

    pmain_buf->tempbuff = buf_st_temp.offset + buf_st_temp.len;
    pmain_buf->tempbuff_len =LEN - pmain_buf->tempbuff;

    buf_st_temp.isused=0;
    buf_st_temp.offset=pmain_buf->tempbuff;
    buf_st_temp.len=pmain_buf->tempbuff_len;
    buf_st_temp.use_len=pmain_buf->tempbuff_len;
    buf_st_temp.nds_max_len=pmain_buf->tempbuff_len;
    buf_st_temp.nds_cmd=0;
    buf_st_temp.type= (buf_max_num<<16);
    pmain_buf->buf_st_list[buf_max_num] = buf_st_temp;

    pmain_buf->nds_video_up_w=0;
    pmain_buf->nds_video_up_r=0;
    pmain_buf->nds_video_down_w=0;
    pmain_buf->nds_video_down_r=0;
    pmain_buf->nds_audio_w=0;
    pmain_buf->nds_audio_r=0;

    for (i=0;i< 16;i++ )
    {
        nds_iqe_list[i].num=0;
#if enable_cache
        nds_iqe_list[i].c0_addr = (u32)&pmain_buf->nds_iqe_list_c0[i][0];
#else
        nds_iqe_list[i].c0_addr=(u32)((((unsigned int)virt_to_phys((char*)&pmain_buf->nds_iqe_list_c0[i][0])) & 0x1fffffff) | 0xa0000000);
#endif
    }

    cpu_iqe_num=0;
    nds_iqe_list_w=0;
    nds_iqe_list_r=0;
	_nds_iqe_list_w_nest = 0;

    mm_check=0;

    //unsigned char err_tmp;

#ifdef HTW_NDS_MP4
    MP4_devices->mutex = os_SemaphoreCreate(1);
    MP4_devices->sem4key = os_SemaphoreCreate(0);
#endif

#if 0

    dgprintf("pmain_buf[0] =%x,%x\n",pmain_buf->buf_st_list[0].offset,pmain_buf->buf_st_list[0].len);
    dgprintf("pmain_buf[1] =%x,%x\n",pmain_buf->buf_st_list[1].offset,pmain_buf->buf_st_list[1].len);
    dgprintf("pmain_buf[2] =%x,%x\n",pmain_buf->buf_st_list[2].offset,pmain_buf->buf_st_list[2].len);
    dgprintf("pmain_buf[3] =%x,%x\n",pmain_buf->buf_st_list[3].offset,pmain_buf->buf_st_list[3].len);
    dgprintf("pmain_buf[4] =%x,%x\n",pmain_buf->buf_st_list[4].offset,pmain_buf->buf_st_list[4].len);
    dgprintf("pmain_buf[5] =%x,%x\n",pmain_buf->buf_st_list[5].offset,pmain_buf->buf_st_list[5].len);
    dgprintf("pmain_buf[6] =%x,%x\n",pmain_buf->buf_st_list[6].offset,pmain_buf->buf_st_list[6].len);
    dgprintf("nds_video_up =%x,%x\n",pmain_buf->nds_video_up_w,pmain_buf->nds_video_up_r);
    dgprintf("nds_video_down =%x,%x\n",pmain_buf->nds_video_down_w,pmain_buf->nds_video_down_r);
    dgprintf("nds_audio =%x,%x\n",pmain_buf->nds_audio_w,pmain_buf->nds_audio_r);

#endif
	
#if enable_initfpga
    init_fpga();
#endif

	//Initialize escape key function, for console
	_escape_key_function = NULL;
	_escape_key = -1;

    dgprintf("mp4 ok\n");
	return 0;

nds_data_irq_failed:
    free_irq( NDS_CMD_IRQ );

nds_cmd_irq_failed:
    return -1;

fail:
	dgprintf(" fail in request Semaphore Create");
	return result;
}


