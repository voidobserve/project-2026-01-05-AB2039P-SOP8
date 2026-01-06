#ifndef _SARADC_SAMPLING_H
#define _SARADC_SAMPLING_H

#define SARADC_GENERAL_EN        1  //general channel
#define SARADC_MIC_EN            1  //mic channel

void saradc_sampling_init(void);
void saradc_sampling_run(void);

#endif // _SARADC_SAMPLING_H
