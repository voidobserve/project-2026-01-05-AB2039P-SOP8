#ifndef _API_THR_TRACK_FLASH_H
#define _API_THR_TRACK_FLASH_H


/***********************************thr track flash***************************************/
typedef struct {
	bool dump_en;
	u16 part_len2;
	u16 rand_len;
	u16 buf_len;
	s32 *data_max_mean_s;
	u8 *buf_s;

	s16 sensitity;
	s16 min_range;
	s16 smooth_v;
	s16 smooth_max_v;
	u8  enr_mean_max_en;
	u8  enr_mean_max_en2;
	u8  pre_en;
	s32 enr_nr_thr;
	s32 enr_nr_thr_mc;
	s32 enr_nr_thr2;
	s32 enr_nr_thr_base;
	s32 enr_nr_thr_base2;

	s32 enr_nr_ry;
	u32 cal_cnt;
	s32 enr_nr_ry_mcra;
	//u16 pitch_filter_range;
} thr_track_flash_cb_t;

void thr_track_flash_thr_set(s32 thr);    //设置鼓点阈值， 范围s32，32768代表1dB，32768/2代表0.5dB，32768 * 4代表4dB
void thr_track_flash_nr_init(thr_track_flash_cb_t *thr_track_flash_cb);
int thr_track_flash_process(s16 *data);

#endif // _API_THR_TRACK_FLASH_H
