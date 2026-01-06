#include "include.h"
#include "prod_test.h"

void prod_test_init(void)
{
#if FUNC_IODM_EN
    prod_test_iodm_init();
#endif //FUNC_IODM_EN
}

AT(.text.app.proc.test)
void prod_test_process(void)
{
#if FUNC_IODM_EN
    prod_test_iodm_process();
#endif //FUNC_IODM_EN
}

