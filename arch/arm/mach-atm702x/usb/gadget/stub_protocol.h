#ifndef	__ATD_USB_STUB_PROTOCOL_H
#define	__ATD_USB_STUB_PROTOCOL_H


#define	USB_GDB_CMD_WRAP_LENTH	64
#define	USB_GDB_ACK_WRAP_LENTH  USB_GDB_CMD_WRAP_LENTH

#define GDB_ACK_OK                      0
#define	GDB_ACK_ERR_PARSE_INVALID		1
#define	GDB_ACK_ERR_COMD_NOSUPP		2
#define GDB_ACK_ERR_COMD_LENGTH			3
#define GDB_ACK_ERR_COMD_CHECKSUM		4
#define GDB_ACK_ERR_COMMUICATION_FAILURE		5
#define	GDB_ACK_ERR_INVALID_ADDRESS				6
#define GDB_NOT_SEND_STATUS     0x88

#define USB_BULK_CS_WRAP_LEN	13
#define USB_BULK_CS_SIG		0x53425355	/* Spells out 'USBS' */
#define USB_STATUS_PASS		0
#define USB_STATUS_FAIL		1
#define USB_STATUS_PHASE_ERROR	2

#define SYSCALL_ID_ERROR    0xFFFFFFFF
#define SYSCALL_ID_OPEN     0x1
#define SYSCALL_ID_READ     0x2
#define SYSCALL_ID_WRITE    0x3
#define SYSCALL_ID_CLOSE    0x4
#define SYSCALL_ID_LSEEK    0x5

#define SYSCALL_ID_TELL                 0x31           
#define SYSCALL_ID_STAT                 0x3a
#define SYSCALL_ID_FSTAT                0x39
#define SYSCALL_ID_STATFS               0x3b
#define SYSCALL_ID_READDIR              0x34
#define SYSCALL_ID_PREVDIR              0x3d
#define SYSCALL_ID_SEEKDIR              0x35
#define SYSCALL_ID_LASTDIR              0x3c
#define SYSCALL_ID_REWINDDIR            0x36
#define SYSCALL_ID_RESET2PARENTDIR      0x3e
#define SYSCALL_ID_MKDIR                0x37
#define SYSCALL_ID_RMDIR                0x38
#define SYSCALL_ID_REMOVE               0x33
#define SYSCALL_ID_RENAME               0x32
#define SYSCALL_ID_TRUNCATE             0x3f
#define SYSCALL_ID_FTRUNCATE            0x40

#define SYSCALL_ID_SYSTEM               0x10
#define SYSCALL_ID_USDKINFO             0x20

enum gdb_cmd_type {
	NO_DATA,
	BIN_DATA,
	ASCII_DATA,
};

struct gdb_cmd_wrap{
	char 	cmd[USB_GDB_CMD_WRAP_LENTH];
	unsigned int	length;
	enum gdb_cmd_type	type;
	unsigned char	status;
	struct pt_regs*	pt_regs;
};

struct gdb_ack_wrap{
	char 	ack[USB_GDB_ACK_WRAP_LENTH];
	unsigned int	length;
};

#endif


