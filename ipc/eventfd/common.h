#ifndef __H_COMMON_

#define B_IO 0xAE
#define B_SET_CALLFD             _IO(B_IO,0x00)
#define B_SET_KICKFD             _IO(B_IO,0x01)
#define B_ACTIVE_EVENTFD         _IO(B_IO,   0x02)
#define B_WR_EVENTFD              _IO(B_IO,0x03)
#define B_SET_TUN_FD              _IO(B_IO,0x04)

#define C_IO 0xAE
#define C_SET_CALLFD        _IO(C_IO,   0x00)
#define C_SET_KICKFD        _IO(C_IO,   0x01)
#define C_WR_EVENTFD        _IO(C_IO,   0x02)
#define C_NOTIFY            _IO(C_IO,   0x03)

#define VHOST_WORK_QUEUED 1


#endif
