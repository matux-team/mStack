#ifndef CONSOLE_DRIVER_H_
#define CONSOLE_DRIVER_H_
#include <core/event.h>
#include <console/define.h>

COMPONENT(console, Driver)
    M_EVENT(send);
	M_EVENT(receive, uint8_t)
public:
	void init();
	uint16_t getMinAvail(){return txMinAvail_;}
	bool sendPacket(uint16_t type, uint8_t length, const uint8_t* data);
    bool checkBeforeSend(uint8_t lenth){return (txQueue_.available()>lenth);}

private:
	typedef void (Driver::*RxState) (uint8_t);
	void ReceiveHeader_(uint8_t data);
	void ReceiveLength_(uint8_t data);
	void ReceiveType_(uint8_t data);
	void ReceiveData_(uint8_t data);
	void ReceiveChecksum_(uint8_t data);
	void ReceiveFooter_(uint8_t data);
	RxState rxState_ = &Driver::ReceiveHeader_;

private:
    QUEUE_DEF(txQueue, TX_BUF_SIZE);
    uint16_t txMinAvail_= TX_BUF_SIZE;
    uint16_t rxType_;
    uint8_t rxBuffer_[MAX_PACKET_LENGTH];
    uint8_t rxLength_, checksum_;
    uint8_t rxIndex_;
    bool sending_ = false;

COMPONENT_END

#endif
