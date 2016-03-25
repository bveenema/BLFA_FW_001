/**
* @file NexSleep.h
*
* The definition of class NexSleep.
*
* @author Ben Veenema (email:<bveenema@gmail.com>)
* @date 2016/3/25
*
* Credit to:
* @author Wu Pengfei (email:<pengfei.wu@itead.cc>)
* @date 2015/8/13
*
* @copyright
* Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* Port for Particle platform (particle.io)
* by BSpranger & ScruffR (Dec. 2015)
*/

#ifndef __NEXSLEEP_H__
#define __NEXSLEEP_H__

#include "NexTouch.h"
#include "NexHardware.h"
/**
 * @addtogroup Component
 * @{
 */

/**
 * NexHotspot component.
 */
class NexSleep: public NexTouch
{
public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, const char *name, void *value);
     */
    NexSleep(uint8_t pid, uint8_t cid, const char *name, void *value = NULL);
};
/**
 * @}
 */


#endif /* #ifndef __NEXHOTSPOT_H__ */
