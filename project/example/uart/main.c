#include "include.h"
#include "uart_transfer.h"

int main(void)
{
    WDT_DIS();

    uart_transfer_init(115200);

    while (1) {
        uart_transfer_example();
    }

    return 0;
}
