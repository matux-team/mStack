#include <console/controller.h>
#include <console/driver.h>
#include <console/hal.h>

void console::Driver::init()
{
    console::HAL::init();
    SM_START(ReceiveHeader);
}

STATE_BODY(console::Driver::ReceiveHeader)
{
    if (data_ == HEADER_INDICATOR) SM_SWITCH(Driver::ReceiveLength);
}

STATE_BODY(console::Driver::ReceiveLength)
{
    rxLength_ = data_;
    if (rxLength_ < MAX_PACKET_LENGTH)
    {
        rxIndex_ = 2;
        rxType_ = 0;
        checksum_ = HEADER_INDICATOR + rxLength_;
        SM_SWITCH(ReceiveType);
    }
    else SM_SWITCH(ReceiveHeader);
}

STATE_BODY(console::Driver::ReceiveType)
{
    checksum_ += data_;
    rxType_ <<= 8;
    rxType_+= data_;
    if (--rxIndex_ == 0)
    {
        if (rxLength_ > 0) SM_SWITCH(ReceiveData);
        else SM_SWITCH(ReceiveChecksum);
    }
}

STATE_BODY(console::Driver::ReceiveData)
{
    checksum_ += data_;
    rxBuffer_[rxIndex_++] = data_;
    if (rxIndex_==rxLength_) SM_SWITCH(ReceiveChecksum);
}
STATE_BODY(console::Driver::ReceiveChecksum)
{
    if (data_ == checksum_) SM_SWITCH(ReceiveFooter);
    else SM_SWITCH(ReceiveHeader);
}

STATE_BODY(console::Driver::ReceiveFooter)
{
    if (data_ == FOOTER_INDICATOR)
    {
        Controller::instance().processCommand(rxType_, rxLength_, rxBuffer_);
    }
    SM_SWITCH(ReceiveHeader);
}

bool console::Driver::sendPacket(uint16_t type, uint8_t length, const uint8_t* data)
{
    uint8_t checksum = 0u;
    if (txQueue_.available() < length + 5) return false;
    txQueue_.push(HEADER_INDICATOR);
    checksum += (uint8_t) HEADER_INDICATOR;
    txQueue_.push(length);
    checksum += length;
    txQueue_.push((type >> 8) & 0xFF);
    checksum += (uint8_t) ((type >> 8) & 0xFF);
    txQueue_.push(type & 0xFF);
    checksum += (uint8_t) (type & 0xFF);

    for (int i =0;i < length;i++)
    {
        txQueue_.push(data[i]);
        checksum += data[i];
    }
    txQueue_.push(checksum);
    txQueue_.push(FOOTER_INDICATOR);

    if (!sending_)
    {
        sendEvent.post();
        sending_ = true;
    }
    return true;
}

M_EVENT_HANDLER(console::Driver,send)
{
    if (txQueue_.empty()){sending_ = false;return;}
    if (console::HAL::txReady()) console::HAL::write(txQueue_.pop());
    sendEvent.post();
}
