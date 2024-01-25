#ifndef CONSOLE_DRIVER_H_
#define CONSOLE_DRIVER_H_
#include <core/machine.h>
#include <console/define.h>

MACHINE(console, Driver, uint8_t)
    M_EVENT(send);
public:
	void init();
	bool sendPacket(uint16_t type, uint8_t length, const uint8_t* data);
    bool checkBeforeSend(uint8_t lenth){return (txQueue_.available()>lenth);}

private:
	STATE_DEF(ReceiveHeader)
	STATE_DEF(ReceiveLength)
	STATE_DEF(ReceiveType)
	STATE_DEF(ReceiveData)
	STATE_DEF(ReceiveChecksum)
	STATE_DEF(ReceiveFooter)

private:
    QUEUE_DEF(txQueue, TX_BUF_SIZE);
    bool sending_ = false;
    uint8_t rxBuffer_[MAX_PACKET_LENGTH];
    uint8_t rxLength_, checksum_;
    uint8_t rxIndex_;
    uint16_t rxType_;
MACHINE_END

#endif
