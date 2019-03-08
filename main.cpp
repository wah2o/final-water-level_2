#include "MQTTTimer.h"
#include "CayenneMQTTClient.h"
#include "MQTTNetworkIDW01M1.h"
#include "mbed.h"
#include "lorawan_network.h"
#include "CayenneLPP.h"
#include "mbed_events.h"
#include "mbed_trace.h"
#include "lora_radio_helper.h"
#include "SX1276_LoRaRadio.h"
#include "LoRaWANInterface.h"
#include "hcsr04.h"
#include "standby.h"

#define STANDBY_TIME_S                 1* 60

extern EventQueue ev_queue;

static uint32_t DEV_ADDR_1   =      0x260118FA;
static uint8_t NWK_S_KEY_1[] =      { 0x5D, 0xD5, 0xC9, 0x84, 0xB9, 0xCE, 0x26, 0x05, 0xB7, 0x61, 0xBF, 0x9F, 0x3C, 0x2D, 0x4E, 0x14 };
static uint8_t APP_S_KEY_1[] =      { 0xF0, 0xE4, 0xA1, 0xEB, 0x1F, 0xB5, 0x6E, 0xBA, 0x31, 0x69, 0x80, 0x9D, 0x85, 0x92, 0x68, 0xB4 };

//static uint32_t DEV_ADDR_2   =      0x2601114C;
//static uint8_t NWK_S_KEY_2[] =      { 0xDB, 0xC0, 0xD7, 0x32, 0xF8, 0x41, 0xE7, 0xF5, 0xB9, 0xCB, 0x54, 0x6F, 0x49, 0xD5, 0x48, 0x25 };
//static uint8_t APP_S_KEY_2[] =      { 0x64, 0x6C, 0xF0, 0xA4, 0x9C, 0x6F, 0x30, 0xF4, 0x0C, 0x80, 0x3E, 0x4A, 0x50, 0xC3, 0x77, 0x53 };
HCSR04 sensor(D4, D3);
//DigitalOut led3(LED3);

// WiFi network info.
//char* ssid = "Jeff_HomeAP_2EX";
//char* wifiPassword = "iots2480";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
//char* username = "0cb35a60-7fbc-11e8-890a-c1153a0b021e";
//char* password = "8f7cb3b94334a5a0a5cbe11b068f4630b6c8d478";
//char* clientID = "d8504b50-b0ff-11e8-85ea-f10189fd2756";


  //global variable
long distance; 
bool dist_updated = false;
float volume;

//void dust_sensor_cb(int lpo, float ratio, float concentration) {
 //   dust_concentration = concentration;
 //   dust_updated = true;
//}

void dist_measure(){
    sensor.start();
    wait_ms(100); 
    distance=sensor.get_dist_cm();
    volume = 6*6*(2-(distance*0.01));
    dist_updated = true;
    
    }
    
void check_for_updated_dist() {
    if (dist_updated){
        dist_updated = false ;
        printf("Measure Distance = %ld ",distance);
        printf("Volume=%3.0f\n", volume);
        
        CayenneLPP payload(50);
        float volume_value = volume;
        printf("Volume_value=%f\n", volume_value);
        payload.addAnalogInput(4, volume_value);
        
        if (!lorawan_send(&payload)){
           // delete distance;
            standby(STANDBY_TIME_S);
            }
        }
    }

static void lora_event_handler(lorawan_event_t event) {
    switch (event) {
        case CONNECTED:
            printf("[LNWK][INFO] Connection - Successful\n");
            break;
        case DISCONNECTED:
            ev_queue.break_dispatch();
            printf("[LNWK][INFO] Disconnected Successfully\n");
            break;
        case TX_DONE:
            printf("[LNWK][INFO] Message Sent to Network Server\n");
            
         //   delete distance;
            standby(STANDBY_TIME_S);
            break;
        case TX_TIMEOUT:
        case TX_ERROR:
        case TX_CRYPTO_ERROR:
        case TX_SCHEDULING_ERROR:
            printf("[LNWK][INFO] Transmission Error - EventCode = %d\n", event);
             

            //delete distance;
            standby(STANDBY_TIME_S);
            break;
        case RX_DONE:
            printf("[LNWK][INFO] Received message from Network Server\n");
            receive_message();
            break;
        case RX_TIMEOUT:
        case RX_ERROR:
            printf("[LNWK][INFO] Error in reception - Code = %d\n", event);
            break;
        case JOIN_FAILURE:
            printf("[LNWK][INFO] ABP Failed - Check Keys\n");
            break;
        default:
            MBED_ASSERT("Unknown Event");
    }
}


int main() {
    set_time(0);
    
    printf("=========================================\n");
    printf("      Water Level Monitoring System        \n");
    printf("=========================================\n");

    lorawan_setup(DEV_ADDR_1, NWK_S_KEY_1, APP_S_KEY_1, lora_event_handler);

    printf("Measuring Distance...\n");
    
    //immediately measure the distance
     sensor.start();
     wait_ms(100);
     distance = sensor.get_dist_cm();
    printf("Measuring Dist =%ld...\n",distance);
      dist_measure();
     ev_queue.call_every(3000, &check_for_updated_dist);   
     

    ev_queue.dispatch_forever();
}



