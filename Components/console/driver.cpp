#include <console/controller.h>
#include <console/driver.h>
#include <console/hal.h>

void console::Driver::init()
{
    console::_HAL::init();
}

void console::Driver::ReceiveHeader_(uint8_t data)
{
    if (data == HEADER_INDICATOR) rxState_ = &Driver::ReceiveLength_;
}

void console::Driver::ReceiveLength_(uint8_t data)
{
    rxLength_ = data;
    if (rxLength_ < MAX_PACKET_LENGTH)
    {
        rxIndex_ = 2;
        rxType_ = 0;
        checksum_ = HEADER_INDICATOR + rxLength_;
        rxState_ = &Driver::ReceiveType_;
    }
    else rxState_ = &Driver::ReceiveHeader_;
}

void console::Driver::ReceiveType_(uint8_t data)
{
    checksum_ += data;
    rxType_ <<= 8;
    rxType_+= data;
    if (--rxIndex_ == 0)
    {
        if (rxLength_ > 0) rxState_ = &Driver::ReceiveData_;
        else rxState_ = &Driver::ReceiveChecksum_;
    }
}

void console::Driver::ReceiveData_(uint8_t data)
{
    checksum_ += data;
    rxBuffer_[rxIndex_++] = data;
    if (rxIndex_==rxLength_) rxState_ = &Driver::ReceiveChecksum_;
}
void console::Driver::ReceiveChecksum_(uint8_t data)
{
    if (data == checksum_) rxState_ = &Driver::ReceiveFooter_;
    else rxState_ = &Driver::ReceiveHeader_;
}

void console::Driver::ReceiveFooter_(uint8_t data)
{
    if (data == FOOTER_INDICATOR)
    {
        Controller::instance().processCommand(rxType_, rxLength_, rxBuffer_);
    }
    rxState_ = &Driver::ReceiveHeader_;
}

bool console::Driver::sendPacket(uint16_t type, uint8_t length, const uint8_t* data)
{
    uint8_t checksum = 0u;
    uint16_t avail = txLast_ - txIndex_;
    if(avail < txMinAvail_) txMinAvail_ = avail;
    if (avail < length + 6)
	{
    	return false;
	}
    (*txIndex_) = HEADER_INDICATOR; txIndex_++;
    checksum += (uint8_t) HEADER_INDICATOR;
    (*txIndex_) = length; txIndex_++;
    checksum += length;
    (*txIndex_) = (type >> 8) & 0xFF; txIndex_++;
    checksum += (uint8_t) ((type >> 8) & 0xFF);
    (*txIndex_) = type & 0xFF; txIndex_++;
    checksum += (uint8_t) (type & 0xFF);

    for (int i =0;i < length;i++)
    {
    	(*txIndex_) = data[i]; txIndex_++;
        checksum += data[i];
    }
    (*txIndex_) = checksum; txIndex_++;
    (*txIndex_) = FOOTER_INDICATOR; txIndex_++;

    if(!sending_)
    {
    	sending_ = true;
    	transferDMA_();
    }
    return true;
}

M_EVENT_HANDLER(console::Driver,send)
{
	if(txIndex_ == txFirst_){sending_ = false; return;}
	transferDMA_();
}

M_EVENT_HANDLER(console::Driver, receive, uint8_t)
{
	(this->*(rxState_))(event);
}
