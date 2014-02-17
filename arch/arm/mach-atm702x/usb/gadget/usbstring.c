/*
 * Copyright (C) 2003 David Brownell
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 */

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/nls.h>
#include <linux/fs.h>

#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>

#include <asm/uaccess.h>

#define UNIFIED_ASCII_SERIALNUM         0     //
#define UNIFIED_UNICODE_SERIALNUM       1     //统一unicode码
#define NOUNIFIED_UNICODE_SERIALNUM    2     //非统一unicode码
#define NOUNIFIED_ASCII_SERIALNUM      3     //

char usbserialnumber[33];

int usbgadget_get_serialnumber(void);

extern int get_config(const char *key, char *buff, int len);

int usbgadget_get_serialnumber(void)
{
  struct file *filp = NULL;
  mm_segment_t old_fs;
  loff_t file_offset =0;
  int	length;
  //unsigned long copylen;
  //char __user serial[33]; 
  
  /*get usb serialnumber from */
  filp = filp_open("/data/usb_serialnumber", O_RDONLY, 0);
  if(IS_ERR(filp)) {
    printk("open usb_serialnumber ERR\n");
    return PTR_ERR(filp);
  }
  
  old_fs = get_fs();
  set_fs(KERNEL_DS);
  length = vfs_read(filp, usbserialnumber, 32, &file_offset);
  set_fs(old_fs);
  
  filp_close(filp, NULL);
  
  //copylen = copy_from_user(usbserialnumber, serial, length);
  usbserialnumber[32] = '\0';

  return 0;
}




/**
 * usb_gadget_get_string - fill out a string descriptor 
 * @table: of c strings encoded using UTF-8
 * @id: string id, from low byte of wValue in get string descriptor
 * @buf: at least 256 bytes, must be 16-bit aligned
 *
 * Finds the UTF-8 string matching the ID, and converts it into a
 * string descriptor in utf16-le.
 * Returns length of descriptor (always even) or negative errno
 *
 * If your driver needs stings in multiple languages, you'll probably
 * "switch (wIndex) { ... }"  in your ep0 string descriptor logic,
 * using this routine after choosing which set of UTF-8 strings to use.
 * Note that US-ASCII is a strict subset of UTF-8; any string bytes with
 * the eighth bit set will be multibyte UTF-8 characters, not ISO-8859/1
 * characters (which are also widely used in C strings).
 */
//int
//usb_gadget_get_string (struct usb_gadget_strings *table, int id, u8 *buf)
//{
//	struct usb_string	*s;
//	int			len,value;
//	char key[32];
//
//	/* descriptor 0 has the language id */
//	if (id == 0) {
//		buf [0] = 4;
//		buf [1] = USB_DT_STRING;
//		buf [2] = (u8) table->language;
//		buf [3] = (u8) (table->language >> 8);
//		return 4;
//	}
//	for (s = table->strings; s && s->s; s++)
//		if (s->id == id)
//			break;
//
//	/* unrecognized: stall. */
//	if (!s || !s->s)
//		return -EINVAL;
//
//	/* string descriptors have length, tag, then UTF16-LE text */
//	len = min ((size_t) 126, strlen (s->s));
//	if(id == 4) {
//	  sprintf(key, "usb.%s", "cfgf_serialnum");
//	  if(get_config(key, (char *)&value, 4) == 0) {
//	    printk("usb serialnumber config is %d\n", value);
//	    if(value == 0) {
//	      strncpy(&buf[2], s->s, len);
//	      for(; len < 32; len++)
//	        buf[len] = 0x2e;
//        buf [0] = 32;
//	      goto _get_s;
//      }else{
//	        //
//	     }
//	   }
//	 }	
//	len = utf8s_to_utf16s(s->s, len, UTF16_LITTLE_ENDIAN,
//			(wchar_t *) &buf[2], 126);
//	if (len < 0)
//		return -EINVAL;
//	buf [0] = (len + 1) * 2;
//_get_s:	
//	buf [1] = USB_DT_STRING;
//	return buf [0];
//}

int
usb_gadget_get_string (struct usb_gadget_strings *table, int id, u8 *buf)
{
	struct usb_string	*s;
	int	len, value;
	char key[33];

	/* descriptor 0 has the language id */
	if (id == 0) {
		buf [0] = 4;
		buf [1] = USB_DT_STRING;
		buf [2] = (u8) table->language;
		buf [3] = (u8) (table->language >> 8);
		return 4;
	}
	for (s = table->strings; s && s->s; s++)
		if (s->id == id)
			break;

	/* unrecognized: stall. */
	if (!s || !s->s)
		return -EINVAL;

	/* string descriptors have length, tag, then UTF16-LE text */
	if(id == 4) {	  
	  sprintf(key, "usb.%s", "cfgf_serialnum");
	  get_config(key, (char *)&value, 4);
    printk("usb serialnumber config is %d\n", value);
	  switch(value) {      
      case UNIFIED_UNICODE_SERIALNUM:
        len = min ((size_t) 126, strlen (s->s));
  	    len = utf8s_to_utf16s(s->s, len, UTF16_LITTLE_ENDIAN,
            (wchar_t *) &buf[2], 126);
  	    if (len < 0)
  		    return -EINVAL;
  	    buf [0] = (len + 1) * 2;
  	    break;
  	    
      case NOUNIFIED_UNICODE_SERIALNUM:
      /*不能在这添加,中断过程不能调用引起睡眠的函数*/
//        filp = filp_open("/data/usb_serialnumber", O_RDONLY, 0);
//        if(IS_ERR(filp))
//		      return PTR_ERR(filp);
//		    len = vfs_read(filp, (char __user*)key, 16, &file_offset);
//        key[len] = '\0';
//        filp_close(filp, NULL);
        len = sizeof(usbserialnumber) / 2;
        memcpy(key, usbserialnumber, len);
        key[len] = '\0';
        len = utf8s_to_utf16s(usbserialnumber, len, UTF16_LITTLE_ENDIAN,
          (wchar_t *) &buf[2], 126);
  	    if (len < 0)
  		    return -EINVAL;
  	    buf [0] = (len + 1) * 2;
  	    break;
  	   case NOUNIFIED_ASCII_SERIALNUM:
  	   /*不能在这添加,中断过程不能调用引起睡眠的函数*/
//  	    filp = filp_open("/data/usb_serialnumber", O_RDONLY, 0);
//        if(IS_ERR(filp))
//		      return PTR_ERR(filp);
//  	    len = vfs_read(filp, (char __user*)key, 32, &file_offset);
//  	    key[len] = '\0';
//        filp_close(filp, NULL);
        
        /* 下面代码有问题,暂不支持,无意义 */
        /*
        len = sizeof(usbserialnumber) - 1 ;
        memcpy(key, usbserialnumber, len);
        key[len] = '\0';
  	    strncpy(&buf[2], usbserialnumber, len);
  	    buf [0] = 32;
        break;
        */
        
  	  case UNIFIED_ASCII_SERIALNUM:
      default:
        len = min ((size_t) 126, strlen (s->s));
        strncpy(&buf[2], s->s, len);
        for(; len < 32; len++)
          buf[len] = 0x2e;
        buf [0] = 32;
       break;        

      }
    }else {
      len = min ((size_t) 126, strlen (s->s));
      len = utf8s_to_utf16s(s->s, len, UTF16_LITTLE_ENDIAN,
          (wchar_t *) &buf[2], 126);
      if (len < 0)
        return -EINVAL;
      buf [0] = (len + 1) * 2;
  }
  
	buf [1] = USB_DT_STRING;
	return buf [0];
}

