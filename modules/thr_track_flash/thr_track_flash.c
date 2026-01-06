#include "include.h"
#include "thr_track_flash.h"

#ifndef THR_TRACK_FLASH_PART_LEN
#define THR_TRACK_FLASH_PART_LEN	  128    //128个样点一帧
#endif

#ifndef THR_TRACK_FLASH_BUF_LEN
#define THR_TRACK_FLASH_BUF_LEN	      6      //6帧输出一次鼓点判断值（(1/8000) * 128 * 6 = 96ms）
#endif

#ifndef THR_TRACK_FLASH_DUMP_EN
#define THR_TRACK_FLASH_DUMP_EN       0      //获取基础阈值（enr_nr_thr_base）
#endif

static thr_track_flash_cb_t thr_track_flash_cb AT(.thr_track_flash.small);
static s32 data_max_mean_s[THR_TRACK_FLASH_BUF_LEN] AT(.thr_track_flash.small);
static u8 buf_s[THR_TRACK_FLASH_BUF_LEN] AT(.thr_track_flash.small);

//用户需要修改enr_nr_thr、enr_nr_thr_mc、enr_nr_thr_base、enr_nr_thr_base2
//使能THR_TRACK_FLASH_DUMP_EN会自动打印实时的enr_nr_thr_base和enr_nr_thr_mc
AT(.text.thr_track_flash_small_init)
void thr_track_flash_init(void)
{
	thr_track_flash_cb.dump_en                  = THR_TRACK_FLASH_DUMP_EN;

	thr_track_flash_cb.part_len2                = (THR_TRACK_FLASH_PART_LEN << 1);
	thr_track_flash_cb.rand_len                 = THR_TRACK_FLASH_PART_LEN;
	thr_track_flash_cb.buf_len                  = THR_TRACK_FLASH_BUF_LEN;
	thr_track_flash_cb.data_max_mean_s          = data_max_mean_s;
	thr_track_flash_cb.buf_s                    = buf_s;

	thr_track_flash_cb.sensitity				= 50;
	thr_track_flash_cb.enr_mean_max_en			= 1;//1:mean   0:max
	thr_track_flash_cb.enr_mean_max_en2		    = 0;//1:mean   0:max
	thr_track_flash_cb.pre_en					= 0;

	//参数1
	thr_track_flash_cb.enr_nr_thr				= -31;    //dB 最大正噪声值，通常比安静噪声高9dB

	//参数2
	thr_track_flash_cb.enr_nr_thr_mc		    = -20;    //dB 最小负噪声值
	//thr_track_flash_cb.enr_nr_thr2				= -40;//dB

	//参数3
	thr_track_flash_cb.enr_nr_thr_base			= -38-2;  //dB 安静环境的底噪值(样机自身底噪)，通常取出现频率最多的值然后再减2dB

	//参数4
	thr_track_flash_cb.enr_nr_thr_base2		    = 32768/2;//dB Q15 鼓点阈值， 范围0~s32，32768表示1dB，32768/2表示0.5dB

	thr_track_flash_cb.enr_nr_ry				= 32768;//dB Q15->1dB

	thr_track_flash_cb.smooth_v				    = 26216; // 0.8f
	thr_track_flash_cb.smooth_max_v			    = 26216; // 0.8f
	thr_track_flash_cb.cal_cnt					= 25;//1 frame 8ms 50��-��25*8=0.2s
	thr_track_flash_cb.min_range				= 76;
	thr_track_flash_cb.enr_nr_ry_mcra		    = 32768;//dB Q15->3dB
	thr_track_flash_nr_init(&thr_track_flash_cb);
}

//void thr_track_flash_thr_set(s32 thr);  //设置鼓点阈值， 范围s32，32768代表1dB，32768/2代表0.5dB，32768 * 4代表4dB
