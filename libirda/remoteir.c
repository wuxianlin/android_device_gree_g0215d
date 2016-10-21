/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "RemoteIrHal"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cutils/log.h>
#include <hardware/hardware.h>
#include <hardware/remoteir.h>
#include <linux/i2c.h>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))



#define I2C_DEV_NAME	"/dev/remote_ctrl_dev"


#define RC_GPIO_MODE_INPUT			0
#define RC_GPIO_MODE_OUTPUT			1

/* 
 * Magic Number
 * ============
 * Refer to file ioctl-number.txt for allocation
 */
#define REMOTECTRL_IOCTL_MAGIC      		'r'

/* IOCTLs for remote-ctrl device */
#define REMOTE_CTRL_CTL_GET_GPIO_MODE		0x1000
#define REMOTE_CTRL_CTL_SET_GPIO_MODE		0x1001

#define REMOTE_CTRL_CTL_GET_GPIO_VALUE		0x1002
#define REMOTE_CTRL_CTL_SET_GPIO_VALUE		0x1003

#define REMOTE_CTRL_CTL_SET_RESET_VALUE		0x1004

#define REMOTE_CTRL_CTL_MODE_BOOTLOADER		_IOW(REMOTECTRL_IOCTL_MAGIC, 0x05, int)
#define REMOTE_CTRL_CTL_MODE_USERIR			_IOW(REMOTECTRL_IOCTL_MAGIC, 0x06, int)
#define REMOTE_CTRL_WRITE					_IOW(REMOTECTRL_IOCTL_MAGIC, 0x07, int)
#define REMOTE_CTRL_READ					_IOW(REMOTECTRL_IOCTL_MAGIC, 0x08, int)


 struct remote_msg
 {
 	   unsigned short u16Length;
 	   unsigned char* pu8Buf;
 };


static const remoteir_freq_range_t remoteir_freqs[] = {
    {.min = 30000, .max = 30000},
    {.min = 33000, .max = 33000},
    {.min = 36000, .max = 36000},
    {.min = 38000, .max = 38000},
    {.min = 40000, .max = 40000},
    {.min = 56000, .max = 56000},
};


static int remoteir_get_num_carrier_freqs(struct remoteir_device *dev)
{
    return ARRAY_SIZE(remoteir_freqs);
}

static int remoteir_get_carrier_freqs(struct remoteir_device *dev,
    size_t len, remoteir_freq_range_t *ranges)
{
    size_t to_copy = ARRAY_SIZE(remoteir_freqs);

    to_copy = len < to_copy ? len : to_copy;
    memcpy(ranges, remoteir_freqs, to_copy * sizeof(remoteir_freq_range_t));
    
    return to_copy;
}


/*
 * writeIR
 */
static int remoteir_transmit(struct remoteir_device *dev, const unsigned char *buf, int bufsize )
{
	int ret = 0;
	ALOGE("RemoteIrHal:remoteir_transmit called ");//xsd
	if( dev->file_fd <= 0 ) {
		
		return -2;
		}
	
	ret = write(dev->file_fd, buf, bufsize);
	if( ret < 0 ) {
		usleep(100 * 1000);
		ret = write(dev->file_fd, buf, bufsize);
		ALOGE("can not remoteir_transmit2 ");//xsd
	}
	ALOGE("RemoteIrHal:remoteir_transmit called ended");//xsd

    return ret;
}

static int remoteir_transmit_raw( struct remoteir_device *dev, const unsigned char *buf, int bufsize)
{
	
	//struct i2c_msg msg;
	struct remote_msg msg;
	int ret;
	ALOGE("RemoteIrHal:remoteir_transmit_raw called ");//xsd
	if( dev->file_fd <= 0 ) {
		ALOGE("can not remoteir_transmit_raw 1 ");//xsd
		return -2;
	}
   
    msg.u16Length = bufsize;
    msg.pu8Buf = buf;

    ret = ioctl(dev->file_fd, REMOTE_CTRL_WRITE, &msg );
	ALOGE("RemoteIrHal:remoteir_transmit_raw called ended");//xsd

	return ret;
}

/*
 * cancelTransmit
 */
static int remoteir_canceltransmit(struct remoteir_device *dev)
{
	unsigned char buf[4] = {0x69, 0x69, 0x96, 0x96};
    int ret = 0;
    int i;
    ALOGE("RemoteIrHal:remoteir_canceltransmit called ");//xsd

	if( dev->file_fd <= 0 ) {
		return -2;
	}
    
	ret = write(dev->file_fd, buf, 4);
	if( ret < 0 ) {
		usleep(100 * 1000);
		ret = write(dev->file_fd, buf, 4);
	}
	ALOGE("RemoteIrHal:remoteir_canceltransmit called ended ");//xsd

    return ret;
}


/*
 * readIR
 */
static int remoteir_receive_data(struct remoteir_device *dev, unsigned char *buf, int bufsize )
{
	int ret = 0;
	ALOGE("RemoteIrHal:remoteir_receive_data called ");//xsd
	if( dev->file_fd <= 0 ) {
		return -2;
	}
	
	ret = read(dev->file_fd, buf, bufsize);
	ALOGE("RemoteIrHal:remoteir_receive_data called ended");//xsd

    return ret;
}

static int remoteir_receive_data_raw( struct remoteir_device *dev, unsigned char *buf, int bufsize)
{
		//struct i2c_msg msg;
	struct remote_msg msg;
	int ret;
	ALOGE("RemoteIrHal:remoteir_receive_data_raw called ");//xsd
	if( dev->file_fd <= 0 ) {
		return -2;
	}
   
    msg.u16Length = bufsize;
    msg.pu8Buf = buf;

    ret = ioctl(dev->file_fd, REMOTE_CTRL_READ, &msg );
	ALOGE("RemoteIrHal:remoteir_receive_data_raw called ended");//xsd

	return ret;
}


/*
 * readIR
 */
static int remoteir_recvinit(struct remoteir_device *dev)
{
	unsigned char buf[4] = {0x11,0x11,0xee, 0xee};
    int ret = 0;
    int i;
    ALOGE("RemoteIrHal:remoteir_recvinit called ");//xsd

	if( dev->file_fd <= 0 ) {
		return -2;
	}
	
  
	ret = write(dev->file_fd, buf, 4);
	if( ret < 0 ) {
		usleep(100 * 1000);
		ret = write(dev->file_fd, buf, 4);
	}
	
	/* change mode to input mode */
	ioctl(dev->file_fd, REMOTE_CTRL_CTL_SET_GPIO_MODE, 0, 0);
	
	ALOGE("RemoteIrHal:remoteir_recvinit called ended");//xsd

    return ret;
}


static int remoteir_recv_is_ready( struct remoteir_device *dev )
{
    int ret = 0;
	ALOGE("RemoteIrHal:remoteir_recv_is_ready called ");//xsd

	if( dev->file_fd <= 0 ) {
		return -2;
	}

	/* data is ready to read if gpio value changed to 1 */    
	if( ioctl(dev->file_fd, REMOTE_CTRL_CTL_GET_GPIO_VALUE, 0) == 1 ) {
		ret = 1;
	}
	ALOGE("RemoteIrHal:remoteir_recv_is_ready called ended");//xsd

    return ret;
}

/*
 * readIR (Sync version) : this will block a process while reading job finished. max 8 sec.
 * 	return -3 if timed out.
 */
static int remoteir_receive_sync( struct remoteir_device *dev, unsigned char *buf, int bufsize )
{
    int i, ret = 0;
	ALOGE("RemoteIrHal:remoteir_receive_sync called ");//xsd

	if( dev->file_fd <= 0 ) {
		return -2;
	}
    
	/* wait until gpio value changes to 1 max 8 sec. */
	i = 0;
	while( i < 8000 ) {
		if( ioctl(dev->file_fd, REMOTE_CTRL_CTL_GET_GPIO_VALUE, 0) == 1 ) break;
		usleep(100 * 1000);
		i += 100;
	}

	if( i >= 8000 ) {
		ret = -3;	/* read timed out */
	}
	else {
		ret = read(dev->file_fd, buf, bufsize);
	}
	ALOGE("RemoteIrHal:remoteir_receive_sync called ended");//xsd

    return ret;
}


/*
 * readIR (Async version) : non-blocking version. return -3 if data not yet ready for reading.
 */

static int remoteir_receive_async( struct remoteir_device *dev, unsigned char *buf, int bufsize )
{
    int ret = 0;
	ALOGE("RemoteIrHal:remoteir_receive_async called ");//xsd

	if( dev->file_fd <= 0 ) {
		return -2;
	}

	if( ioctl(dev->file_fd, REMOTE_CTRL_CTL_GET_GPIO_VALUE, 0) != 1 ) {
		return -3;	/* data not ready */
	}
	
	ret = read(dev->file_fd, buf, bufsize);
	ALOGE("RemoteIrHal:remoteir_receive_async called ended");//xsd

    return ret;
}
/*
 * manipulate GPIO mode
 */
static int remoteir_setmode(struct remoteir_device *dev, int mode, int value )
{
	int ret;
	ALOGE("RemoteIrHal:remoteir_setmode called ");//xsd
	if( dev->file_fd <= 0 ) {
		return -2;
	}
	
	ret = ioctl(dev->file_fd, REMOTE_CTRL_CTL_SET_GPIO_MODE, mode, value);
	ALOGE("RemoteIrHal:remoteir_setmode called ended");//xsd
	return ret;
}
	
static int remoteir_getmode(struct remoteir_device *dev)
{
	int ret;
	ALOGE("RemoteIrHal:remoteir_getmode called ");//xsd

	if( dev->file_fd <= 0 ) {
		return -2;
	}
	
	ret = ioctl(dev->file_fd, REMOTE_CTRL_CTL_GET_GPIO_MODE, 0);
	ALOGE("RemoteIrHal:remoteir_getmode called ended");//xsd
	return ret;
}


/*
 * set or get GPIO's current status (value)
 */
static int remoteir_setvalue(struct remoteir_device *dev, int value )
{
	int ret;
	ALOGE("RemoteIrHal:remoteir_setvalue called ");//xsd

	if( dev->file_fd <= 0 ) {
		return -2;
	}
	
	ret = ioctl(dev->file_fd, REMOTE_CTRL_CTL_SET_GPIO_VALUE, value);
	ALOGE("RemoteIrHal:remoteir_setvalue called ended");//xsd
	return ret;
}
	
static int remoteir_getvalue(struct remoteir_device *dev)
{
	int ret;
	ALOGE("RemoteIrHal:remoteir_getvalue called ");//xsd

	if( dev->file_fd <= 0 ) {
		return -2;
	}
	
	ret = ioctl(dev->file_fd, REMOTE_CTRL_CTL_GET_GPIO_VALUE, 0);
	ALOGE("RemoteIrHal:remoteir_getvalue called ended");//xsd
	return ret;
}


static int remoteir_set_reset_mode(struct remoteir_device *dev, int mode)
{
	int ret;
	ALOGE("RemoteIrHal:remoteir_set_reset_mode called ");//xsd

	if( dev->file_fd <= 0 ) {
		return -2;
	}
	
	ret = ioctl(dev->file_fd, REMOTE_CTRL_CTL_SET_RESET_VALUE, mode);
	ALOGE("RemoteIrHal:remoteir_set_reset_mode called ended");//xsd
	return ret;
}


/* mode: zero for bootloader, non-zero for normal  */
static int remoteir_change_device_mode(struct remoteir_device *dev, int mode)
{
	int ret;
	ALOGE("RemoteIrHal:remoteir_change_device_mode called ");//xsd

	if( dev->file_fd <= 0 ) {
		return -2;
	}
	
	if( mode == 0 ) {
		mode = REMOTE_CTRL_CTL_MODE_BOOTLOADER;
	}
	else {
		mode = REMOTE_CTRL_CTL_MODE_USERIR;
	}

	ret = ioctl(dev->file_fd, mode, 0);
	ALOGE("RemoteIrHal:remoteir_change_device_mode called ended");//xsd
	return ret;
}



/*
 * Generic device handling
 */
static int remoteir_close(hw_device_t *dev)
{
	remoteir_device_t *ir_dev = (remoteir_device_t *)dev;
	ALOGE("RemoteIrHal:remoteir_close called ");//xsd
	if(ir_dev->file_fd != 0 ) {
		close(ir_dev->file_fd);
	}
	
    free(dev);
	ALOGE("RemoteIrHal:remoteir_close called ended");//xsd
    return 0;
}

 
static int remoteir_open(const hw_module_t* module, const char* name,
        hw_device_t** device)
{
	ALOGE("RemoteIrHal:remoteir_open called ");//xsd

    if (strcmp(name, REMOTEIR_TRANSMITTER) != 0) {
		ALOGE("can not remoteir_open 1 ");//xsd
		return -EINVAL;
    }
    if (device == NULL) {
        ALOGE("NULL device on open");
		ALOGE("can not remoteir_open 2");//xsd
        return -EINVAL;
    }

    remoteir_device_t *dev = malloc(sizeof(remoteir_device_t));
    memset(dev, 0, sizeof(remoteir_device_t));

    dev->common.tag 	= HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module 	= (struct hw_module_t*) module;
    dev->common.close 	= remoteir_close;

    dev->transmit 		= remoteir_transmit;
    dev->transmit_raw 	= remoteir_transmit_raw;
    dev->cancel_transmit = remoteir_canceltransmit;
    dev->receive_data	= remoteir_receive_data;
    dev->receive_data_raw = remoteir_receive_data_raw;
    dev->recv_init  	= remoteir_recvinit;
    dev->recv_is_ready	= remoteir_recv_is_ready;
    dev->recv_sync		= remoteir_receive_sync;
    dev->recv_async		= remoteir_receive_async;
    dev->setmode		= remoteir_setmode;
    dev->getmode		= remoteir_getmode;
    dev->setvalue		= remoteir_setvalue;
    dev->getvalue		= remoteir_getvalue;
    dev->set_reset_mode	= remoteir_set_reset_mode;
    dev->change_device_mode	= remoteir_change_device_mode;
    
    dev->get_num_carrier_freqs 	= remoteir_get_num_carrier_freqs;
    dev->get_carrier_freqs 		= remoteir_get_carrier_freqs;

	dev->file_fd		= open(I2C_DEV_NAME, O_RDWR, 0660 );
	if(dev->file_fd<0)
		{
			  	ALOGE("RemoteIrHal:remoteir_open Failed");//xsd
			  	return dev->file_fd;
		}
	
    *device = (hw_device_t*) dev;
	ALOGE("RemoteIrHal:remoteir_open called ended");//xsd
    return 0;
}

static struct hw_module_methods_t remoteir_module_methods = {
    .open = remoteir_open,
};

remoteir_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag                = HARDWARE_MODULE_TAG,
        .module_api_version = REMOTEIR_MODULE_API_VERSION_1_0,
        .hal_api_version    = HARDWARE_HAL_API_VERSION,
        .id                 = REMOTEIR_HARDWARE_MODULE_ID,
        .name               = "Remote IR HAL",
        .author             = "The Android Open Source Project",
        .methods            = &remoteir_module_methods,
    },
};
