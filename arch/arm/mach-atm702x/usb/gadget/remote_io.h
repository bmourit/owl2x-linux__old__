
#ifndef __REMOTE_IO_H__
#define __REMOTE_IO_H__

#ifdef MAX_PATH
#undef MAX_PATH
#endif
#define MAX_PATH                800	//1024
#define MAX_ROOT_PATH           256
#define REMOTE_FS_PC_MAX_PATH   MAX_PATH
#define REMOTE_FS_MAX_PATH      MAX_PATH

struct remote_io;

struct remote_io_ops {
    int (*open)(struct remote_io *io, const char *pathname, int flags, int mode);
    int (*close)(struct remote_io *io, int fd);
    ssize_t (*read)(struct remote_io *io, int fd, char __user *buf, size_t count);
    ssize_t (*write)(struct remote_io *io, int fd, const char __user *buf, size_t count);
    loff_t (*lseek)(struct remote_io *io, int fd, loff_t offset, int flag);
    int (*system)(struct remote_io *io, const char *command);
    int (*call_internal)(struct remote_io *io, const char *command);
    int (*send_usdk_info)(struct remote_io *io, const unsigned char *ptr, int length);
    long long (*tell)(struct remote_io *io, int fd);
    int (*rename)(struct remote_io *io, const char *oldpath, const char *newpath);
    int  (*remove)(struct remote_io *io, const char *pathname);
    int (*readdir)(struct remote_io *io, int fd, loff_t pos, void __user * ent , int len);
    int (*seekdir)(struct remote_io *io, int fd, loff_t offset);
    int (*rewinddir)(struct remote_io *io, int fd);
    int (*mkdir)(struct remote_io *io, const char *path, mode_t mode);
    int (*rmdir)(struct remote_io *io, const char *path);
    int (*fstat) (struct remote_io *io, int fd, void __user *buf );
    int (*stat) (struct remote_io *io, const char __user *path, void __user *buf);
    int (*statfs) (struct remote_io *io, const char __user *pathname, void __user * buf );
    int (*lastdir)(struct remote_io *io, int fd);
    int (*prevdir)(struct remote_io *io, int fd, loff_t pos, void __user *buf, int len);
    int (*reset2parentdir)(struct remote_io *io, int fd);
    int (*truncate)(struct remote_io *io, const char *pathname, long long len);
    int (*ftruncate)(struct remote_io *io, int fd, long long len);
       
};

struct remote_io {
    spinlock_t io_lock;
    char pc_root_path[MAX_ROOT_PATH];
    char pc_root_path_win_type[MAX_ROOT_PATH];
    u8   load_flag;
    u8   path_type;
    int  path_ready : 1;
    int  load_ready : 1;
    struct completion *complete;
    wait_queue_head_t remotefs_wait_queue;
    struct remote_io_ops ops;
    void  *buf_for_user;
    int remotefs_call_handle : 1;
    int remote_io_exit : 1;
    loff_t ret;
	struct module *owner;   
    unsigned long		private;
};

#define REMOTEFS_ENTRY_NAME_MAX     256

typedef struct remotefs_dirent
{
    /*! 文件序列号*/
    unsigned int d_ino;   
    /*! 目录头到该文件目录项的偏移量*/
    unsigned int  d_off; 
    /*! 文件修改时间 */
    unsigned int d_mtime;    
    /*! 该记录的总长度,包含记录头和文件名.32b对齐*/
    unsigned short int d_reclen;    
    /*! 文件类型属性，位定义如下：
     * \n ATTR_READONLY置位:只读文件
     * \n ATTR_HIDDEN置位:隐藏文件
     * \n ATTR_SYSTEM置位：系统文件
     * \n ATTR_VOLUME置位:卷标文件
     * \n ATTR_DIRECTORY置位:目录文件
     * \n ATTR_ARCHIVE置位:存档文件 */
    unsigned char  d_type;    
    /*! 文件名大小写标示位*/
    unsigned char  d_lcase;    
    /*! 文件名,可变长.长度为 = 记录长度(d_reclen) - 
        offsetof(struct dirent, d_name)
    */
    char  d_name[REMOTEFS_ENTRY_NAME_MAX];
}remotefs_dirent_t;

#if 0
struct remotefs_stat {
   dev_t     st_dev;     /* ID of device containing file */
   ino_t     st_ino;     /* inode number */
   mode_t    st_mode;    /* protection */
   nlink_t   st_nlink;   /* number of hard links */
   uid_t     st_uid;     /* user ID of owner */
   gid_t     st_gid;     /* group ID of owner */
   dev_t     st_rdev;    /* device ID (if special file) */
   off_t     st_size;    /* total size, in bytes */
   long      st_blksize; /* blocksize for file system I/O */
   long      st_blocks;  /* number of 512B blocks allocated */
   time_t    st_atime;   /* time of last access */
   time_t    st_mtime;   /* time of last modification */
   time_t    st_ctime;   /* time of last status change */
};
#endif

struct remotefs_stat {
    unsigned int    st_dev; /*! 文件所在的设备的编号*/
    unsigned int     st_ino; /*! 文件序列号*/
    unsigned int     st_mode; /*! 文件的类型和存储权限*/
    unsigned int     st_nlink; /*! 连接该文件的硬连接数目*/
    unsigned int     st_uid; /*! 文件所有者的用户识别码*/
    unsigned int     st_gid; /*! 文件所有者的组识别码*/
    unsigned int     st_rdev; /*! 如果是设备为设备号*/
    unsigned int     st_size; /*! 文件大小，单位byte*/
    unsigned long st_blksize; /*! 文件系统块I/O缓冲区大小*/
    unsigned long st_blocks; /*! 文件占用文件区块的个数，区大小为512byte*/
    time_t   st_atime; /*! 文件最近一次被读取或被执行的时间*/
    time_t   st_mtime; /*! 文件最后一次数据被更改的时间*/
    time_t   st_ctime; /*! 文件最后一次修改属性的时间*/
};

typedef struct remotefs_stat remotefs_stat_t;

/*!

 * \brief  

 *      文件系统状态结构定义

 */

typedef struct remotefs_statfs
{
         /*! 文件系统类型*/
         unsigned int    f_type;
         /*! 块大小或簇大小*/
         unsigned int    f_bsize;
         /*! 基本文件系统块大小*/
         unsigned int    f_frsize;
         /*! 总共含有多少个数据块，或含有多少个簇*/
         unsigned int    f_blocks;
         /*! 总共没有使用的数据块，或没有使用的簇*/
         unsigned int    f_bfree;
         /*! 空闲块可用于普通文件的个数*/
         unsigned int    f_bavail;
         /*! 文件序号总数*/
         unsigned int    f_files;
         /*! 空闲文件序号总数*/
         unsigned int    f_ffree;
         /*! 普通文件还可以使用的空文件*/
         unsigned int    f_favail;
         /*! 文件系统id*/
         unsigned int    f_fsid;
         /*! 文件系统flag*/
         unsigned int    f_flag;
         /*! 文件系统允许的文件名最大长度*/
         unsigned int    f_namemax;
}remotefs_statfs_t;


struct entry_inode_num {
    struct list_head list;
    int num;  
    char name[REMOTEFS_ENTRY_NAME_MAX];
};


struct remotefs_inode_info {
    struct list_head    file_list;
    struct list_head    entry_list;
    ino_t     st_ino;     /* inode number */
    mode_t    st_mode;    /* protection */
    nlink_t   st_nlink;   /* number of hard links */
    uid_t     st_uid;     /* user ID of owner */
    gid_t     st_gid;     /* group ID of owner */
    off_t     st_size;    /* total size, in bytes */
    u32        st_blksize; /* blocksize for file system I/O */
    u32         st_blocks;  /* number of 512B blocks allocated */
    time_t    st_atime;   /* time of last access */
    time_t    st_mtime;   /* time of last modification */
    time_t    st_ctime; 
    struct inode       vfs_inode;   
};

struct remotefs_sb_info {
	unsigned int rsize;
	unsigned int wsize;
	uid_t	mnt_uid;
	gid_t	mnt_gid;
	mode_t	mnt_file_mode;
	mode_t	mnt_dir_mode;
	struct super_block *sb;
};

#define REMOTEFS_ROOT_I     2

static inline struct remotefs_sb_info *
REMOTEFS_SB(struct super_block *sb)
{
	return sb->s_fs_info;
}

static inline struct remotefs_inode_info *
REMOTEFS_I(struct inode *inode)
{
	return container_of(inode, struct remotefs_inode_info, vfs_inode);
}


#endif

