/* 
 * File:   ROSTasks.h
 * Author: Quang
 *
 * Created on April 4, 2014, 2:57 PM
 */

#ifndef ROSTASKS_H
#define	ROSTASKS_H

extern xQueueHandle LCDDisplayinfo;
extern char RTCFlag;

static void ButtonPushCounter( void *pvParameters);
void WriteLCD(void *pvParameters);
void RTCTimer(void *pvParameters);
void vApplicationIdleHook( void);

#endif	/* ROSTASKS_H */

