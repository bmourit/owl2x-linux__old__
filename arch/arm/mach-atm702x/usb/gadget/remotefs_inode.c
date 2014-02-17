
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/blkdev.h>
#include <linux/slab.h>
#include <linux/buffer_head.h>
#include <linux/vfs.h>
#include <linux/mutex.h>
#include <linux/delay.h>

#include <asm/uaccess.h>

#include "remote_io.h"

//#define REMOTEFS_DEBUG_ON

#ifdef REMOTEFS_DEBUG_ON
#define REMOTEFS_DEBUG(fmt, args...)  printk("[remote fs] " fmt, ## args)
#else
#define REMOTEFS_DEBUG(fmt, args...)  do {} while (0)
#endif

#define REMOTEFS_ERR(fmt, args...)  printk(KERN_ERR "[remote fs] " fmt, ## args)

struct dirent {
   ino_t          d_ino;       /* inode number */
   off_t          d_off;       /* offset to the next dirent */
   unsigned short d_reclen;    /* length of this record */
   unsigned char  d_type;      /* type of file; not supported
                                  by all file system types */
   char           d_name[256]; /* filename */
};

extern struct remote_io * get_remote_io(void);
extern int remote_io_ready(void);

extern long __remote_io;

static DEFINE_MUTEX(mutex);

static const struct inode_operations remotefs_dir_inode_operations;
static const struct file_operations remotefs_directory_operations;
const struct address_space_operations remotefs_addr_ops;
static const struct inode_operations remotefs_file_inode_operations;
static const struct file_operations remotefs_file_operations;

static struct dentry_operations remotefs_dentry_ops;

static struct remote_io *io = NULL;
static struct kmem_cache *remotefs_inode_cashep;

struct remote_priv {
    int fd;
};

static inline int bits_count(u32 num)
{
	int bit_count = 0;
	u32 tmp = num - 1;
	
	if (num == 0) return 0;
	
	while (tmp & 0x1) {
		bit_count++;
		tmp >>= 1;	
	}	
	
	return bit_count;
}

static char *build_path_from_dentry(struct dentry *direntry)
{
	struct dentry *temp;
	int namelen;
	char *full_path;

	if (direntry == NULL)
		return NULL;

	namelen = 0;
	for (temp = direntry; !IS_ROOT(temp);) {
		namelen += (1 + temp->d_name.len);
		temp = temp->d_parent;
		if (temp == NULL) {
			REMOTEFS_ERR("tmep = null\n");
			return NULL;
		}
	}

	full_path = kmalloc(namelen+1, GFP_KERNEL);
	if (full_path == NULL)
		return full_path;
	full_path[namelen] = 0;
	for (temp = direntry; !IS_ROOT(temp);) {
		namelen -= 1 + temp->d_name.len;
		if (namelen < 0) {
			break;
		} else {
			full_path[namelen] = '/';
			strncpy(full_path + namelen + 1, temp->d_name.name,
				temp->d_name.len);
		}
		temp = temp->d_parent;
		if (temp == NULL) {
			REMOTEFS_ERR("corrupt dentry");
			kfree(full_path);
			return NULL;
		}
	}

    REMOTEFS_DEBUG("path : %s, %s, %d\n", full_path, __func__, __LINE__);
	return full_path;
}

#if 0
static int remotefs_subdir(const char *path, ulong *size_out)
{
	int ret;
    int fd;
    loff_t offset;
    struct dirent ent;
    int count;
    
    fd = io->ops.open(io, "", O_RDONLY | O_DIRECTORY, 0);
    if (fd < 0) {
        REMOTEFS_DEBUG("open failed , %s, %d\n", __func__, __LINE__);
        return fd;
    }
   
    ret = -1;
    offset = 0;
	count = 0;
	while (ret != 0) {
        ret = io->ops.readdir(io, fd, offset, &ent, sizeof(struct dirent));
        if (ret < 0) {
            count = -EIO;
            goto exit;
        }
        else if (ret >= 1) {
            offset = ent.d_off;
            count++;
        }
        else {
            break;   
        }
    }

exit:    
    *size_out = offset;
    io->ops.close(io, fd);
    
    return count;
}
#endif

static int find_ino(struct inode *dir, const char *name)
{
    struct remotefs_inode_info *inode_info = REMOTEFS_I(dir);
    struct entry_inode_num *entry;
    struct list_head *head = &inode_info->entry_list;
    
    list_for_each_entry(entry, head, list) {
        if (strncmp(name, entry->name, REMOTEFS_ENTRY_NAME_MAX))  
            continue;
        else 
            return entry->num; 
    }
    
    return 0;
}

static void remotefs_init_once(void * inode)
{
    struct remotefs_inode_info *remotefs_inode = inode;

	inode_init_once(&remotefs_inode->vfs_inode);
	INIT_LIST_HEAD(&remotefs_inode->file_list);
	INIT_LIST_HEAD(&remotefs_inode->entry_list);     
}

static void remotefs_renew_times(struct dentry * dentry)
{
	dget(dentry);
	spin_lock(&dentry->d_lock);
	for (;;) {
		struct dentry *parent;

		dentry->d_time = jiffies;
		if (IS_ROOT(dentry))
			break;
		parent = dentry->d_parent;
		dget(parent);
		spin_unlock(&dentry->d_lock);
		dput(dentry);
		dentry = parent;
		spin_lock(&dentry->d_lock);
	}
	spin_unlock(&dentry->d_lock);
	dput(dentry);
}

struct inode *remotefs_iget(struct super_block *sb, struct remotefs_inode_info *inode_info)
{
	struct remotefs_sb_info *sb_info;
	struct remotefs_inode_info *our_inode;
	struct inode *inode;
    
	inode = iget_locked(sb, inode_info->st_ino);
	if (!inode) {
	    unlock_super(sb);
		return ERR_PTR(-ENOMEM);
	}
	if (!(inode->i_state & I_NEW)) {
	    unlock_super(sb);
	    REMOTEFS_DEBUG("inode not new, %s, %d\n", __func__, __LINE__);
		return inode;
    }

	sb_info = REMOTEFS_SB(inode->i_sb);
    our_inode = REMOTEFS_I(inode);
    
    our_inode->st_mode = inode_info->st_mode;
    our_inode->st_nlink = inode_info->st_nlink;
    our_inode->st_size = inode_info->st_size;
    our_inode->st_blocks = inode_info->st_blocks;
    our_inode->st_blksize = inode_info->st_blksize;
    if (inode_info->st_blksize == 0) inode_info->st_blksize = sb->s_blocksize;
    
    inode->i_mode |=  inode_info->st_mode;
	//inode->i_nlink = inode_info->st_nlink;
	inode->__i_nlink = inode_info->st_nlink;
	i_size_write(inode, inode_info->st_size);
	inode->i_uid = sb_info->mnt_uid;
	inode->i_gid = sb_info->mnt_gid;
	inode->i_blocks = inode_info->st_blocks;
	inode->i_blkbits = bits_count(inode_info->st_blksize);
	
	inode->i_mtime.tv_sec = inode->i_atime.tv_sec = inode->i_ctime.tv_sec = 0;
	inode->i_mtime.tv_nsec = inode->i_atime.tv_nsec = inode->i_ctime.tv_nsec = 0;
	
	REMOTEFS_DEBUG("inode->i_mode: %08X, %s, %d\n", inode->i_mode, __func__, __LINE__);
	if (S_ISDIR(inode->i_mode)) {
		inode->i_op = &remotefs_dir_inode_operations;
		inode->i_fop = &remotefs_directory_operations;
	} else {
		inode->i_fop = &remotefs_file_operations;
		inode->i_op = &remotefs_file_inode_operations;
		inode->i_data.a_ops = &remotefs_addr_ops;
	}
    
    unlock_new_inode(inode);
    
	return inode;
}

static loff_t remotefs_file_llseek (struct file *filp, loff_t offset, int origin)
{
    struct remote_priv *priv = (struct remote_priv *)filp->private_data;
    loff_t pos_ret;
    
    REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
    
    pos_ret = io->ops.lseek(io, priv->fd, offset, origin);
    if (pos_ret < 0) 
        return -EIO;
        
    filp->f_pos = pos_ret;
    
    return pos_ret;  
}

static ssize_t remotefs_file_read_kernel (struct file *filp, char * buf, size_t size, loff_t *pos)
{
    struct remote_priv *priv = (struct remote_priv *)filp->private_data;
    ssize_t ret;
    
    if (*pos < 0) {
    	REMOTEFS_ERR("error pos: %lld, %s, %d\n", *pos, __func__, __LINE__);
    	return -EINVAL;	
    }
    
//    REMOTEFS_DEBUG("filp->f_pos: %lld, , pos: %lld, %s, %d\n", filp->f_pos, *pos, __func__, __LINE__);
    mutex_lock(&mutex);
    if (filp->f_pos != *pos) {
        if (io->ops.lseek(io, priv->fd, *pos, SEEK_SET) < 0) {
            ret = -EIO;
            goto exit;   
        }
        filp->f_pos = *pos;
    }
    
    //REMOTEFS_DEBUG("size: %u, %s, %d\n", size, __func__, __LINE__); 
    
    ret = io->ops.read(io, priv->fd, buf, size);
    if (ret < 0) {
        ret = -EIO;
    }
    else {
    	*pos += ret;
    	filp->f_pos = *pos;
    }
exit:    
    mutex_unlock(&mutex);
    
    //REMOTEFS_DEBUG("ret: %d, %s, %d\n", ret, __func__, __LINE__); 
    
    return ret;   
}

static ssize_t remotefs_file_read (struct file *filp, char __user * buf, size_t size, loff_t *pos)
{
    struct remote_priv *priv = (struct remote_priv *)filp->private_data;
    ssize_t ret;
    
    if (*pos < 0) {
    	REMOTEFS_ERR("error pos: %lld, %s, %d\n", *pos, __func__, __LINE__);
    	return -EINVAL;	
    }
    
//    REMOTEFS_DEBUG("filp->f_pos: %lld, , pos: %lld, %s, %d\n", filp->f_pos, *pos, __func__, __LINE__);
    mutex_lock(&mutex);
    if (filp->f_pos != *pos) {
        if (io->ops.lseek(io, priv->fd, *pos, SEEK_SET) < 0) {
            ret = -EIO;
            goto exit;   
        }
        filp->f_pos = *pos;
    }
    
    //REMOTEFS_DEBUG("size: %u, %s, %d\n", size, __func__, __LINE__); 
    
    ret = io->ops.read(io, priv->fd, buf, size);
    if (ret < 0) {
        ret = -EIO;
    }
    else {
    	*pos += ret;
    	filp->f_pos = *pos;
    }
exit:    
    mutex_unlock(&mutex);
    
    //REMOTEFS_DEBUG("ret: %d, %s, %d\n", ret, __func__, __LINE__); 
    
    return ret;   
}

static ssize_t remotefs_file_write (struct file *filp, const char __user *buf, size_t size, loff_t *pos)
{
    struct remote_priv *priv = (struct remote_priv *)filp->private_data;
    ssize_t ret;
    
    if (*pos < 0) {
    	REMOTEFS_ERR("error pos: %lld, %s, %d\n", *pos, __func__, __LINE__);
    	return -EINVAL;	
    }
    
    //REMOTEFS_DEBUG("filp->f_pos: %lld, , pos: %lld, %s, %d\n", filp->f_pos, *pos, __func__, __LINE__);
    mutex_lock(&mutex);
    if (filp->f_pos != *pos) {
        if (io->ops.lseek(io, priv->fd, *pos, SEEK_SET) < 0) {
            ret = -EIO;
            goto exit;   
        }
        filp->f_pos = *pos;
        REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__); 
    }
    
    //REMOTEFS_DEBUG("size: %u, %s, %d\n", size, __func__, __LINE__);
    
    ret = io->ops.write(io, priv->fd, buf, size); 
    if (ret < 0) 
        ret = -EIO;
    else {
    	*pos += ret;
    	filp->f_pos = *pos;
    }
exit:        
    mutex_unlock(&mutex);
    return ret;   
}

static int remotefs_file_open(struct inode *inode, struct file *filp)
{
    int result = 0;
	struct dentry *dentry = filp->f_path.dentry;
	int mode = filp->f_mode;
	int flag = filp->f_flags;
	struct remotefs_stat stat_info;
	struct remote_priv *priv;
	char *path;
	int ret;
	
	REMOTEFS_DEBUG("mode: %08x, flag: %08x, %s, %d\n", mode, flag, __func__, __LINE__);
	
	priv = kmalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv) 
	    return -ENOMEM;
	
	path = build_path_from_dentry(dentry);
	if (!path) {
	    kfree(priv);
	    return -ENOMEM;   
	}
	
	priv->fd = io->ops.open(io, path, flag, mode);   
	if (priv->fd > 0) {
		ret = io->ops.stat(io, path, &stat_info);
		if (ret) {
			REMOTEFS_ERR("stat [%s] error, ret: %d\n", path, ret);
			io->ops.close(io, priv->fd);
			result = -EIO;	
			kfree(priv);
			goto out;
		}
		
	    filp->private_data = (void *)priv;
    	inode->i_atime.tv_sec = stat_info.st_atime;
    	if (inode->i_mtime.tv_sec != stat_info.st_mtime) {
    		struct page *page = NULL;
    		int index = 0;
    		do {
    			page = find_get_page(inode->i_mapping, index++);
    			if (page)
    				ClearPageUptodate(page);
    		} while (page);
    		inode->i_mtime.tv_sec = stat_info.st_mtime;
    	}
    	inode->i_ctime.tv_sec = stat_info.st_ctime; 
	}
	else {
	    REMOTEFS_ERR("open file[%s] error\n", path);
	    kfree(priv);
	    result = -EIO;
		goto out;
	}

out:
	kfree(path);
	return result;
}

static int remotefs_file_release(struct inode *inode, struct file *filp)
{
    struct remote_priv *priv = (struct remote_priv *)filp->private_data;
    char *path = build_path_from_dentry(filp->f_path.dentry);
    remotefs_stat_t stat_info;
    int ret = 0;
    
    io->ops.close(io, priv->fd);
    
    if (filp->f_flags & O_WRONLY) {
	    if (path) {
		    ret = io->ops.stat(io, path, &stat_info);
		    if (unlikely(ret < 0)) {
		    	REMOTEFS_ERR("stat error, ret:%d, %s, %d\n", ret, __func__, __LINE__);	
		    }
		    else {
		    	if ((i_size_read(inode) != stat_info.st_size) && (stat_info.st_size > 0)) {
			    	REMOTEFS_DEBUG("size: %d, %s, %d\n", (int)stat_info.st_size, __func__, __LINE__);	
			    	i_size_write(inode, stat_info.st_size);
			    	mark_inode_dirty(inode);
			    }
		    }
		    kfree(path);
		}
	}
    
    kfree(priv);
     
    return 0;
}

static void remotefs_file_truncate(struct inode *inode)
{
//	struct remotefs_sb_info *sbi = REMOTEFS_SB(inode->i_sb);
	
	REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
	
	inode->i_ctime = inode->i_mtime = CURRENT_TIME_SEC;
	if (!IS_DIRSYNC(inode))
		mark_inode_dirty(inode);
	
	//inode->i_blocks = inode->i_size / inode->;
}

static int remotefs_readpage(struct file *file, struct page *page)
{
	loff_t offset = (loff_t)page->index << PAGE_CACHE_SHIFT;
	char *read_data;
	int ret;
	
	REMOTEFS_DEBUG("offset: %lld, %s, %d\n", offset, __func__, __LINE__);
	
	page_cache_get(page);
	read_data = kmap(page);

	ret = remotefs_file_read_kernel(file, read_data, PAGE_CACHE_SIZE, &offset);
	if (ret < 0)
		goto io_error;

	file->f_path.dentry->d_inode->i_atime =
		current_fs_time(file->f_path.dentry->d_inode->i_sb);

	if (PAGE_CACHE_SIZE > ret)
		memset(read_data + ret, 0, PAGE_CACHE_SIZE - ret);

	flush_dcache_page(page);
	SetPageUptodate(page);
	ret = 0;

io_error:
	kunmap(page);
	page_cache_release(page);
	unlock_page(page);
	return ret;
}

static struct dentry * remotefs_lookup(struct inode *dir, struct dentry *dentry, struct nameidata *nd)
{
    struct super_block *sb = dir->i_sb;
    char *path = build_path_from_dentry(dentry);
    struct remotefs_stat stat_info;
    struct remotefs_inode_info inode_info;
    struct remotefs_sb_info *sb_info = REMOTEFS_SB(dir->i_sb);
    struct inode *inode = NULL;
    int ret = 0;

    REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
    
    if (!path)
    	return ERR_PTR(-ENOMEM);
    
    lock_super(sb);
    
    ret = io->ops.stat(io, path, &stat_info);   
    
    if (ret < 0) {
        if (ret == -2) {
        	ret = 0;
        }
        else {
        	REMOTEFS_ERR("stat error: %d\n", ret);  
        	ret = -EIO;	
        }
        goto out;
    }
    
    inode_info.st_ino = find_ino(dir, dentry->d_name.name);
    REMOTEFS_DEBUG("inode_num: %d, %s, %d\n", (int)inode_info.st_ino, __func__, __LINE__);
    if (inode_info.st_ino == 0) {
        struct entry_inode_num *entry = kmalloc(sizeof(struct entry_inode_num), GFP_KERNEL);
        if (!entry) {
            ret = -ENOMEM;
            goto out;
        }
        inode_info.st_ino = iunique(sb, REMOTEFS_ROOT_I);
        entry->num = inode_info.st_ino;
        strncpy(entry->name, dentry->d_name.name, dentry->d_name.len);
        INIT_LIST_HEAD(&entry->list);
        list_add(&entry->list, &REMOTEFS_I(dir)->entry_list);
    }

    if (S_ISREG(stat_info.st_mode)) {
        inode_info.st_mode = sb_info->mnt_file_mode &  stat_info.st_mode;
    } 
    else if (S_ISDIR(stat_info.st_mode)) {
        inode_info.st_mode = sb_info->mnt_dir_mode & stat_info.st_mode;  
    } 
    else {
        REMOTEFS_ERR("stat not support mode: %08X\n", stat_info.st_mode);
        goto out;     
    }
    inode_info.st_nlink = stat_info.st_nlink;  
    inode_info.st_uid = sb_info->mnt_uid;    
    inode_info.st_gid = sb_info->mnt_gid;    
    inode_info.st_size = stat_info.st_size;   
    inode_info.st_blksize = stat_info.st_blksize;
    inode_info.st_blocks = stat_info.st_blocks;
    inode_info.st_atime = stat_info.st_atime;
    inode_info.st_mtime = stat_info.st_mtime;
    inode_info.st_ctime = stat_info.st_ctime;
    
    REMOTEFS_DEBUG("st_ino: %d, st_mode: %08X, st_nlink: %d, st_uid: %d, st_gid: %d\n", 
            (int)inode_info.st_ino, inode_info.st_mode, (int)inode_info.st_nlink, (int)inode_info.st_uid, (int)inode_info.st_gid);
            
    REMOTEFS_DEBUG("st_size: %d, st_blksize: %d, st_blocks: %d\n", 
            (int)inode_info.st_size, (int)inode_info.st_blksize, (int)inode_info.st_blocks);
            
    REMOTEFS_DEBUG("st_atime: %08X, st_mtime: %08X, st_ctime: %08X\n", 
            (u32)inode_info.st_atime, (u32)inode_info.st_mtime, (u32)inode_info.st_ctime);
    
    unlock_super(sb);
    remotefs_renew_times(dentry);
    inode = remotefs_iget(dir->i_sb, &inode_info);
    if (inode) {
    	inode->i_atime.tv_sec = inode_info.st_atime;
    	if (inode->i_mtime.tv_sec != inode_info.st_mtime) {
    		struct page *page = NULL;
    		int index = 0;
    		do {
    			page = find_get_page(inode->i_mapping, index++);
    			if (page)
    				ClearPageUptodate(page);
    		} while (page);
    		inode->i_mtime.tv_sec = inode_info.st_mtime;
    		
    	}
    	inode->i_ctime.tv_sec = inode_info.st_ctime;
    }
    d_add(dentry, inode);
    if (dentry) {
		dentry->d_op = &remotefs_dentry_ops;
		dentry->d_time = dentry->d_parent->d_inode->i_version;
	}

    kfree(path);
    return NULL;

out:
    unlock_super(sb);   
    d_add(dentry, NULL);
    if (dentry) {
		dentry->d_op = &remotefs_dentry_ops;
		dentry->d_time = dentry->d_parent->d_inode->i_version;
	}
	
    kfree(path);
    return ERR_PTR(ret); 
}

static int remotefs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode)
{
	struct super_block *sb = dir->i_sb;
	char *path = build_path_from_dentry(dentry);
	struct inode *inode = NULL;
	struct remotefs_inode_info inode_info;
	struct timespec ts;
	struct entry_inode_num *entry;
	int err;
    
    REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
    
	lock_super(sb);

	ts = CURRENT_TIME_SEC;
	err = io->ops.mkdir(io, path, mode);
	if (err) {
	    err = -EIO;
	    goto out;
	}
	
	dir->i_ctime = dir->i_mtime = dir->i_atime = ts;
	if (!IS_DIRSYNC(dir))
		mark_inode_dirty(dir);
		
	dir->i_version++;
	inc_nlink(dir);
	
	inode_info.st_ino = iunique(dentry->d_sb, REMOTEFS_ROOT_I); 
	inode_info.st_mode = ((dir->i_mode &  ~S_IFREG) & mode) | S_IFDIR;
	inode_info.st_uid = current_uid();    
    inode_info.st_gid = current_gid();   
    inode_info.st_size = 0;   
    inode_info.st_blksize = REMOTEFS_I(dir)->st_blksize;
    inode_info.st_blocks = 0;
    
	inode = remotefs_iget(dir->i_sb, &inode_info);
	if (IS_ERR(inode)) {
	    err = PTR_ERR(inode);
	    goto out;
	}
	
	entry = kmalloc(sizeof(struct entry_inode_num), GFP_KERNEL);
    if (!entry) {
        err = -ENOMEM;
        goto out;
    }
    
    entry->num = inode_info.st_ino;
    strncpy(entry->name, dentry->d_name.name, dentry->d_name.len);
    INIT_LIST_HEAD(&entry->list);
    list_add(&entry->list, &REMOTEFS_I(dir)->entry_list);
	
	inode->i_version++;
	//inode->i_nlink = 0;
	inode->__i_nlink = 0;
	inode->i_mtime = inode->i_atime = inode->i_ctime = ts;
	/* timestamp is already written, so mark_inode_dirty() is unneeded. */

	dentry->d_time = dentry->d_parent->d_inode->i_version;
	unlock_super(sb);
//	d_add(dentry, inode);
	d_instantiate(dentry, inode);
	kfree(path);
	
	return 0;

out:
	unlock_super(sb);
	if (inode) iput(inode);
	d_add(dentry, NULL);
	kfree(path);
	
	return err;
}

static int remotefs_rmdir(struct inode *dir, struct dentry *dentry)
{
	struct inode *inode = dentry->d_inode;
	struct super_block *sb = dir->i_sb;
	char *path = build_path_from_dentry(dentry);
	int err;
    
    REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
    
    if (!path) 
    	return -ENOMEM;
    
	lock_super(sb);
    
    err = io->ops.rmdir(io, path);
    if (err < 0) {
    	REMOTEFS_ERR("err: %d, %s, %d\n", err, __func__, __LINE__);
        err = -EIO;
        goto out;   
    }
    
    dir->i_version++;
    dir->i_mtime = dir->i_atime = CURRENT_TIME_SEC;
	if (!IS_DIRSYNC(dir))
		mark_inode_dirty(dir);
		
    drop_nlink(dir);
    clear_nlink(inode);
	inode->i_mtime = inode->i_atime = CURRENT_TIME_SEC;
  
out:
	unlock_super(sb);
	d_drop(dentry);
	kfree(path);

	return err;
}

static int remotefs_unlink(struct inode *dir,struct dentry *dentry)
{
    struct inode *inode = dentry->d_inode;
	struct super_block *sb = dir->i_sb;
	char *path = build_path_from_dentry(dentry);
	int err = 0;
	
	REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
	
	if (!path) 
		return -ENOMEM;
	
	lock_super(sb);

	err = io->ops.remove(io, path);
	if (err < 0) {
		REMOTEFS_ERR("err: %d, %s, %d\n", err, __func__, __LINE__);
		err = 0;
		goto out;
	}
	
	if (!IS_DIRSYNC(dir))
		mark_inode_dirty(dir);

	dir->i_version++;
    dir->i_mtime = dir->i_atime = CURRENT_TIME_SEC;
	if (!IS_DIRSYNC(dir))
		mark_inode_dirty(dir);
		
    drop_nlink(dir);
    clear_nlink(inode);
	inode->i_mtime = inode->i_atime = CURRENT_TIME_SEC;
out:
	unlock_super(sb);
	d_drop(dentry);
	kfree(path);
	return err;
}

static int remotefs_rename(struct inode *old_dir, struct dentry *old_dentry,
		       struct inode *new_dir, struct dentry *new_dentry)
{
	struct inode *old_inode, *new_inode;
	struct timespec ts;
	int err = 0, is_dir;
	struct super_block *sb = old_dir->i_sb;
	char *new_path, *old_path;
	
	REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
	
	old_inode = old_dentry->d_inode;
	new_inode = new_dentry->d_inode;
	old_path = build_path_from_dentry(old_dentry);
	new_path = build_path_from_dentry(new_dentry);
	if (!old_path || !new_path) {
		err = -ENOMEM;
		goto out;	
	}
	
	lock_super(sb);

	is_dir = S_ISDIR(old_inode->i_mode);
	
	err = io->ops.rename(io, old_path, new_path);
	if (err < 0) {
		err = -EIO;
		goto out;	
	}
	
	ts = CURRENT_TIME_SEC;
	new_dir->i_version++;

	if (!IS_DIRSYNC(new_dir))
		mark_inode_dirty(old_inode);
	
	drop_nlink(old_dir);
	if (!new_inode)
		inc_nlink(new_dir);

	old_dir->i_version++;
	old_dir->i_ctime = old_dir->i_mtime = ts;
	if (!IS_DIRSYNC(old_dir))
		mark_inode_dirty(old_dir);

	if (new_inode) {
		drop_nlink(new_inode);
		if (is_dir)
			drop_nlink(new_inode);
		new_inode->i_ctime = ts;
	}
out:
	unlock_super(sb);
	if (old_path) kfree(old_path);
	if (new_path) kfree(new_path);
	return err;
}

#define TIMES_SET_FLAGS	(ATTR_MTIME_SET | ATTR_ATIME_SET | ATTR_TIMES_SET)
#define REMOTEFS_VALID_MODE (S_IFREG | S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO)

static int remotefs_sanitize_mode(const struct remotefs_sb_info *sbi,
			     struct inode *inode, umode_t *mode_ptr)
{
	mode_t mask, perm;

	/*
	 * Note, the basic check is already done by a caller of
	 * (attr->ia_mode & ~MSDOS_VALID_MODE)
	 */

	if (S_ISREG(inode->i_mode))
		mask = sbi->mnt_file_mode;
	else
		mask = sbi->mnt_dir_mode;

	perm = *mode_ptr & ~(S_IFMT);

//	/*
//	 * Of the r and x bits, all (subject to umask) must be present. Of the
//	 * w bits, either all (subject to umask) or none must be present.
//	 */
//	if ((perm & (S_IRUGO | S_IXUGO)) != (inode->i_mode & (S_IRUGO|S_IXUGO))) {
//		REMOTEFS_ERR("error!! perm: %08X, inode->i_mode: %08X, %s, %d\n", perm, inode->i_mode, __func__, __LINE__);
//		return -EPERM;
//	}
//	
//	if ((perm & S_IWUGO) && ((perm & S_IWUGO) != (S_IWUGO & ~mask))) {
//		REMOTEFS_ERR("error!! perm: %08X, inode->i_mode: %08X, %s, %d\n", perm, inode->i_mode, __func__, __LINE__);
//		return -EPERM;
//	}

	*mode_ptr &= S_IFMT | perm;

	return 0;
}

static int remotefs_setattr(struct dentry *dentry, struct iattr *attr)
{
	struct inode *inode = dentry->d_inode;
	struct remotefs_sb_info *sbi = REMOTEFS_SB(dentry->d_sb);
	char *path = build_path_from_dentry(dentry);
	int error = 0;
	unsigned int ia_valid;
	
	REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
	
	if (!path)
		return -ENOMEM;
	
	if (attr->ia_valid & ATTR_SIZE) {
		REMOTEFS_DEBUG("attr->ia_size:%llu, inode->i_size:%llu, %s, %d\n", 
				attr->ia_size, inode->i_size, __func__, __LINE__);
		if (attr->ia_size > inode->i_size) {
			error = io->ops.truncate(io, path, attr->ia_size);
			//attr->ia_valid &= ~ATTR_SIZE;
		}
	}

	/* Check for setting the inode time. */
	ia_valid = attr->ia_valid;
	if (ia_valid & TIMES_SET_FLAGS) {
		attr->ia_valid &= ~TIMES_SET_FLAGS;
	}

	error = inode_change_ok(inode, attr);
	attr->ia_valid = ia_valid;
	if (error) {
		REMOTEFS_ERR("error: %d, %s, %d\n", error, __func__, __LINE__);
		goto out;
	}

	if (((attr->ia_valid & ATTR_UID) && (attr->ia_uid != sbi->mnt_uid)) 
			|| ((attr->ia_valid & ATTR_GID) && (attr->ia_gid != sbi->mnt_gid))
			|| ((attr->ia_valid & ATTR_MODE) && (attr->ia_mode & ~REMOTEFS_VALID_MODE))) {
		REMOTEFS_ERR("attr->ia_valid: %08X, err, %s, %d\n", attr->ia_valid, __func__, __LINE__);
		error = -EPERM;
	}

	/*
	 * We don't return -EPERM here. Yes, strange, but this is too
	 * old behavior.
	 */
	if (attr->ia_valid & ATTR_MODE) {
		if (remotefs_sanitize_mode(sbi, inode, &attr->ia_mode)) {
			attr->ia_valid &= ~ATTR_MODE;
		}
	}

	//error = inode_setattr(inode, attr);
	setattr_copy(inode, attr);
out:
	kfree(path);
	return error;
}

static int remotefs_getattr(struct vfsmount *mnt, struct dentry *dentry, struct kstat *stat)
{
	struct inode *inode = dentry->d_inode;
	generic_fillattr(inode, stat);
	return 0;
}

static int remotefs_inode_create(struct inode *dir, struct dentry *dentry, umode_t mode, struct nameidata *nd)
{
    struct super_block *sb = dir->i_sb;
//    struct remotefs_sb_info *sb_info = REMOTEFS_SB(dir->i_sb);
	struct inode *inode;
	char *path = build_path_from_dentry(dentry);
	struct remotefs_inode_info inode_info;
//	struct remotefs_stat stat_info;
	struct timespec ts;
	struct entry_inode_num *entry;
	int err = 0;
    
    REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
    
    if (!path)
    	return -ENOMEM;
    
	lock_super(sb);

	ts = CURRENT_TIME_SEC;
	dir->i_version++;
	
	inode_info.st_ino = iunique(dentry->d_sb, REMOTEFS_ROOT_I);//REMOTEFS_ROOT_I + stat_info.st_ino;  
	inode_info.st_mode = ((dir->i_mode &  ~S_IFDIR) & mode) | S_IFREG;
	
    if (S_ISREG(mode)) {
        inode_info.st_mode = ((dir->i_mode &  ~S_IFDIR) & mode) | S_IFREG;
        REMOTEFS_DEBUG("it is regu file\n");
    } 
    else if (S_ISDIR(mode)) {
    	inode_info.st_mode = ((dir->i_mode &  ~S_IFREG) & mode) | S_IFDIR;
        REMOTEFS_DEBUG("it is dir\n");
    } 
    else {
        REMOTEFS_ERR("stat not support mode: %08X\n", inode_info.st_mode);
        goto out;     
    }
    
    inode_info.st_nlink = 0;
    inode_info.st_uid = current_uid();    
    inode_info.st_gid = current_gid();   
    inode_info.st_size = 0;   
    inode_info.st_blksize = REMOTEFS_I(dir)->st_blksize;
    inode_info.st_blocks = 0;

	inode = remotefs_iget(sb, &inode_info);
	if (IS_ERR(inode)) {
		err = PTR_ERR(inode);
		goto out;
	}
	
	entry = kmalloc(sizeof(struct entry_inode_num), GFP_KERNEL);
    if (!entry) {
    	iput(inode);
        err = -ENOMEM;
        goto out;
    }
    
    entry->num = inode_info.st_ino;
    strncpy(entry->name, dentry->d_name.name, dentry->d_name.len);
    INIT_LIST_HEAD(&entry->list);
    list_add(&entry->list, &REMOTEFS_I(dir)->entry_list);

	inode->i_version++;
	inode->i_mtime = inode->i_atime = inode->i_ctime = ts;
	/* timestamp is already written, so mark_inode_dirty() is unneeded. */

	dentry->d_time = dentry->d_parent->d_inode->i_version;
//	d_add(dentry, inode);
	d_instantiate(dentry, inode);

out:
	unlock_super(sb);
	kfree(path);
	return err;
}

static int remotefs_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
    struct inode *inode = filp->f_path.dentry->d_inode;
    //struct dentry *dir_dentry = filp->f_path.dentry;
//    struct remotefs_inode_info *inode_info = REMOTEFS_I(inode);
	struct super_block *sb = inode->i_sb;
	struct remote_priv *priv = (struct remote_priv *)filp->private_data;
	remotefs_dirent_t ent;
	loff_t offset;
	int inode_num;
	int ret;
	int err;
	
	lock_super(sb);
	
	offset = filp->f_pos;
	REMOTEFS_DEBUG("offset: %llu, inode size: %llu, %s, %d\n", offset, inode->i_size, __func__, __LINE__);
//	if (offset >= inode->i_size) {
//		ret = 0;
//		REMOTEFS_DEBUG("offset: %llu, inode size: %llu, %s, %d\n", offset, inode->i_size, __func__, __LINE__);
//		goto error;
//	}

	/* Directory entries are always 4-byte aligned */
	if (offset & 3) {
		ret = -EINVAL;
		goto error;
    }
    
    ret = io->ops.seekdir(io, priv->fd, offset);
    if (ret < 0) {
        REMOTEFS_ERR("io->ops.seekdir error: %d\n", ret);
        ret = -EIO;   
        goto error;
    }

	ret = -1;
	while (1) {
        ret = io->ops.readdir(io, priv->fd, offset, &ent, sizeof(struct dirent));
        if (ret > 0) {
            inode_num = find_ino(inode, ent.d_name);
            if (inode_num == 0) {
                struct entry_inode_num *entry = kmalloc(sizeof(struct entry_inode_num), GFP_KERNEL);
                if (!entry) {
                    ret = -ENOMEM;
                    goto error;
                }
                
                inode_num = iunique(sb, REMOTEFS_ROOT_I);
                entry->num = inode_num;
                strncpy(entry->name, ent.d_name, REMOTEFS_ENTRY_NAME_MAX);
                INIT_LIST_HEAD(&entry->list);
                list_add(&entry->list, &REMOTEFS_I(inode)->entry_list);
            }
#if 0
            else {
            	struct dentry *dentry;
            	
            	list_for_each_entry(dentry, &dir_dentry->d_subdirs, d_u.d_child) {
            		if (!strncmp(dentry->d_name.name, ent.d_name, REMOTEFS_ENTRY_NAME_MAX)) {
            			d_drop(dentry);
            			mark_inode_dirty(dentry->d_inode);
            			break;
            		}
            	}	
            }
#endif            
            err = filldir(dirent, ent.d_name, strnlen(ent.d_name, REMOTEFS_ENTRY_NAME_MAX), offset, inode_num, ent.d_type);
            if (err) {
                ret = err;
                REMOTEFS_ERR("filldir err: %d, %s, %d\n", err, __func__, __LINE__);
                goto error;
            }
            offset = ent.d_off;
        }
        else if (ret == 0) {
            break;
        }
        else {
        	ret = -EIO;
        	REMOTEFS_ERR("readdir error, ret: %d, %s, %d\n", ret, __func__, __LINE__);
            goto error;
        }
    }
    
error:
    filp->f_pos = offset;
    unlock_super(sb);
    mark_inode_dirty(inode);
    return ret;   
}

static const struct inode_operations remotefs_file_inode_operations = {
    .create = remotefs_inode_create,  
    .truncate = remotefs_file_truncate,
    .setattr = remotefs_setattr,
    .getattr    = remotefs_getattr,
};

static const struct file_operations remotefs_file_operations = {
	.llseek     = remotefs_file_llseek,
	.open       = remotefs_file_open,
	.read		= remotefs_file_read,
	.write      = remotefs_file_write,
	.release    = remotefs_file_release,
	.mmap		= generic_file_mmap,
};

const struct address_space_operations remotefs_addr_ops = {
	.readpage = remotefs_readpage,
};

static const struct file_operations remotefs_directory_operations = {
	.open       = remotefs_file_open,
	.llseek		= remotefs_file_llseek,
	.read		= remotefs_file_read,
	.readdir	= remotefs_readdir,
	.release    = remotefs_file_release,
};

static const struct inode_operations remotefs_dir_inode_operations = {
	.create     = remotefs_inode_create,
	.lookup	    = remotefs_lookup,
	.mkdir      = remotefs_mkdir,
	.rmdir      = remotefs_rmdir,
	.unlink     = remotefs_unlink,
	.rename     = remotefs_rename,
	.setattr    = remotefs_setattr,
	.getattr    = remotefs_getattr,
};

static void remotefs_d_release(struct dentry *dentry)
{
	struct remotefs_inode_info *inode_info = REMOTEFS_I(dentry->d_parent->d_inode);
    struct entry_inode_num *entry;
    struct list_head *head = &inode_info->entry_list;
    
    REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
    
    if (IS_ROOT(dentry)) return;
    
    list_for_each_entry(entry, head, list) {
        if (strnlen(entry->name, REMOTEFS_ENTRY_NAME_MAX) > dentry->d_name.len) {
            continue;
        }
        if (strncmp(dentry->d_name.name, entry->name, dentry->d_name.len)) {
            continue;
        }
        else {
            list_del(&entry->list);
            kfree(entry); 
            break;
        }
    }
}

static struct dentry_operations remotefs_dentry_ops = {
    .d_release =       remotefs_d_release,   
};

static struct inode *remotefs_alloc_inode(struct super_block *sb)
{
    struct remotefs_inode_info *remotefs_inode;
    
    REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
	remotefs_inode = kmem_cache_alloc(remotefs_inode_cashep, GFP_KERNEL);
	if (!remotefs_inode)
		return NULL;
	return &remotefs_inode->vfs_inode;
}

static void remotefs_destroy_inode(struct inode *inode)
{
//    struct remotefs_inode_info *inode_info = REMOTEFS_I(inode);
//    struct entry_inode_num *entry;
//    struct list_head *head = &inode_info->entry_list;
    
    REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
    
//    list_for_each_entry(entry, head, list) {
//        list_del(&entry->list); 
//        kfree(entry);
//    }
    
    kmem_cache_free(remotefs_inode_cashep, REMOTEFS_I(inode));
}

static void remotefs_delete_inode(struct inode *inode)
{
	int wait;

        truncate_inode_pages(&inode->i_data, 0);
        end_writeback(inode);
        //coda_cache_clear_inode(inode);
	inode->i_size = 0;
    	wait = IS_DIRSYNC(inode);
    	inode->i_ctime = inode->i_mtime = CURRENT_TIME_SEC;
    	if (!wait) 
		mark_inode_dirty(inode);

#if 0
	REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
	truncate_inode_pages(&inode->i_data, 0);
	inode->i_size = 0;
    	wait = IS_DIRSYNC(inode);
    	inode->i_ctime = inode->i_mtime = CURRENT_TIME_SEC;
    	if (!wait) 
		mark_inode_dirty(inode);
	//clear_inode(inode);
#endif
	return;
}

static void remotefs_put_super(struct super_block *sb)
{
	struct remotefs_sb_info *sbi = REMOTEFS_SB(sb);
	
	REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
	
	module_put(io->owner);
	sb->s_fs_info = NULL;
	kfree(sbi);
}

static int remotefs_statfs(struct dentry *dentry, struct kstatfs *buf)
{
    remotefs_statfs_t statfs_info;
    char *path = build_path_from_dentry(dentry);
    int ret;
    
    REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
    
    if (!path)
    	return -ENOMEM;
    
    ret = io->ops.statfs(io, path, &statfs_info);
    if (ret < 0) {
    	REMOTEFS_ERR("error: %d, path: %s, %s, %d\n", ret, path, __func__, __LINE__);
    	ret = -EIO; 
    	goto out;   	
   	}
   	
   	buf->f_type = statfs_info.f_type;
   	buf->f_bsize = statfs_info.f_bsize;
   	buf->f_blocks = statfs_info.f_blocks;
   	buf->f_bfree = statfs_info.f_bfree;
   	buf->f_bavail = statfs_info.f_bavail;
   	buf->f_files = statfs_info.f_files;
   	buf->f_ffree = statfs_info.f_ffree;
   	buf->f_fsid.val[0] = statfs_info.f_fsid;
   	buf->f_fsid.val[1] = 0;
   	buf->f_namelen = statfs_info.f_namemax;
   	buf->f_frsize = statfs_info.f_frsize;
out:   	
   	kfree(path);
    return ret;
}

static int remotefs_show_options(struct seq_file *s, struct dentry *m)
{
	REMOTEFS_DEBUG("%s, %d\n", __func__, __LINE__);
	return 0;
}

static int remotefs_remount(struct super_block *sb, int *flags, char *data)
{
	*flags |= MS_NODIRATIME;
	return 0;
}

static const struct super_operations remotefs_sops =
{
	.alloc_inode	= remotefs_alloc_inode,
	.destroy_inode	= remotefs_destroy_inode,
	.drop_inode	= generic_delete_inode,
	//.delete_inode	= remotefs_delete_inode,
	.evict_inode	= remotefs_delete_inode,
	.put_super	= remotefs_put_super,
	.statfs		= remotefs_statfs,
	.show_options	= remotefs_show_options,
	.remount_fs	= remotefs_remount,
};

static int remotefs_fill_super(struct super_block *sb, void *raw_data, int silent)
{
	struct inode *root_inode;
	struct remotefs_sb_info *sb_info;
	struct remotefs_inode_info remotefs_root_info;
	remotefs_stat_t stat_info;
	int ret = 0;

	if (!raw_data) {
		ret = -EINVAL;
		goto out_no_data;
	}
	
    sb_info = kmalloc(sizeof (*sb_info), GFP_KERNEL);
    if (!sb_info) 
        return -ENOMEM;
    
    sb->s_fs_info = sb_info;
    sb->s_flags |= MS_NODIRATIME;
    sb->s_blocksize = 65536;
    sb->s_blocksize_bits = bits_count(sb->s_blocksize);
    sb->s_magic = 0;
    sb->s_op = &remotefs_sops;
    sb->s_time_gran = 100;
	
    sb_info->mnt_uid = current_uid();
    sb_info->mnt_gid = current_gid();

    sb_info->mnt_file_mode = S_IRWXUGO | S_IFREG;
    sb_info->mnt_dir_mode = S_IRWXUGO | S_IFDIR;

	ret = io->ops.stat(io, "", &stat_info);
	if (ret < 0) {
		REMOTEFS_ERR("error: %d, %s, %d\n", ret, __func__, __LINE__);
		ret = -EIO;
		goto out_fail;
	}
    
    remotefs_root_info.st_ino = REMOTEFS_ROOT_I;
    remotefs_root_info.st_mode = stat_info.st_mode;//sb_info->mnt_dir_mode | S_IFDIR;
    if (!S_ISDIR(remotefs_root_info.st_mode)) {
    	REMOTEFS_ERR("error root, it is not a directory! %s, %d\n", __func__, __LINE__);
    	ret = -EINVAL;
    	goto out_fail;	
    }
    
    remotefs_root_info.st_size = stat_info.st_size;
    remotefs_root_info.st_nlink = stat_info.st_nlink;
    remotefs_root_info.st_blksize = stat_info.st_blksize;
    remotefs_root_info.st_blocks = stat_info.st_blocks;
    
    REMOTEFS_DEBUG("st_size: %u, st_nlink: %u, st_blocks: %u, st_blksize: %u\n", 
    		(u32)remotefs_root_info.st_size, (u32)remotefs_root_info.st_nlink, remotefs_root_info.st_blocks, remotefs_root_info.st_blksize);

	root_inode = remotefs_iget(sb, &remotefs_root_info);
	if (!root_inode) {
	    REMOTEFS_ERR("root_inode null\n");
		goto out_no_root_inode;
	}
    
	//sb->s_root = d_alloc_root(root_inode);
	sb->s_root = d_make_root(root_inode);
	if (!sb->s_root) {
	    REMOTEFS_ERR("sb->s_root null\n");
		goto out_no_root_dentry;
	}
	sb->s_root->d_time = jiffies;

	return 0;

out_no_root_dentry:
	REMOTEFS_ERR("out_no_root_dentry\n");
	iput(root_inode);
out_no_root_inode:
	REMOTEFS_ERR("out_no_root_inode\n");
out_fail:
    REMOTEFS_ERR("remotefs_fill_super: out_fail\n");
    kfree(sb_info);
out_no_data:
	REMOTEFS_ERR("remotefs_fill_super: out_no_data\n");
	return ret;
}


#if 0
static int remotefs_get_sb(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data, struct vfsmount *mnt)
{
	io = (struct remote_io *)__remote_io;
	if (!io) {
	    REMOTEFS_ERR("remote io not ready\n");
	    return -ENXIO;
	}
	else {
		if (!try_module_get(io->owner)) {
			REMOTEFS_ERR("can not get remote io module\n");
			return -ENODEV;
		}	
	}
	
	return get_sb_nodev(fs_type, flags, data, remotefs_fill_super,
			   mnt);
}
#endif

static struct dentry * remotefs_get_sb(struct file_system_type *fs_type,
                  int flags, const char *dev_name,
                  void *data)
{
	io = (struct remote_io *)__remote_io;
	if (!io) {
	    REMOTEFS_ERR("remote io not ready\n");
	    return NULL;
	}
        return mount_single(fs_type, flags, data, remotefs_fill_super);
}

static struct file_system_type remotefs_fs_type = {
	.owner		= THIS_MODULE,
	.name		= "remotefs",
	//.get_sb	= remotefs_get_sb,
	.mount		= remotefs_get_sb,
	//.kill_sb	= kill_anon_super,
	.kill_sb	= kill_litter_super,
	//.fs_flags	= FS_BINARY_MOUNTDATA,
};

static int __init init_remotefs_fs(void)
{
	remotefs_inode_cashep = kmem_cache_create("remotfe_inode_cache",
					      sizeof(struct remotefs_inode_info),
					      0, 
					      (SLAB_RECLAIM_ACCOUNT | SLAB_MEM_SPREAD),
					      remotefs_init_once);
	if (remotefs_inode_cashep == NULL)
		return -ENOMEM;
	
	return register_filesystem(&remotefs_fs_type);
}

static void __exit exit_remotefs_fs(void)
{
	kmem_cache_destroy(remotefs_inode_cashep);
	unregister_filesystem(&remotefs_fs_type);
}

module_init(init_remotefs_fs)
module_exit(exit_remotefs_fs)

MODULE_DESCRIPTION("remote file system");
MODULE_AUTHOR("wyzhao, <wyzhao@actions-semi.com>");
MODULE_LICENSE("GPL");

