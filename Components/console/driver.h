#ifndef CONSOLE_DRIVER_H_
#define CONSOLE_DRIVER_H_
#include <console/define.h>
#include <core/event.h>
#include <string.h>

COMPONENT(console, Driver)
    M_EVENT(send);
	M_EVENT(receive, uint8_t)
public:
	void init();
    uint16_t getMinAvail(){return txMinAvail_;}
    uint8_t * txQueue(){return &txBufferDMA_[0];}
	bool sendPacket(uint16_t type, uint8_t length, const uint8_t* data);
	void errorRx();
	void errorDma();

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
	inline void transferDMA_()
	{
		uint16_t len = txIndex_ - txFirst_;
    	memcpy(txBufferDMA_, txBuffer_, len);
        LL_DMA_SetDataLength(DMA_MODULE, DMA_CHANNEL, len);
        LL_DMA_EnableChannel(DMA_MODULE, DMA_CHANNEL);
        txIndex_ = txFirst_;
	}

private:
	uint16_t rxType_;
	uint16_t txMinAvail_= TX_BUF_SIZE;

    uint8_t rxBuffer_[MAX_PACKET_LENGTH];
    uint8_t rxLength_, checksum_;
    uint8_t rxIndex_;

    uint8_t txBuffer_[TX_BUF_SIZE];
    uint8_t txBufferDMA_[TX_BUF_SIZE];
    uint8_t* txFirst_ = txBuffer_;
    uint8_t* txLast_ = txBuffer_ + TX_BUF_SIZE;
    uint8_t* txIndex_ = txFirst_;

    bool sending_ = false;
COMPONENT_END

#endif
