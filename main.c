#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <openthread/coap.h>


#define SLEEP_TIME_MS 1000
#define TEXTBUFFER_SIZE 30
char myText[TEXTBUFFER_SIZE];
uint16_t myText_length = 0;
uint16_t t[1] = {0,0};
uint16_t timeDif = 0;
int i;


static void storedata_request_cb(void *p_context, otMessage *p_message, const otMessageInfo *p_message_info);
static void storedata_request_send(otMessage *p_request_message, const otMessageInfo *p_message_info);


static otCoapResource m_storedata_resource = 
{
.mUriPath = "storedata",
.mHandler = storedata_request_cb,
.mContext = NULL,
.mNext = NULL
};


static void storedata_request_send(otMessage *p_request_message, const otMessageInfo *p_message_info)
{
	otInstance *p_instance = openthread_get_default_instance();
	otMessage *p_response ;
	p_response = otCoapNewMessage(p_instance,NULL);
	otError error;
	
	do{
		error = otCoapMessageInitResponse(p_response,p_request_message,OT_COAP_TYPE_ACKNOWLEDGMENT,OT_COAP_CODE_CHANGED);
		if (error != OT_ERROR_NONE){break;}
		error = otCoapSendResponse(p_instance, p_response, p_message_info);
	}
	while(false);
}



static void storedata_request_cb(void *p_context, otMessage *p_message, const otMessageInfo *p_message_info)
{
t[1] = k_uptime_get();
timeDif = t[1] - t[0];
t[0] = t[1];
i++;
printk("Time Differences : %d ms\n",timeDif);
printk("Replied packet number: %d\n",i);
printk("----------------------------------------------\n");

otCoapCode messageCode = otCoapMessageGetCode(p_message);
otCoapType messageType = otCoapMessageGetType(p_message);

do
{
if(messageType != OT_COAP_TYPE_CONFIRMABLE && messageType != OT_COAP_TYPE_NON_CONFIRMABLE)
{
break;
}
myText_length = otMessageRead(p_message, otMessageGetOffset(p_message), myText, TEXTBUFFER_SIZE -1);
myText[myText_length] = '\0';
printk("%s\n",myText);

if(messageType == OT_COAP_TYPE_CONFIRMABLE)
{
storedata_request_send(p_message,p_message_info);
}
}while(false);

}


void coap_init(void)
{
	otInstance *p_instance = openthread_get_default_instance();
	otError error = otCoapStart(p_instance,OT_DEFAULT_COAP_PORT);
	
	otCoapAddResource(p_instance,&m_storedata_resource);
}


void addIpv6Address(void)
{
otInstance *myInstance = openthread_get_default_instance();
otNetifAddress aAddress;
const otMeshLocalPrefix *ml_prefix = otThreadGetMeshLocalPrefix(myInstance);
uint8_t interfaceID[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};

memcpy(&aAddress.mAddress.mFields.m8[0], ml_prefix,8);
memcpy(&aAddress.mAddress.mFields.m8[8], interfaceID,8);
otError error = otIp6AddUnicastAddress(myInstance, &aAddress);
}


void main(void)
{
addIpv6Address();
coap_init();

while(1)
{
k_msleep(SLEEP_TIME_MS);
}
}