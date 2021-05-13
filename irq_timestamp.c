/*                                   80                                       */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/ktime.h>

#include "irq_timestamp.h"

// Arguments du module

static long time_before_logout = 0;
module_param(time_before_logout, long, S_IWUSR | S_IRUGO);
MODULE_PARM_DESC(time_before_logout, "Time without interaction before logout.");

#define NB_ELEM_READ_DEV	5

static struct timeval start;
static int major;
static int minor;

static struct input_user data = {
	.last_message = 0,
	.last_login = 0,
	.nb_msg = 0,
	.header = 0,
	.keystroke_since_last_login = 0,
	.nb_session = 0
};


struct class *class_output;
dev_t devno;

ssize_t output_dev_read(struct file *filp, char __user *buf,
			size_t count, loff_t *f_pos)
{
	char *msg = (char *)kzalloc(
		(data.nb_msg + NB_ELEM_READ_DEV + data.nb_session) * (SIZE_MAX_MSG + 1) 
		* sizeof(char), GFP_KERNEL);
	char tmp[SIZE_MAX_MSG];
	int size = 0;
	int i = 0;

	if (data.nb_msg == 0) {
		return 0;
	}

	size = write_user_data(msg, size);
	snprintf(tmp, SIZE_MAX_MSG, "Time of last %d keystrokes :\n", data.nb_msg);
	strcpy(msg + size, tmp);
	size += strlen(tmp);

	for (i = 0; i < data.nb_msg; ++i) {
		snprintf(tmp, SIZE_MAX_MSG, "%d (%s) : %s", (i+1),
			data.keystrokes_scancode[(data.header + i) % COUNT_MAX_MSG],
			get_time_since(data.keystrokes_time[(data.header + i) % COUNT_MAX_MSG],
				true));
		strcpy(msg + size, tmp);
		size += strlen(tmp);
		strcpy(msg + size, "\n");
		size += 1;
	}
	if (copy_to_user(buf, msg, size)) {
		return -EFAULT;
	}
	data.nb_msg = 0;
	data.header = 0;
	return size;
}

int write_user_data(char *buf, int size) 
{
	int index = size;
	char number[SIZE_MAX_MSG];
	char *val;
	int i;

	char messages[NB_ELEM_READ_DEV][SIZE_MAX_MSG] = {
		"\n=========== SESSION ===========\n",
		"Last keystroke : ",
		"Number of session : ",
		"\n=========== KEYSTROKE ===========\n",
		"Number of keystrokes since last login (approximately) : "
	};

	long data_messages[NB_ELEM_READ_DEV] = {
		-1,
		data.last_message,
		data.nb_session,
		-1,
		data.keystroke_since_last_login
	};

	bool get_time[NB_ELEM_READ_DEV] = {
		false,
		true,
		false,
		false,
		false
	};

	int (*function_message[5])(char *, int) = {
		NULL,
		NULL,
		write_session,
		NULL,
		NULL
	};

	for (i = 0; i < NB_ELEM_READ_DEV; ++i) {
		strcpy(buf + index, messages[i]);
		index += strlen(messages[i]);
		if (data_messages[i] != -1) {
			if (get_time[i]) {
				val = get_time_since(data_messages[i], true);
				strcpy(number, val);
				kfree(val);
			} else {
				snprintf(number, SIZE_MAX_MSG, "%lu", data_messages[i]);
			}
			strcpy(buf + index, number);
			index += strlen(number);
		}
		strcpy(buf + index, "\n");
		index += 1;
		if (function_message[i] != NULL) {
			index = function_message[i](buf, index);
		}
	}
	return index;
}

int write_session(char *buf, int size) 
{
	char message[SIZE_MAX_MSG];
	int index = size;
	int number = 0;
	struct session_user *elem = data.head_session;

	while (elem != NULL) {
		number += 1;
		if (elem->logout == -1) {
			snprintf(message, SIZE_MAX_MSG, "\tcurrent session : started %s\n",
				get_time_since(elem->login, true));
			strcpy(buf + index, message);
			index += strlen(message);
		} else {
			snprintf(message, SIZE_MAX_MSG, 
				"\tsession %d (%s): started %s, finished %s\n",
				number,
				get_time_since((elem->logout - elem->login), false),
				get_time_since(elem->login, true),
				get_time_since(elem->logout, true));
			strcpy(buf + index, message);
			index += strlen(message);
		}
		elem = elem->next;
	}
	return index;
}

bool new_irq_after_login(const long time_message) 
{
	if (data.last_message == 0) {
		return true;
	}
	return (time_message >= data.last_message + time_before_logout ? 
		true : false);
}

char *get_time_since(long sec_to_check, bool print_ago)
{
	struct timeval time;
	
	long total_sec;
	char *val = (char *)kzalloc(SIZE_MAX_MSG * sizeof(char), GFP_KERNEL);
	
	do_gettimeofday(&time);
	total_sec = (print_ago ? 
		(long)(time.tv_sec - start.tv_sec) - sec_to_check : sec_to_check);
	if (total_sec > (S_FROM_M * M_FROM_H * H_FROM_D)) {
		snprintf(val, SIZE_MAX_MSG, "%d day, %d h %d min %d sec%s", 
			(int)(total_sec / (S_FROM_M * M_FROM_H * H_FROM_D)),
			(int)((total_sec / (S_FROM_M * M_FROM_H)) % H_FROM_D),
			(int)((total_sec / S_FROM_M) % M_FROM_H), 
			(int)(total_sec % S_FROM_M),
			(print_ago ? " ago" : ""));
	} else if (total_sec > (S_FROM_M * M_FROM_H)) {
		snprintf(val, SIZE_MAX_MSG, "%d h %d min %d sec%s",
			(int)((total_sec / (S_FROM_M * M_FROM_H)) % H_FROM_D),
			(int)((total_sec / S_FROM_M) % M_FROM_H), 
			(int)(total_sec % S_FROM_M),
			(print_ago ? " ago" : ""));
	} else if (total_sec > S_FROM_M) {
		snprintf(val, SIZE_MAX_MSG, "%d min %d sec%s",
			(int)((total_sec / S_FROM_M) % M_FROM_H), 
			(int)(total_sec % S_FROM_M),
			(print_ago ? " ago" : ""));
	} else {
		snprintf(val, SIZE_MAX_MSG, "%d sec%s", 
			(int)(total_sec % S_FROM_M),
			(print_ago ? " ago" : ""));
	}
	return val;
}

struct file_operations fops = {
	.owner = THIS_MODULE,
	.read  = output_dev_read,
};

static irqreturn_t irq_handler(int irq, void *dev)
{
	struct timeval time;
	struct session_user *elem;
        char scancode;

	scancode = inb(KBD_DATA_REG);
	
	if (!(scancode & KBD_STATUS_MASK)) {
		return IRQ_NONE;
	}
	pr_info("irq : %x\n", scancode & KBD_SCANCODE_MASK);
	do_gettimeofday(&time);
	if (new_irq_after_login((const long)(time.tv_sec - start.tv_sec))) {
		elem = (struct session_user *)kzalloc(
			sizeof(struct session_user), GFP_KERNEL);
		if (elem == NULL) {
			return IRQ_NONE;	
		}
		elem->login = (long)(time.tv_sec - start.tv_sec);
		elem->logout = -1;
		elem->next = NULL;
		if (data.head_session == NULL) {
			data.head_session = elem;
			data.tail_session = elem;
		} else {
			data.tail_session->logout = (long)(time.tv_sec - start.tv_sec);
			data.tail_session->next = elem;
			data.tail_session = elem;
		}
		data.nb_session += 1;
		data.last_login = (long)(time.tv_sec - start.tv_sec);
		data.keystroke_since_last_login = 0;
	}

	strcpy((char *)data.keystrokes_scancode +
		((data.header + data.nb_msg) % COUNT_MAX_MSG) * SIZE_MAX_MSG,
		(const char *)codekeystroke[scancode & KBD_SCANCODE_MASK]);
	
	data.keystrokes_time[(data.header + data.nb_msg) % COUNT_MAX_MSG] =
		(long)(time.tv_sec - start.tv_sec);
	data.last_message = (long)(time.tv_sec - start.tv_sec);
	data.keystroke_since_last_login += 1;
	if (data.nb_msg < COUNT_MAX_MSG) {
		data.nb_msg += 1;
		
	} else {
		data.header = (data.header + 1) % COUNT_MAX_MSG;
	}
	return IRQ_NONE;
}

static int __init fun_mod_init(void)
{
  	int err;
  	static struct device *dev_output;

  	do_gettimeofday(&start);
	pr_info("%s : Starting irq keylogger timestamp\n", MODULE_NAME);
	if (request_irq(IRQ_WATCHING, irq_handler, IRQF_SHARED,
			"test_rtc_irq", irq_handler)) {
		pr_err("%s : Cannot register IRQ %d\n", MODULE_NAME, IRQ_WATCHING);
		return -EIO;
	}

  	if ((major = register_chrdev(0, CHAR_DEV_RANGE_DEVICES_NAME, &fops)) < 0) {
    		err = major;
    		goto err2;
  	}

  	class_output = class_create(THIS_MODULE, CHAR_DEV_CLASS_NAME);
  	if (IS_ERR(class_output)) {
    		err = PTR_ERR(class_output);
   		goto err1;
  	}

  	devno = MKDEV(major, minor);
  	dev_output = device_create(class_output, NULL, devno, NULL,
			     CHAR_DEV_DEVICE_NAME);
  	if (IS_ERR(dev_output)) {
    		err = PTR_ERR(dev_output);
    		goto err0;
	}
	return 0;


err0:
 	class_destroy(class_output);
err1:
  	unregister_chrdev(major, CHAR_DEV_RANGE_DEVICES_NAME);
err2:
  	return err;
}
module_init(fun_mod_init);

static void __exit fun_mod_exit(void)
{
	pr_info("%s : Shutdown irq keylogger timestamp\n", MODULE_NAME);
	disable_irq(IRQ_WATCHING);
	free_irq(IRQ_WATCHING, irq_handler);
	enable_irq(IRQ_WATCHING);

 	device_destroy(class_output, devno);
  	class_destroy(class_output);
  	unregister_chrdev(major, CHAR_DEV_RANGE_DEVICES_NAME);
}
module_exit(fun_mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
