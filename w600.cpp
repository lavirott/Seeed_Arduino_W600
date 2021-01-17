/*
 * seeed_w600.cpp
 *
 * Copyright (c) 2018 seeed technology inc.
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <inttypes.h>

#include "w600.h"
#include <avr/pgmspace.h>

static const char* RSP_OK = "+OK\r\n";
static const char* RSP_OK_equal = "+OK=";
static const char* RSP_OK_min = "+OK";
static const char* AT_enter = "\r\n";

#ifndef HAVE_HWSERIAL1
  #include "SoftwareSerial.h"
#endif

#if defined(ARDUINO_ARCH_SAMD) || defined(HAVE_HWSERIAL1)
void AtWifi::begin(HardwareSerial &uart,uint32_t baud)
{
    ATSerial::begin(uart,baud);
}
#else
void AtWifi::begin(SoftwareSerial &uart,uint32_t baud)
{
    ATSerial::begin(uart,baud);
}
#endif

// for more documentation on AT commands for this chip see http://www.winnermicro.com/en/upload/1/editor/1559640551866.pdf

//TODO write seperate documentation on AT commands that can be sent with sendAT
//TODO write function get response
//TODO figure out how to compile

//-----------------------------------------------------------------
// bool AtWifi::scan()
// {
//     return sendAT("AT+WSCAN");
// }

// bool AtWifi::wifiGetMode()
// {
//     return sendAT("AT+WPRT=!");
// }

// bool AtWifi::wifiStaGetTargetApSsid()
// {
//     return sendAT("AT+SSID");
// }

// bool AtWifi::wifiStaGetTargetApPswd()
// {
//     return sendAT("AT+KEY");
// }

// bool AtWifi::wifiApGetSsid()
// {
//     return sendAT("AT+APSSID");
// }
// bool AtWifi::wifiApGetPswd()
// {
//     return sendAT("AT+APKEY");
// }
// bool AtWifi::wifiApGetEncry()
// {
//     return sendAT("AT+APENCRY");
// }
// bool AtWifi::wifiApGetIpAndMask()
// {
//     return sendAT("AT+APNIP");
// }

// bool AtWifi::wifiReset()
// {
//     return sendAT("AT+Z");
// }

// bool AtWifi::enterTcMode()
// {
//     return sendAT("AT+ENTM");
// }

// bool AtWifi::resetTofactorySettings()
// {
//     return sendAT("AT+RSTF");
// }

// bool AtWifi::saveSettingsToFlash()
// {
//     return sendAT("AT+PMTF");
// }


// bool AtWifi::getStaMac()
// {
//     return sendAT("AT+QMAC");
// }

// bool AtWifi::getApMac()
// {
//     return sendAT("AT+APMAC");
// }

// bool AtWifi::wifiStaGetIp()
// {
//     return sendAT("AT+NIP");
// }
// bool AtWifi::getBaudrate()
// {
//     return sendAT("AT+UART");
// }

// bool AtWifi::wifiGetOneshotInfo()
// {
//     return sendAT("AT+CUSTDATA");
// }

// bool AtWifi::joinNetwork()
// {
//     return sendAT("AT+WJOIN") //TODO this also had custom timeout 8000
// }

// bool AtWifi::wifiStaDisconnect()
// {
//     return sendAT("AT+WLEAV");
// }

// bool AtWifi::wifiApDisconnect()
// {
//     return sendAT("AT+WLEAV=2");
// }

// bool AtWifi::getWifiConnectStatus()
// {
//     return sendAT("AT+LKSTT");
// }


// bool AtWifi::wifiApGetStationsInfo()
// {
//     return sendAT("AT+SLIST");
// }
//-----------------------------------------------------------------------------

bool AtWifi::sendAT(const __FlashStringHelper* cmd)
{
    ATSerial::flush();
    strcpy_P(_cmd_buffer,(const char*)cmd);
    strcat(_cmd_buffer,AT_enter);
    return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK_min,_resp_buffer);
}

bool AtWifi::wifiSetMode(WifiMode mode)
{
    ATSerial::flush();
    sprintf_P(_cmd_buffer,PSTR("AT+WPRT=!%d"),mode);
    strcat(_cmd_buffer,AT_enter);
    return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK,_resp_buffer);
}

bool AtWifi::wifiStaSetTargetApSsid(const __FlashStringHelper* ssid)
{
    ATSerial::flush();
    strcpy_P(_cmd_buffer,PSTR("AT+SSID=!"));
    strcat_P(_cmd_buffer,(const char*)ssid);
    strcat(_cmd_buffer,AT_enter);
    return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK,_resp_buffer);
}

bool AtWifi::wifiStaSetTargetApPswd(const __FlashStringHelper* password)
{
    ATSerial::flush();
    strcpy_P(_cmd_buffer,PSTR("AT+KEY=!1,0,"));
    strcat_P(_cmd_buffer,(const char*)password);
    strcat(_cmd_buffer,AT_enter);
    return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK,_resp_buffer);
}


/**Create a socket connection in STA mode.
 * msg - The socket number that module return.
 * pro - network protocol,TCP or UDP.
 * mode - client or server.
 * server - server ip addr.
 * remote_port
 * local_port
 * */
int AtWifi::wifiCreateSocketSTA(NetProtocol pro,NetMode mode,const char* server,uint16_t remote_port,uint16_t local_port)
{
    ATSerial::flush();
    sprintf_P(_cmd_buffer,PSTR("AT+SKCT=%d,%d,%s,%d,%d,%s"),pro,mode,server,remote_port,local_port,AT_enter);
    bool ok = ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK_equal,_resp_buffer);
    if (ok) {
        return atoi(&_resp_buffer[4]);
    } else {
        return -1;
    }
    //TODO this previously had a custom timeout set 5000
}

/**Send msg from socket.
 * socket - socket number.
 * send - the string msg to send.
 * */
bool AtWifi::wifiSocketSend(int32_t socket,const char* send)
{
    bool ret = wifiSocketPrepareSend(socket, strlen(send));
    if (ret) {
        ATSerial::flush();
        ATSerial::ATWrite(send);
    }
    return ret;
}

bool AtWifi::wifiSocketPrepareSend(int socket,int message_length)
{
    ATSerial::flush();
    const char* fmt = PSTR("AT+SKSND=%d,%d,%s");
    int buflen = snprintf_P(nullptr,0, fmt,socket,message_length,AT_enter);
    char buf[buflen];
    sprintf_P(buf, fmt,socket,message_length,AT_enter);
      #if DEBUG_EN
        debug.print(F("message_length:"));
        debug.println(message_length);
        debug.print("buffer size:");
        debug.println(buflen);
        debug.print(F("buffer:"));
        debug.println(buf);
    #endif
    return ATSerial::sendCmdAndCheckRsp(buf,RSP_OK_equal,_resp_buffer);
}

void AtWifi::write_P(const __FlashStringHelper* data){
    strcpy_P(_cmd_buffer, (const char*)data);
    ATSerial::ATWrite(_cmd_buffer);
}

/***
 * each header must be terminated with \n
 * content must end with \n\n.
 ***/
bool AtWifi::httpPost(
    int socket,
    const __FlashStringHelper* post_url,
    const __FlashStringHelper* host,
    const __FlashStringHelper* user_agent,
    const __FlashStringHelper* content_type,
    const __FlashStringHelper* opt_headers,
    char* content)
{
   int header_len = strlen_P((const char*)post_url) + strlen_P((const char*)host) + strlen_P((const char*)content_type) + strlen_P((const char*)opt_headers) + strlen_P((const char*)user_agent);
   int content_len = strlen(content);
   const char* fmt = PSTR("Content-Length: %d\n\n");
   int len_message_len = snprintf_P(nullptr,0,fmt,(content_len-2));//does not include the 2 newlines
   bool sent =  wifiSocketPrepareSend(socket,header_len + content_len + len_message_len);

   if (sent) {
        char buf[len_message_len];
        sprintf_P(buf,fmt,(content_len-2));
    #if DEBUG_EN
        debug.print(F("len_message_len:"));
        debug.println(len_message_len);
        debug.print(F("buffer:"));
        debug.println(buf);
    #endif
        ATSerial::flush();
        write_P(post_url);
        write_P(host);
        write_P(user_agent);
        write_P(content_type);
        write_P(opt_headers);
        ATSerial::ATWrite(buf);
        ATSerial::ATWrite(content);
   } 
   return sent;
}

/**Recv msg from socket.
 * msg - the msg recved from socket.
 * scoket - socket number
 * read_len - recv len.
 * */
bool AtWifi::wifiSocketRead(int32_t socket, uint32_t read_len) //TODO is there a problem with types passed to sprintf_P here?
{
    ATSerial::flush();
    sprintf_P(_cmd_buffer, PSTR("AT+SKRCV=%" PRId32 ",%" PRIu32 ",%s"), socket, read_len, AT_enter);
    return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK_equal,_resp_buffer);
}

void AtWifi::sendBinaryMsg(uint8_t *msg,uint32_t msg_len)
{
    ATSerial::flush();
    ATSerial::binWrite(msg,msg_len);
}

bool AtWifi::recvData(uint8_t *recv_msg,uint32_t *len)
{
    return ATSerial::waitForData(recv_msg,len); //prev had custom timeout 5000
}

// Untested code todo with config or reading data
// bool AtWifi::setBaudrate(uint32_t baud)
// {
//     ATSerial::flush();
//     sprintf_P(_cmd_buffer,PSTR("AT+UART=!%d,0,0,0,0"),baud);
//     strcat(_cmd_buffer,AT_enter);
//     return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK,_resp_buffer);
// }

// bool AtWifi::getSpecSocketInfo(int socket)
// {
//     ATSerial::flush();
//     sprintf_P(_cmd_buffer,PSTR("AT+SKSTT=%d"),socket);
//     strcat(_cmd_buffer,AT_enter);
//     return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK_equal,_resp_buffer);
// }

// bool AtWifi::wifiCloseSpecSocket(int socket)
// {
//     ATSerial::flush();
//     sprintf_P(_cmd_buffer,PSTR("AT+SKCLS=%d"),socket);
//     strcat(_cmd_buffer,AT_enter);
//     return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK,_resp_buffer);
// }

// bool AtWifi::wifiSetDefaultSocket(int32_t socket)
// {
//     ATSerial::flush();
//     sprintf_P(_cmd_buffer,PSTR("AT+SKSDF=%d"),socket);
//     strcat(_cmd_buffer,AT_enter);
//     return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK,_resp_buffer);
// }

// bool AtWifi::setWifiConfigMode(WifiConfig mode)
// {
//     ATSerial::flush();
//     sprintf_P(_cmd_buffer,PSTR("AT+ONEMODE=!%d"),mode);
//     strcat(_cmd_buffer,AT_enter);
//     return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK,_resp_buffer);
// }


// Code below contains code for operating in AP mode - ported from original seeed library and re-written to avoid Strings but untested.

// /**Create a socket binding in AP mode.
//  * msg - The socket number that module return.
//  * pro - network protocol,TCP or UDP.
//  * mode - client or server.
//  * time_out - time out for client,when there is not a communication between server and client for a long time,abort connection.
//  * remote_port
//  * local_port
//  * */
// bool AtWifi::wifiCreateSocketAP(NetProtocol pro,NetMode mode,uint32_t time_out,uint16_t remote_port,uint16_t local_port)
// {
//     ATSerial::flush();
//     sprintf_P(_cmd_buffer,PSTR("AT+SKCT=%d,%d,%d,%d,%d,%s"),pro,mode,time_out,remote_port,local_port,AT_enter);
//     return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK_equal,_resp_buffer);
// }

// /*Whether to enable ap ssid broadcast function!*/
// bool AtWifi::broadcastApSsidSet(bool flag)
// {
//     ATSerial::flush();
//     sprintf_P(_cmd_buffer,PSTR("AT+BRDSSID=!%d"),flag);
//     strcat(_cmd_buffer,AT_enter);
//     return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK,_resp_buffer);
// }

// bool AtWifi::wifiApSetSsid(const char* ssid)
// {
//     ATSerial::flush();
//     strcpy_P(_cmd_buffer,PSTR("AT+APSSID=!"));
//     strcat(_cmd_buffer,ssid);
//     strcat(_cmd_buffer,AT_enter);
//     return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK,_resp_buffer);
// }

// bool AtWifi::wifiApSetPswd(const char* password,Key_format format)
// {
//     ATSerial::flush();
//     sprintf_P(_cmd_buffer,PSTR("AT+APKEY=!%d,4,"),format);
//     strcat(_cmd_buffer,password);
//     strcat(_cmd_buffer,AT_enter);
//     return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK,_resp_buffer);
// }

// /** Set encryption mode
//  *  0 - OPEN
//  *  1 - WEP64
//  *  2 - WEP128
//  *  3 - WPA-PSK(TKIP)
//  *  4 - WPA-PSK(CCMP/AES)
//  *  5 - WPA2-PSK(TKIP)
//  *  6 - WPA2-PSK(CCMP/AES)
//  *  We use the 5 - WPA2-PSK(TKIP) hear;
//  * */
// bool AtWifi::wifiApSetEncry(int mode)
// {
//     ATSerial::flush();
//     sprintf_P(_cmd_buffer,PSTR("AT+APENCRY=!%d"),mode);
//     strcat(_cmd_buffer,AT_enter);
//     return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK,_resp_buffer);
// }

// /**Set network infomation for module when it works in AP mode.
//  * appr:The way that client get ip addr:DHCP or static.
//  * ip:ip addr
//  * gateway:gateway ip addr
//  * mask:network mask
//  * dns:dns ip addr.
//  * */
// bool AtWifi::wifiApSetIpAndMask(Get_ip_approach appr,const char* ip,const char* gateway,const char* mask,const char* dns)
// {
//     ATSerial::flush();
//     sprintf_P(_cmd_buffer,PSTR("AT+APNIP=!%d,"),appr);
//     strcat(_cmd_buffer,ip);
//     strcat(_cmd_buffer,",");
//     strcat(_cmd_buffer,gateway);
//     strcat(_cmd_buffer,",");
//     strcat(_cmd_buffer,mask);
//     strcat(_cmd_buffer,",");
//     strcat(_cmd_buffer,dns);
//     strcat(_cmd_buffer,AT_enter);
//     return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK,_resp_buffer);
// }

// bool AtWifi::exit_Tc_mode()
// {
//     ATSerial::flush();
//     strcpy_P(_cmd_buffer,PSTR("+++"));
//     return ATSerial::sendCmdAndCheckRsp(_cmd_buffer,RSP_OK,_resp_buffer);
// }



