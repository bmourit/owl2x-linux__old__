

#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/delay.h>

#include <asm/uaccess.h>

struct phy {
unsigned char reg;
unsigned char value;
};

union phy_op_data {
	struct phy phy;
	int time;
};

#define SET_PHY_OP_END		0
#define SET_PHY_OP_SET		1
#define SET_PHY_OP_UDELAY	2

struct set_phy_op {
	int op_type;
	union phy_op_data data;
};

/**
 * kernel_addr_limit_expend 
 * wrapper for the memory boundary extention
 * function
 */
static mm_segment_t kernel_addr_limit_expend(void)
{
    mm_segment_t old_fs; //save variable memory boundaries
    old_fs = get_fs();
    set_fs(KERNEL_DS);
 
    return old_fs;
}

/**
 * kernel_addr_limit_resume
 * Wrapper for the memory boundary restore function
 * parameters none.
 */
static void kernel_addr_limit_resume(mm_segment_t old_fs)
{
    set_fs(old_fs);
}

static struct file *kernel_file_open(char *file_path, int flags)
{
	struct file *file = NULL;
	file = filp_open(file_path,flags,0);

	if (IS_ERR(file)) {
        printk("Open file %s failed.\n", file_path);
        return NULL;
	}

	return file;
}

static loff_t kernel_file_size(struct file *file)
{
	struct inode *inode = NULL; 
	loff_t fsize; 
	inode = file->f_dentry->d_inode;
	fsize = inode->i_size;
	printk(KERN_ALERT "size=%d\n", (int)fsize);

	return fsize;
}

static char *kernel_file_buf_alloc(size_t size)
{
	return kzalloc(size+100, GFP_KERNEL);
}

static void kernel_file_buf_free(char *file_buf)
{
	if (file_buf != NULL)
		kfree(file_buf);
}

static ssize_t kernel_file_read(struct file *file, char *buf, size_t count)
{
	loff_t *pos = &(file->f_pos);
	return vfs_read(file, buf, count, pos);
}


static char *read_phy_config_file(char *file_path, ssize_t *len)
{
	struct file *file = NULL;
	char *file_buf;
	ssize_t ret;
	loff_t fsize; 
	mm_segment_t old_fs;

	printk("%s\n",__FUNCTION__);
	file = kernel_file_open(file_path, O_RDONLY); 

	if (!file)
		return NULL;
	
	fsize = kernel_file_size(file); 
	old_fs = kernel_addr_limit_expend(); 
    	file_buf = kernel_file_buf_alloc(fsize);

	if (!file_buf)
		goto file_alloc_fail;

	ret = kernel_file_read(file, file_buf, fsize); 
	if (ret != fsize) {
		printk("the file size is:%d and the read size is %d\n", (int)fsize, (int)ret);
		kernel_file_buf_free(file_buf);
		goto file_read_err;
	}
	kernel_addr_limit_resume(old_fs); 
	filp_close(file, NULL); 
	*len = fsize;
	return file_buf;

file_read_err:
	kernel_file_buf_free(file_buf);

file_alloc_fail:	
	kernel_addr_limit_resume(old_fs); 
	filp_close(file, NULL); 

	return NULL;
}


#if 0
static int kernel_file_write(struct file *file,char *buf,size_t count)
{
	loff_t *pos = &(file->f_pos);
	vfs_write(file,buf,count,pos);

	return 0;
}

#define FILE_PATH_WRITE "/storage/sdcard0/phy_config_write"
static int write_phy_config_file(char *file_buf, loff_t fsize) 
{
	struct file *file = NULL;
	mm_segment_t old_fs;
	printk("%s\n", __FUNCTION__);
	file = kernel_file_open(FILE_PATH_WRITE, O_RDWR|O_CREAT); 
	old_fs = kernel_addr_limit_expend();
	kernel_file_write(file, file_buf, fsize);
	filp_close(file, NULL); 
	kernel_addr_limit_resume(old_fs); 

	return 0;
}
#endif

#define SETPHY_PREFIX "setphy("
static inline int is_setphy_prefix(char *str)
{
	return (!strncmp(str, SETPHY_PREFIX, strlen(SETPHY_PREFIX)));
}

#define UDELAY_PREFIX "udelay("
static inline int is_udelay_prefix(char *str)
{
	return (!strncmp(str, UDELAY_PREFIX, strlen(UDELAY_PREFIX)));
}

#define SUFFIX  ");"
static inline int is_suffix(char *str)
{
	return (!strncmp(str, SUFFIX, strlen(SUFFIX)));
}


static inline int is_end(char *str)
{
	return (*str == '}');
}



static inline int is_hex_prefix(char *str)
{
	return ((*str == '0')&&(*(str+1) == 'x'));
}


static inline int is_0_to_9_char(int ch)
{
	return (ch >= '0') && (ch <= '9');
}


static inline int is_a_to_f_char(int ch)
{
	return (ch >= 'a') && (ch <= 'f');
}

static int parse_hex_string(char *str, char *value)
{
	char *ptr = str;
	char val_tmp = 0;
	int i;

	if (!is_hex_prefix(ptr))
		return  -1;

	ptr += 2;
	for(i = 0; i < 2; i++) {
		if (is_0_to_9_char(*ptr)) {
			val_tmp <<= 4;
			val_tmp += *ptr - '0';
		} else if(is_a_to_f_char(*ptr)) {
			val_tmp <<= 4;
			val_tmp += (*ptr - 'a' + 10);
		} else
			return -1;

		ptr++;
	}
	*value = val_tmp;

	return 0;
}

static int parse_phy_data(char *str, struct phy *phy)
{
	char reg;
	char value;
	char *ptr;
	int err;

	ptr = str + strlen(SETPHY_PREFIX);
	err = parse_hex_string(ptr, &reg);

	if (err != 0)
		return -1;

	ptr += 4;
	if(*ptr != ',')
		return -1;

	ptr++;
	err = parse_hex_string(ptr, &value);
	if (err != 0)
		return -1;

	ptr += 4;
	if (!is_suffix(ptr))
		return -1;

	ptr += strlen(SUFFIX);
	phy->reg = reg;
	phy->value = value;
	
	return ptr-str;
}

static int parse_udelay_time(char *str,int *time)
{
	int time_tmp = 0;
	char *ptr;
	ptr = str + strlen(UDELAY_PREFIX);
	if (!is_0_to_9_char(*ptr))
		return -1;
	
	while (is_0_to_9_char(*ptr)) {
		time_tmp *=10;
		time_tmp += *ptr - '0';
		ptr++;
	}

	if (!is_suffix(ptr))
		return -1;

	ptr += strlen(SUFFIX);
	*time = time_tmp;

	return ptr-str;	
}

static int parse_phy_config_string(struct set_phy_op *phy_op, int max_item, char *str, int max_len)
{
	int len;
	char *ptr = str;
	char *str_end = str + max_len;
	struct set_phy_op *phy_op_end = phy_op + max_item - 1;
	
	printk("%s\n", __FUNCTION__);

	while ((ptr < str_end) && (phy_op < phy_op_end)) {
		if (is_setphy_prefix(ptr)) {
			len = parse_phy_data(ptr, &phy_op->data.phy);
			if (len > 0) {
			 	ptr += len;
			 	phy_op->op_type = SET_PHY_OP_SET;
				phy_op++;
			} else {
			ptr += strlen(SETPHY_PREFIX);
			}
		} else if (is_udelay_prefix(ptr)) {
			len = parse_udelay_time(ptr,&phy_op->data.time);
		
			if (len > 0) {
			 	ptr += len;
			 	phy_op->op_type = SET_PHY_OP_UDELAY;
				phy_op++;
			} else {
			 	ptr += strlen(UDELAY_PREFIX);
			}
		} else if (is_end(ptr)) {
			break;
		} else { 
			ptr++;
		}
	}
	phy_op->op_type = SET_PHY_OP_END;
	
	return 0;
}

static void phy_init_printk(struct set_phy_op *phy_op, int max_item)
{
	int i;
	printk("%s\n",__FUNCTION__);
	for (i=0;i<max_item;i++) {
		if (phy_op->op_type == SET_PHY_OP_SET) {
			printk("setphy(0x%02x,0x%02x);\n", phy_op->data.phy.reg, phy_op->data.phy.value);
		} else if(phy_op->op_type == SET_PHY_OP_UDELAY) {
			printk("udelay(%d);\n",phy_op->data.time);
		} else {
			break;
		}
		phy_op++;
	}
}

extern void setphy(unsigned char reg_add, unsigned char value);

static inline void set_phy(unsigned char reg, unsigned char value)
{
	setphy(reg,value);
}

static void phy_init(struct set_phy_op *phy_op, int max_item)
{
	int i;
	printk("%s\n",__FUNCTION__);
	for (i=0;i<max_item;i++) {
		if (phy_op->op_type == SET_PHY_OP_SET) {
			set_phy(phy_op->data.phy.reg,phy_op->data.phy.value);
		} else if(phy_op->op_type == SET_PHY_OP_UDELAY) {
			udelay(phy_op->data.time);
		} else {
			break;
		}
		phy_op++;
	}
}

#define PHY_OP_MAX_ITEM  50
int set_phy_from_config_file(char *file_path) 
{
	char *file_buf;
	struct set_phy_op phy_op[PHY_OP_MAX_ITEM];
	int err;
	int len;

	printk("%s\n",__FUNCTION__);

	file_buf = read_phy_config_file(file_path, &len);
	if (!file_buf)
		return -1;
	printk("the content read from file:\n");
	printk("%s\n",file_buf);
	err = parse_phy_config_string(phy_op, PHY_OP_MAX_ITEM,file_buf,len);

	if (err != 0) {
		kernel_file_buf_free(file_buf);
		return -1;
	}
	phy_init_printk(phy_op, PHY_OP_MAX_ITEM);
	phy_init(phy_op, PHY_OP_MAX_ITEM);
	kernel_file_buf_free(file_buf);

	return 0;
}

/**
 * The following can be compiled as a separate 
 * module for more convenient debugging
 */
#if 0
void setphy(unsigned char reg_add, unsigned char value){}

#define PHY_CONFIG_FILE_PATH "/storage/sdcard0/phy_config"

static int phy_debug_init(void) 
{
      	printk("%s\n", __FUNCTION__);
	return set_phy_from_config_file(PHY_CONFIG_FILE_PATH);
}

static void phy_debug_exit(void)
{
      	printk("%s\n",__FUNCTION__);
}

module_init(phy_debug_init);
module_exit(phy_debug_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("tangshaoqing");
MODULE_DESCRIPTION("phy debug");
#endif
