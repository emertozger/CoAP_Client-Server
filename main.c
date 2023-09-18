#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/net/openthread.h>
#include <openthread/thread.h>
#include <openthread/coap.h>

#define SLEEP_TIME 200
static char sendingPayload[200] = "Mert-Ozger-Send-Message-Every-200ms";
int i;


static void coap_send_data_req(void)
{
otError error = OT_ERROR_NONE;
otMessage *myMessage;
otMessageInfo myMessageInfo;
otInstance *myInstance = openthread_get_default_instance();

const otMeshLocalPrefix * ml_prefix = otThreadGetMeshLocalPrefix(myInstance);
uint8_t serverInterfaceID[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};

const char *myMessageJson = sendingPayload;

do
{
myMessage = otCoapNewMessage(myInstance, NULL);
otCoapMessageInit(myMessage, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_PUT);

error = otCoapMessageAppendUriPathOptions(myMessage,"storedata");
if(error != OT_ERROR_NONE){ break;}

error = otCoapMessageAppendContentFormatOption(myMessage,OT_COAP_OPTION_CONTENT_FORMAT_JSON);
if(error != OT_ERROR_NONE){break;}

error = otCoapMessageSetPayloadMarker(myMessage);
if(error != OT_ERROR_NONE){break;}

error = otMessageAppend(myMessage,myMessageJson,strlen(myMessageJson));
if(error != OT_ERROR_NONE){break;}

memset(&myMessageInfo,0,sizeof(myMessageInfo));
memcpy(&myMessageInfo.mPeerAddr.mFields.m8[0],ml_prefix,8);
memcpy(&myMessageInfo.mPeerAddr.mFields.m8[8],serverInterfaceID,8);
myMessageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

error = otCoapSendRequest(myInstance, myMessage, &myMessageInfo, coap_send_data_response_cb,NULL);
}while(false);

if(error != OT_ERROR_NONE)
 {
  printk("Failed to send CoAP Request\n");
   otMessageFree(myMessage);
 }
else
 {
  printk("CoAP data send.\n");
 }
}

static void coap_send_data_response_cb(void *p_context, otMessage *p_message, const otMessageInfo *p_message_info, otError result)
{
if ( result == OT_ERROR_NONE)
{
i++;
printk("Delivery confirmed. and Incoming packet number : %d \n",i);
printk("----------------------------------------------\n");
}
else
{
printk("Delivery not confirmed: %d\n\n",result);
}
}


void coap_init(void)
{
	otInstance *p_instance = openthread_get_default_instance();
	otError error = otCoapStart(p_instance,OT_DEFAULT_COAP_PORT);
}


void main(void)
{
	coap_init();

	while(1)
	{
		coap_send_data_req();
		k_msleep(SLEEP_TIME);
	}
}
