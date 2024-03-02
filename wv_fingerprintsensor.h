#include "esphome.h"

class wvfpsensor : public Component, public UARTDevice
{
public:
      wvfpsensor(UARTComponent *parent) : UARTDevice(parent) {}
      
      void setup() override{}
      void loop() override {}


      void clearbuffer(){ //allow enable state 
            id(enable).turn_on();
            delay(500);
            std::vector<uint8_t> rb;
            while (available()>0){
            rb.push_back(read());
            }
            rb.clear();
      }

      void checkfp(){ //check fingerprint
            clearbuffer();
            std::vector<uint8_t> cmd = {0xF5, 0x0C, 00, 00, 00, 00, 0x0C, 0xF5};
            std::vector<uint8_t> rb;
            write_array(cmd);
            delay(500);
            while (available() > 0){
            rb.push_back(read());
            }
            if (rb[4]==1){
                  uint8_t high = rb[2];
                  uint8_t low = rb[3];
                  uint16_t combine = (high << 8) | low;
                  int users = combine;                  
                  ESP_LOGI("WV sensor", "Matched: %i", users);
                  id(door_switch).turn_on();
                  }
            rb.clear();
            id(enable).turn_off();
      }

      void querynousers(){     //query no of users
            clearbuffer();
            std::vector<uint8_t> cmd = {0xF5, 0x09, 00, 00, 00, 00, 0x09, 0xF5};
            std::vector<uint8_t> rb;
            write_array(cmd);
            delay(30);
            while (available()>0){
            rb.push_back(read());
            }
            if (rb[4]== 0x00){
                  ESP_LOGI("WV sensor", "ACK_SUCCESS");
                  uint8_t high = rb[2];
                  uint8_t low = rb[3];
                  uint16_t combine = (high << 8) | low;
                  int users = combine;
                  ESP_LOGI("WV sensor", "No of users: %i", users);
            }
            rb.clear();
            id(enable).turn_off();
      }

      void queryslno(){ //check serial no
            clearbuffer();
            std::vector<uint8_t> rb;
            std::vector<uint8_t> cmd =  {0xF5, 0x2A, 00, 00, 00, 00, 0x2A, 0xF5};
            write_array(cmd);
            delay(30);
            while (available()>0){
            rb.push_back(read());
            }
            std::string hexstring{rb.begin(), rb.end()};
                  int slno = (rb[2]<<16|rb[3]<<8|rb[4]);
                  ESP_LOGI ("WV sensor", "int = %i", slno);
            rb.clear();
            id(enable).turn_off();
      }

      void deluser(int userno){ //delete user
            clearbuffer();
            uint8_t low= userno & 0xff;
            uint8_t high=(userno>>8) & 0xff;
            char chk1 = 0x04 ^ high ^ low ^ 4 ^ 0;
            std::vector<uint8_t> cmd1 = {0xF5, 0x04, high, low, 0, 0, chk1, 0xF5};
            write_array(cmd1);
            std::vector<uint8_t> rb;
            while (available()>0){
            rb.push_back(read());
            }
            rb.clear();
            id(enable).turn_off();
      }

      void adduser(int userno){ //adduser
            clearbuffer();
            uint8_t low= userno & 0xff;
            uint8_t high=(userno>>8) & 0xff;
            char chk1 = 0x01 ^ high ^ low ^ 1 ^ 0;
            std::vector<uint8_t> cmd1 = {0xF5, 0x01, high, low, 1, 0, chk1, 0xF5};
            write_array(cmd1);
            delay(500);
            char chk2 = 0x02 ^ high ^ low ^ 1 ^ 0;
            std::vector<uint8_t> cmd2 = {0xF5, 0x02, high, low, 1, 0, chk2, 0xF5};
            write_array(cmd2);
            delay(1000);
            char chk3 = 0x03 ^ high ^ low ^ 1 ^ 0;
            std::vector<uint8_t> cmd3 = {0xF5, 0x03, high, low, 1, 0, chk3, 0xF5};
            write_array(cmd3);
            id(enable).turn_off();
      }
  
      void serno(int slno){ //set serial no
            clearbuffer();
            uint8_t first = (slno >> 16) & 0xFF;
            uint8_t second = (slno >> 8 ) & 0xFF;
            uint8_t third = slno & 0xFF;
            char chk = 0x08 ^ first ^ second ^ third ^ 0;
            std::vector<uint8_t> cmd =  {0xF5, 0x08, first, second , third , 00, chk, 0xF5};
            write_array(cmd);
            }

};

wvfpsensor * wfvpobj;
