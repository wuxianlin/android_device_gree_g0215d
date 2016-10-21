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

#ifndef ANDROID_INCLUDE_HARDWARE_REMOTEIR_H
#define ANDROID_INCLUDE_HARDWARE_REMOTEIR_H

#include <stdint.h>
#include <sys/cdefs.h>
#include <hardware/hardware.h>
#include <hardware/hwcomposer_defs.h>

#define REMOTEIR_MODULE_API_VERSION_1_0 		HARDWARE_MODULE_API_VERSION(1, 0)
#define REMOTEIR_HARDWARE_MODULE_ID 			"remoteir"
#define REMOTEIR_TRANSMITTER 				"remotetransmitter"


typedef struct remoteir_freq_range {
    int min;
    int max;
} remoteir_freq_range_t;


typedef struct remoteir_module {
    struct hw_module_t common;
} remoteir_module_t;


typedef struct remoteir_device {
    struct hw_device_t common;

    /*
     * (*transmit)() is called to by the ConsumerIrService to send an IR pattern
     * at a given carrier_freq.
     *
     * The pattern is alternating series of carrier on and off periods measured in
     * microseconds.  The carrier should be turned off at the end of a transmit
     * even if there are and odd number of entries in the pattern array.
     *
     * This call should return when the transmit is complete or encounters an error.
     *
     * returns: 0 on success. A negative error code on error.
     */
    int (*transmit)(struct remoteir_device *dev, const unsigned char *buf, int bufsize );
    int (*transmit_raw)(struct remoteir_device *dev, const unsigned char *buf, int bufsize );
	int (*cancel_transmit)(struct remoteir_device *dev);

    int (*receive_data)(struct remoteir_device *dev, unsigned char *buf, int bufsize );
    int (*receive_data_raw)(struct remoteir_device *dev, unsigned char *buf, int bufsize );

    int (*recv_init)(struct remoteir_device *dev);
    int (*recv_is_ready)(struct remoteir_device *dev);
    int (*recv_sync)(struct remoteir_device *dev, unsigned char *buf, int bufsize );
    int (*recv_async)(struct remoteir_device *dev, unsigned char *buf, int bufsize );

	/*
	 * I2C GPIO control via ioctl()
	 */
	int (*setmode)(struct remoteir_device *dev, int mode, int value );
	int (*getmode)(struct remoteir_device *dev);
	int (*setvalue)(struct remoteir_device *dev, int value );
	int (*getvalue)(struct remoteir_device *dev);
	int (*set_reset_mode)(struct remoteir_device *dev, int mode );
	int (*change_device_mode)(struct remoteir_device *dev, int mode );
	

    /*
     * (*get_num_carrier_freqs)() is called by the ConsumerIrService to get the
     * number of carrier freqs to allocate space for, which is then filled by
     * a subsequent call to (*get_carrier_freqs)().
     *
     * returns: the number of ranges on success. A negative error code on error.
     */
    int (*get_num_carrier_freqs)(struct remoteir_device *dev);


    /*
     * (*get_carrier_freqs)() is called by the ConsumerIrService to enumerate
     * which frequencies the IR transmitter supports.  The HAL implementation
     * should fill an array of consumerir_freq_range structs with the
     * appropriate values for the transmitter, up to len elements.
     *
     * returns: the number of ranges on success. A negative error code on error.
     */
    int (*get_carrier_freqs)(struct remoteir_device *dev,
            size_t len, remoteir_freq_range_t *ranges);


	int file_fd;
	
    /* Reserved for future use. Must be NULL. */
    void* reserved[8 - 3];
} remoteir_device_t;

#endif /* ANDROID_INCLUDE_HARDWARE_REMOTEIR_H */
