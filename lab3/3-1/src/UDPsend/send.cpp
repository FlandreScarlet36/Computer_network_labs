#include <iostream>
#include <WINSOCK2.h>
#include <time.h>
#include <fstream>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)
using namespace std;

const int MAXSIZE = 1024;//´«Êä»º³åÇø×î´ó³¤¶È
const unsigned char SYN = 0x1; //001¡ª¡ª FIN = 0 ACK = 0 SYN = 1
const unsigned char ACK = 0x2; //010¡ª¡ª FIN = 0 ACK = 1 SYN = 0
const unsigned char ACK_SYN = 0x3;//011¡ª¡ª FIN = 0 ACK = 1 SYN = 1
const unsigned char FIN = 0x4;//100¡ª¡ª FIN = 1 ACK = 0 SYN = 0
const unsigned char FIN_ACK = 0x5;//101¡ª¡ª FIN = 1 ACK = 0 SYN = 1
const unsigned char OVER = 0x7;//½áÊø±êÖ¾ 111¡ª¡ª FIN = 1 ACK = 1 SYN = 1
double MAX_TIME = 0.5 * CLOCKS_PER_SEC;


u_short cksum(u_short* mes, int size) {
    int count = (size + 1) / 2;
    u_short* buf = (u_short*)malloc(size + 1);
    memset(buf, 0, size + 1);
    memcpy(buf, mes, size);
    u_long sum = 0;
    while (count--) {
        sum += *buf++;
        if (sum & 0xffff0000) {
            sum &= 0xffff;
            sum++;
        }
    }
    return ~(sum & 0xffff);
}

struct HEADER
{
    u_short sum = 0;//Ğ£ÑéºÍ 16Î»
    u_short datasize = 0;//Ëù°üº¬Êı¾İ³¤¶È 16Î»
    //°ËÎ»£¬Ê¹ÓÃºóËÄÎ»£¬ÅÅÁĞÊÇFIN ACK SYN 
    unsigned char flag = 0;
    //°ËÎ»£¬´«ÊäµÄĞòÁĞºÅ£¬0~255£¬³¬¹ıºómod
    unsigned char SEQ = 0;
    HEADER() {
        sum = 0;//Ğ£ÑéºÍ 16Î»
        datasize = 0;//Ëù°üº¬Êı¾İ³¤¶È 16Î»
        flag = 0;//8Î»£¬Ê¹ÓÃºóËÄÎ»£¬ÅÅÁĞÊÇFIN ACK SYN 
        SEQ = 0;//8Î»
    }
};

<<<<<<< HEAD
int Connect(SOCKET& socketClient, SOCKADDR_IN& servAddr, int& servAddrlen)//Èı´ÎÎÕÊÖ½¨Á¢Á¬½Ó
{
    HEADER header;
    char* Buffer = new char[sizeof(header)];

    u_short sum;

    //µÚÒ»´ÎÎÕÊÖ
    header.flag = SYN;
    header.sum = 0;//Ğ£ÑéºÍÖÃ0
    //¼ÆËãĞ£ÑéºÍ
    header.sum = 0;
    header.sum = cksum((u_short*)&header, sizeof(header));
    //½«Êı¾İÍ··ÅÈëbuffer
    memcpy(Buffer, &header, sizeof(header));
    if (sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen) == -1)
    {
        return -1;
    }
    else
    {
        cout << "[info]³É¹¦·¢ËÍµÚÒ»´ÎÎÕÊÖÊı¾İ" << endl;
    }
    clock_t start = clock(); //¼ÇÂ¼·¢ËÍµÚÒ»´ÎÎÕÊÖÊ±¼ä

    //ÉèÖÃsocketÎª·Ç×èÈû×´Ì¬
    u_long mode = 1;
    ioctlsocket(socketClient, FIONBIO, &mode);



    //µÚ¶ş´ÎÎÕÊÖ
    while (recvfrom(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, &servAddrlen) <= 0)
    {
        //³¬Ê±ĞèÒªÖØ´«
        if (clock() - start > MAX_TIME)//³¬Ê±£¬ÖØĞÂ´«ÊäµÚÒ»´ÎÎÕÊÖ
        {
            cout << "[info]µÚÒ»´ÎÎÕÊÖ³¬Ê±" << endl;
            header.flag = SYN;
            header.sum = 0;//Ğ£ÑéºÍÖÃ0
            header.sum = cksum((u_short*)&header, sizeof(header));//¼ÆËãĞ£ÑéºÍ
            memcpy(Buffer, &header, sizeof(header));//½«Êı¾İÍ··ÅÈëBuffer
            sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
            start = clock();
            cout << "[info]ÒÑ¾­ÖØ´«" << endl;
        }
    }

    //µÚ¶ş´ÎÎÕÊÖ£¬ÊÕµ½À´×Ô½ÓÊÕ¶ËµÄACK
    //½øĞĞĞ£ÑéºÍ¼ìÑé
    memcpy(&header, Buffer, sizeof(header));
    if (header.flag == ACK && cksum((u_short*)&header, sizeof(header)) == 0)
    {
        cout << "[info]½ÓÊÕµ½µÚ¶ş´ÎÎÕÊÖÊı¾İ" << endl;
    }
    else
    {
        //cout << "[info]´íÎóÊı¾İ£¬ÇëÖØÊÔ" << endl;
        return -1;
    }

    //½øĞĞµÚÈı´ÎÎÕÊÖ
    header.flag = ACK_SYN;
    header.sum = 0;
    header.sum = cksum((u_short*)&header, sizeof(header));//¼ÆËãĞ£ÑéºÍ
    if (sendto(socketClient, (char*)&header, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen) == -1)
    {
        return -1;
    }
    else
    {
        cout << "[info]³É¹¦·¢ËÍµÚÈı´ÎÎÕÊÖÊı¾İ" << endl;
    }
    cout << "[info]·şÎñÆ÷³É¹¦Á¬½Ó£¡¿ÉÒÔ·¢ËÍÊı¾İ" << endl;
    return 1;
}

void send_package(SOCKET& socketClient, SOCKADDR_IN& servAddr, int& servAddrlen, char* message, int len, int& order)
{
    HEADER header;
    char* buffer = new char[MAXSIZE + sizeof(header)];
    header.datasize = len;
    header.SEQ = unsigned char(order);//ĞòÁĞºÅ
    memcpy(buffer, &header, sizeof(header));//½«¼ÆËãºÃĞ£ÑéºÍµÄheaderÖØĞÂ¸³Öµ¸øbuffer£¬´ËÊ±µÄbuffer¿ÉÒÔ·¢ËÍÁË
    memcpy(buffer + sizeof(header), message, len);//bufferÎªheader+message
    header.sum = cksum((u_short*)buffer, sizeof(header) + len);//¼ÆËãĞ£ÑéºÍ
    //·¢ËÍ
    sendto(socketClient, buffer, len + sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
    cout << "[info]·¢ËÍÁË" << len << " ×Ö½Ú£¬" << " flag:" << int(header.flag) 
        << " SEQ:" << int(header.SEQ) << " SUM:" << int(header.sum) << endl;
    clock_t start = clock();//¼ÇÂ¼·¢ËÍÊ±¼ä

    //½ÓÊÕackµÈĞÅÏ¢
    while (1)
    {
        u_long mode = 1;
        ioctlsocket(socketClient, FIONBIO, &mode);//½«Ì×½Ó¿Ú×´Ì¬¸ÄÎª·Ç×èÈû


        while (recvfrom(socketClient, buffer, MAXSIZE, 0, (sockaddr*)&servAddr, &servAddrlen)<=0) {
            //³¬Ê±ÖØ´«
            if (clock() - start > MAX_TIME)
            {
                cout << "[info]·¢ËÍÊı¾İ³¬Ê±" << endl;
                header.datasize = len;
                header.SEQ = u_char(order);//ĞòÁĞºÅ
                header.flag = u_char(0x0);
                memcpy(buffer, &header, sizeof(header));
                memcpy(buffer + sizeof(header), message, sizeof(header) + len);
                u_short check = cksum((u_short*)buffer, sizeof(header) + len);//¼ÆËãĞ£ÑéºÍ
                header.sum = check;
                memcpy(buffer, &header, sizeof(header));
                sendto(socketClient, buffer, len + sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);//·¢ËÍ
                cout << "[info]ÖØĞÂ·¢ËÍÊı¾İ" << endl;
                start = clock();//¼ÇÂ¼·¢ËÍÊ±¼ä
            }
            else break;
        }
        memcpy(&header, buffer, sizeof(header));//»º³åÇø½ÓÊÕµ½ĞÅÏ¢£¬·ÅÈëheader£¬´ËÊ±headerÖĞÊÇÊÕµ½µÄÊı¾İ
        //u_short check = cksum((u_short*)&header, sizeof(header));
        if (cksum((u_short*)&header, sizeof(header)) == 0 && header.SEQ == u_short(order) && header.flag == ACK)
        {
            cout << "[info] ÒÑÈ·ÈÏÊÕµ½ - Flag:" << int(header.flag) 
                << " SEQ:" << int(header.SEQ) << " SUM:" << int(header.sum) << endl;
            break;
        }
        else
        {
=======
    // å‘é€ç¬¬ä¸€æ¬¡æ¡æ‰‹è¯·æ±‚æŠ¥æ–‡
    memset(header, 0, HEADERSIZE);
    // è®¾ç½®seqä½
    int seq = rand();
    header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
    header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
    header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
    header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
    // è®¾ç½®SYNä½
    header[FLAG_BIT_POSITION] = 0b010; // SYNåœ¨header[8]çš„ç¬¬äºŒä½ï¼Œæ‰€ä»¥è¿™ä¸€è¡Œè¡¨ç¤ºSYN == 1
    checksum = checkSum(header, HEADERSIZE);
    // è®¾ç½®checksumä½
    header[CHECKSUM_BITS_START] = (u_char)(checksum & 0xFF);
    header[CHECKSUM_BITS_START + 1] = (u_char)(checksum >> 8);
    sendto(sendSocket, header, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
    cout << "Send the First Handshake message!" << endl;

    // æ¥å—ç¬¬äºŒæ¬¡æ¡æ‰‹åº”ç­”æŠ¥æ–‡
    char recvBuf[HEADERSIZE] = { 0 };
    int recvResult = 0;
    while (true) {
        recvResult = recvfrom(sendSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, &len);
        // æ¥å—ack
        int ack = recvBuf[ACK_BITS_START] + (recvBuf[ACK_BITS_START + 1] << 8)
            + (recvBuf[ACK_BITS_START + 2] << 16) + (recvBuf[ACK_BITS_START + 3] << 24);
        if ((ack == seq + 1) && (recvBuf[FLAG_BIT_POSITION] == 0b110)) { // 0b110ä»£è¡¨ACK SYN FIN == 110
            cout << "Successfully received the Second Handshake message!" << endl;
            break;
        }
        else {
            cout << "Failed to received the correct Second Handshake message, Handshake failed!" << endl;
            return false;
        }
    }

    // å‘é€ç¬¬ä¸‰æ¬¡æ¡æ‰‹è¯·æ±‚æŠ¥æ–‡
    memset(header, 0, HEADERSIZE);
    // è®¾ç½®ackä½ï¼Œack = seq of message2 + 1
    int ack = (u_char)recvBuf[SEQ_BITS_START] + ((u_char)recvBuf[SEQ_BITS_START + 1] << 8)
        + ((u_char)recvBuf[SEQ_BITS_START + 2] << 16) + ((u_char)recvBuf[SEQ_BITS_START + 3] << 24) + 1;
    header[ACK_BITS_START] = (u_char)(ack & 0xFF);
    header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
    header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
    header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
    // è®¾ç½®ACKä½
    header[FLAG_BIT_POSITION] = 0b100;
    checksum = checkSum(header, HEADERSIZE);
    // è®¾ç½®checksumä½
    header[CHECKSUM_BITS_START] = (u_char)(checksum & 0xFF);
    header[CHECKSUM_BITS_START + 1] = (u_char)(checksum >> 8);
    sendto(sendSocket, header, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
    cout << "Send the Third Handshake message!" << endl;

    cout << "Handshake successfully!" << endl;
    return true;

}

int hasSent = 0; // å·²å‘é€çš„æ–‡ä»¶å¤§å°
int fileSize = 0;
int sendResult = 0; // æ¯æ¬¡sendtoå‡½æ•°çš„è¿”å›ç»“æœ
int sendSize = 0; // æ¯æ¬¡å®é™…å‘é€çš„æŠ¥æ–‡æ€»é•¿åº¦
int seq = 1, ack = 0; // å‘é€åŒ…æ—¶çš„seq, ack
int base = 1; // æ»‘åŠ¨çª—å£èµ·å§‹
int seq_opp = 0, ack_opp = 0; // æ”¶åˆ°çš„å¯¹é¢çš„seq, ack
int dataLength = 0; // æ¯æ¬¡å®é™…å‘é€çš„æ•°æ®éƒ¨åˆ†é•¿åº¦(= sendSize - HEADERSIZE)
u_short checksum = 0; // æ ¡éªŒå’Œ
bool resend = false; // é‡ä¼ æ ‡å¿—
char recvBuf[HEADERSIZE] = { 0 }; // æ¥å—å“åº”æŠ¥æ–‡çš„ç¼“å†²åŒº
int recvResult = 0; // æ¥å—å“åº”æŠ¥æ–‡çš„è¿”å›å€¼
bool finishSend = false; // æ˜¯å¦ç»“æŸäº†ä¸€ä¸ªæ–‡ä»¶çš„å‘é€
// char sendWindow[PACKETSIZE * SEND_WINDOW_SIZE] = {0}; // æ»‘åŠ¨çª—å£

bool THREAD_END = false; // é€šè¿‡è¿™ä¸ªå˜é‡å‘Šè¯‰recvRespondThreadå’ŒtimerThreadé€€å‡º
int THREAD_CREAT_FLAG = 1;
int index = 0; // ç”¨äºæ‹¯æ•‘receiveå‘è¿‡æ¥çš„æœ€åä¸€ä¸ªç¡®è®¤åŒ…ä¸¢å¤±ï¼Œsendç«¯å¡åœ¨hasSent == fileSizeå†…çš„å‡ºä¸æ¥çš„å˜é‡

void timerThread() {
    while (!THREAD_END) {
        last = clock();
        if (last - start >= TIMEOUT) {
            start = clock();
            resend = true;
        }
    }
}


void recvRespondThread() {
    // æ¥å—å“åº”æŠ¥æ–‡çš„çº¿ç¨‹
    while (!THREAD_END) {
        recvResult = recvfrom(sendSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, &len);
        if (recvResult == SOCKET_ERROR) {
            cout << "[Error]: receive error! sleep!" << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
>>>>>>> 3e47a6b2561d05b8b079c9439f659d5a50a4cc84
            continue;
        }
    }
    u_long mode = 0;
    ioctlsocket(socketClient, FIONBIO, &mode);//¸Ä»Ø×èÈûÄ£Ê½
}

<<<<<<< HEAD
void send(SOCKET& socketClient, SOCKADDR_IN& servAddr, int& servAddrlen, char* message, int len)
{
    //ĞèÒª´«ËÍµÄÊı¾İ°ü¸öÊı
    int packagenum = len / MAXSIZE + (len % MAXSIZE != 0);
    int seqnum = 0;
    cout << packagenum << endl;
    for (int i = 0; i < packagenum; i++)
    {
        send_package(socketClient, servAddr, servAddrlen,
            message + i * MAXSIZE, i == packagenum - 1 ? len - (packagenum - 1) * MAXSIZE : MAXSIZE, seqnum);
        seqnum++;
        if (seqnum > 255)
        {
            seqnum = seqnum - 256;
        }
    }
    //·¢ËÍ½áÊøĞÅÏ¢
    HEADER header;
    char* Buffer = new char[sizeof(header)];
    header.flag = OVER;
    header.sum = cksum((u_short*)&header, sizeof(header));
    memcpy(Buffer, &header, sizeof(header));
    sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
    cout << "[info]·¢ËÍOVERĞÅºÅ" << endl;
    clock_t start = clock();
    while (1)
    {
        u_long mode = 1;
        ioctlsocket(socketClient, FIONBIO, &mode);
        //ÊÕµ½°ü£¬´æÔÚBufferÖĞ
        while (recvfrom(socketClient, Buffer, MAXSIZE, 0, (sockaddr*)&servAddr, &servAddrlen) <= 0)
        {
            //³¬Ê±£¬°Ñ¡°send end¡±Êı¾İ°üÖØ´«
            if (clock() - start > MAX_TIME)
            {
                cout << "[info]·¢ËÍOVERĞÅºÅ³¬Ê±" << endl;
                char* Buffer = new char[sizeof(header)];
                header.flag = OVER;
                header.sum = cksum((u_short*)&header, sizeof(header));
                memcpy(Buffer, &header, sizeof(header));
                sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
                cout << "[info]ÒÑ¾­ÖØ·¢OVERĞÅºÅ" << endl;
                start = clock();
            }
            else break;
        }
        memcpy(&header, Buffer, sizeof(header));//»º³åÇø½ÓÊÕµ½ĞÅÏ¢£¬¶ÁÈ¡BufferÀïµÄĞÅÏ¢
        u_short check = cksum((u_short*)&header, sizeof(header));
        //ÊÕµ½µÄÊı¾İ°üÎªOVERÔòÒÑ¾­³É¹¦½ÓÊÜÎÄ¼ş
        if (header.flag == OVER && check == 0)
        {
            cout << "[info]¶Ô·½ÒÑ³É¹¦½ÓÊÕÎÄ¼ş" << endl;
=======
void sendfile(const char* filename) {
    // è¯»å…¥æ–‡ä»¶
    ifstream is(filename, ifstream::in | ios::binary);
    is.seekg(0, is.end);
    fileSize = is.tellg();
    is.seekg(0, is.beg);
    char* filebuf;
    filebuf = (char*)calloc(fileSize, sizeof(char));
    is.read(filebuf, fileSize);
    is.close();

    // å‘é€æ–‡ä»¶å
    memset(sendBuf, 0, PACKETSIZE);
    header[FLAG_BIT_POSITION] = 0b1000;
    strcat((char*)memcpy(sendBuf, header, HEADERSIZE) + HEADERSIZE, filename);
    sendto(sendSocket, sendBuf, PACKETSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));

    // å‘é€æ–‡ä»¶å¤§å°
    memset(sendBuf, 0, PACKETSIZE);
    header[FLAG_BIT_POSITION] = 0b10000;
    strcat((char*)memcpy(sendBuf, header, HEADERSIZE) + HEADERSIZE, to_string(fileSize).c_str());
    sendto(sendSocket, sendBuf, PACKETSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));

    hasSent = 0; // å·²å‘é€çš„æ–‡ä»¶å¤§å°
    seq = 1, ack = 0; // å‘é€åŒ…æ—¶çš„seq, ack
    base = 1; // æ»‘åŠ¨çª—å£èµ·å§‹
    seq_opp = 0, ack_opp = 0;
    resend = false; // é‡ä¼ æ ‡å¿—
    finishSend = false; // ç»“æŸå‘é€æ ‡å¿—

    // å‘é€æ–‡ä»¶
    while (true) {
        if (finishSend) {
            cout << "[Fin]: Send successfully, send " << fileSize << " bytes." << endl;
            totalLength += fileSize;
>>>>>>> 3e47a6b2561d05b8b079c9439f659d5a50a4cc84
            break;
        }
        else
        {
            continue;
        }
<<<<<<< HEAD
=======

        // å¦‚æœä¸éœ€è¦é‡ä¼ ï¼Œåˆ™éœ€è¦é¦–å…ˆæ£€æŸ¥æ»‘åŠ¨çª—å£æ˜¯å¦æ»¡
        if (seq < base + SEND_WINDOW_SIZE) {
            if (hasSent < fileSize) {
                // å¦‚æœæ²¡æ»¡ï¼Œåˆ™è®¾ç½®header
                // seq = å³å°†å‘é€çš„packetåºå·
                // ack ä¸éœ€è¦è®¾ç½®

                // è®¾ç½®seqä½
                header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
                header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
                header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
                header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);

                // è®¾ç½®ACKä½
                header[FLAG_BIT_POSITION] = 0b100;

                // è®¾ç½®data lengthä½
                dataLength = sendSize - HEADERSIZE;
                header[DATA_LENGTH_BITS_START] = dataLength & 0xFF;
                header[DATA_LENGTH_BITS_START + 1] = dataLength >> 8;

                // fileä¸­æ­¤æ¬¡è¦è¢«å‘é€çš„æ•°æ®->dataSegment
                memcpy(dataSegment, filebuf + hasSent, sendSize - HEADERSIZE);
                // header->sendBuf
                memcpy(sendBuf, header, HEADERSIZE);
                // dataSegment->sendBufï¼ˆä»sendBuf[10]å¼€å§‹ï¼‰
                memcpy(sendBuf + HEADERSIZE, dataSegment, sendSize - HEADERSIZE);
                // è®¾ç½®checksumä½
                checksum = checkSum(sendBuf, sendSize);
                header[CHECKSUM_BITS_START] = sendBuf[CHECKSUM_BITS_START] = checksum & 0xFF;
                header[CHECKSUM_BITS_START + 1] = sendBuf[CHECKSUM_BITS_START + 1] = checksum >> 8;
                sendResult = sendto(sendSocket, sendBuf, sendSize, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));

                // å‘é€å®Œæ¯•åï¼Œå¦‚æœbase = seqï¼Œè¯´æ˜å‘çš„æ˜¯æ»‘åŠ¨çª—å£å†…çš„ç¬¬ä¸€ä¸ªpacketï¼Œåˆ™å¯åŠ¨è®¡æ—¶
                if (base == seq) {
                    start = clock();
                }

                // æ›´æ–°å‘é€é•¿åº¦å’Œseq
                hasSent += sendSize - HEADERSIZE;
                seq++;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(TEST_STOPTIME));
        }

        // å¦‚æœhasSent == fileSizeï¼Œè¯´æ˜ä¸ç”¨å†å‘äº†ï¼Œä½†æ˜¯è¿˜ä¸èƒ½ç»“æŸå‘é€ã€‚ç»“æŸå‘é€çš„æ ‡å¿—åªèƒ½ç”±æ¥æ”¶çº¿ç¨‹å‘ŠçŸ¥ï¼Œéœ€è¦ç¡®è®¤æ”¶åˆ°äº†å¯¹æ–¹çš„æ‰€æœ‰ACKæ‰èƒ½ç»“æŸå‘é€
        if (hasSent == fileSize) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            index++;
            if (index == 10) // å¦‚æœäº§ç”Ÿåæ¬¡hasSent == fileSizeä¸”æ²¡æœ‰ç»“æŸå‘é€ï¼Œè¯´æ˜receiveç«¯ç»“æŸäº†æ¥æ”¶packetå’Œå‘é€ackï¼Œä¸”æœ€åä¸€ä¸ªackä¸¢å¤±äº†
                finishSend = true;
        }

>>>>>>> 3e47a6b2561d05b8b079c9439f659d5a50a4cc84
    }
    u_long mode = 0;
    ioctlsocket(socketClient, FIONBIO, &mode);//¸Ä»Ø×èÈûÄ£Ê½
}


<<<<<<< HEAD
int disConnect(SOCKET& socketClient, SOCKADDR_IN& servAddr, int& servAddrlen)
{
    HEADER header;
    char* Buffer = new char[sizeof(header)];

    u_short sum;

    //½øĞĞµÚÒ»´Î»ÓÊÖ
    header.flag = FIN;
    header.sum = 0;//Ğ£ÑéºÍÖÃ0
    header.sum = cksum((u_short*)&header, sizeof(header));//¼ÆËãĞ£ÑéºÍ
    memcpy(Buffer, &header, sizeof(header));//½«Ê×²¿·ÅÈë»º³åÇø
    if (sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen) == -1)
    {
        return -1;
    }
    else
    {
        cout << "[info]³É¹¦·¢ËÍµÚÒ»´Î»ÓÊÖÊı¾İ" << endl;
    }
    clock_t start = clock(); //¼ÇÂ¼·¢ËÍµÚÒ»´Î»ÓÊÖÊ±¼ä


    u_long mode = 1;
    ioctlsocket(socketClient, FIONBIO, &mode);//FIONBIOÎªÃüÁî£¬ÔÊĞí1/½ûÖ¹0Ì×½Ó¿ÚsµÄ·Ç×èÈû1/×èÈû0Ä£Ê½¡£

    //½ÓÊÕµÚ¶ş´Î»ÓÊÖ
    while (recvfrom(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, &servAddrlen) <= 0)
    {
        //³¬Ê±£¬ÖØĞÂ´«ÊäµÚÒ»´Î»ÓÊÖ
        if (clock() - start > MAX_TIME)
        {
            cout << "[info]µÚÒ»´Î»ÓÊÖ³¬Ê±" << endl;
            header.flag = FIN;
            header.sum = 0;//Ğ£ÑéºÍÖÃ0
            header.sum = cksum((u_short*)&header, sizeof(header));//¼ÆËãĞ£ÑéºÍ
            memcpy(Buffer, &header, sizeof(header));//½«Ê×²¿·ÅÈë»º³åÇø
            sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
            start = clock();
            cout << "[info]ÒÑÖØ´«µÚÒ»´Î»ÓÊÖÊı¾İ" << endl;
        }
    }

    //½øĞĞĞ£ÑéºÍ¼ìÑé
    memcpy(&header, Buffer, sizeof(header));
    if (header.flag == ACK && cksum((u_short*)&header, sizeof(header) == 0))
    {
        cout << "[info]½ÓÊÕµ½µÚ¶ş´Î»ÓÊÖÊı¾İ" << endl;
    }
    else
    {
        //cout << "[info]´íÎóÊı¾İ£¬ÇëÖØÊÔ" << endl;
        return -1;
    }

    //½øĞĞµÚÈı´Î»ÓÊÖ
    header.flag = FIN_ACK;
    header.sum = 0;
    header.sum = cksum((u_short*)&header, sizeof(header));//¼ÆËãĞ£ÑéºÍ
    if (sendto(socketClient, (char*)&header, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen) == -1)
    {
        return -1;
    }
    else
    {
        cout << "[info]³É¹¦·¢ËÍµÚÈı´Î»ÓÊÖÊı¾İ" << endl;
    }
    start = clock();
    //½ÓÊÕµÚËÄ´Î»ÓÊÖ
    while (recvfrom(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, &servAddrlen) <= 0)
    {
        if (clock() - start > MAX_TIME)//³¬Ê±£¬ÖØĞÂ´«ÊäµÚÈı´Î»ÓÊÖ
        {
            cout << "[info]µÚÈı´Î»ÓÊÖ³¬Ê±" << endl;
            header.flag = FIN;
            header.sum = 0;//Ğ£ÑéºÍÖÃ0
            header.sum = cksum((u_short*)&header, sizeof(header));//¼ÆËãĞ£ÑéºÍ
            memcpy(Buffer, &header, sizeof(header));//½«Ê×²¿·ÅÈë»º³åÇø
            sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
            start = clock();
            cout << "[info]ÒÑÖØ´«µÚÈı´Î»ÓÊÖÊı¾İ" << endl;
        }
    }
    cout << "[info]ËÄ´Î»ÓÊÖ½áÊø£¬Á¬½Ó¶Ï¿ª£¡" << endl;
    return 1;
}


int main()
{
    cout << MAX_TIME << endl;

    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);

    SOCKADDR_IN severAddr;
    SOCKET server;

    severAddr.sin_family = AF_INET;//Ê¹ÓÃIPV4
    severAddr.sin_port = htons(3939);
    severAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    server = socket(AF_INET, SOCK_DGRAM, 0);
    int len = sizeof(severAddr);
    //½¨Á¢Á¬½Ó
    if (Connect(server, severAddr, len) == -1)
    {
        return 0;
    }

    //¶ÁÈ¡ÎÄ¼şÄÚÈİµ½buffer
    string inputFile;//Ï£Íû´«ÊäµÄÎÄ¼şÃû³Æ
    cout << "ÇëÊäÈëÏ£Íû´«ÊäµÄÎÄ¼şÃû³Æ" << endl;
    cin >> inputFile;
    ifstream fileIN(inputFile.c_str(), ifstream::binary);//ÒÔ¶ş½øÖÆ·½Ê½´ò¿ªÎÄ¼ş
    char* buffer = new char[10000000];//ÎÄ¼şÄÚÈİ
    int i = 0;
    unsigned char temp = fileIN.get();
    while (fileIN)
    {
        buffer[i++] = temp;
        temp = fileIN.get();
=======
void wavehand() {
    int seq = 0, ack = 0;
    u_short checksum = 0;
    // å‘é€ç¬¬ä¸€æ¬¡æŒ¥æ‰‹è¯·æ±‚æŠ¥æ–‡
    memset(header, 0, HEADERSIZE);
    // è®¾ç½®seqä½
    seq = rand();
    header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
    header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
    header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
    header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
    // è®¾ç½®ACK FINä½
    header[FLAG_BIT_POSITION] = 0b101;
    checksum = checkSum(header, HEADERSIZE);
    // è®¾ç½®checksumä½
    header[CHECKSUM_BITS_START] = (u_char)(checksum & 0xFF);
    header[CHECKSUM_BITS_START + 1] = (u_char)(checksum >> 8);
    sendto(sendSocket, header, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
    cout << "Send the First Wavehand message!" << endl;

    // æ¥æ”¶ç¬¬äºŒæ¬¡æŒ¥æ‰‹åº”ç­”æŠ¥æ–‡
    char recvBuf[HEADERSIZE] = { 0 };
    int recvResult = 0;
    while (true) {
        recvResult = recvfrom(sendSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, &len);
        // æ¥å—ack
        ack = recvBuf[ACK_BITS_START] + (recvBuf[ACK_BITS_START + 1] << 8)
            + (recvBuf[ACK_BITS_START + 2] << 16) + (recvBuf[ACK_BITS_START + 3] << 24);
        if ((ack == seq + 1) && (recvBuf[FLAG_BIT_POSITION] == 0b100)) {
            cout << "Successfully received the Second Wavehand message!" << endl;
            break;
        }
        else {
            cout << "Failed to received the correct Second Wavehand message, Wavehand failed!" << endl;
            return;
        }
    }

    // æ¥æ”¶ç¬¬ä¸‰æ¬¡æŒ¥æ‰‹è¯·æ±‚æŠ¥æ–‡
    while (true) {
        recvResult = recvfrom(sendSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, &len);
        // æ¥å—ack
        ack = recvBuf[ACK_BITS_START] + (recvBuf[ACK_BITS_START + 1] << 8)
            + (recvBuf[ACK_BITS_START + 2] << 16) + (recvBuf[ACK_BITS_START + 3] << 24);
        if ((ack == seq + 1) && (recvBuf[FLAG_BIT_POSITION] == 0b101)) {
            cout << "Successfully received the Third Wavehand message!" << endl;
            break;
        }
        else {
            cout << "Failed to received the correct Third Wavehand message, Wavehand failed!" << endl;
            return;
        }
    }

    // å‘é€ç¬¬å››æ¬¡æŒ¥æ‰‹åº”ç­”æŠ¥æ–‡
    memset(header, 0, HEADERSIZE);
    // è®¾ç½®seqä½
    seq = ack;
    header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
    header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
    header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
    header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
    // è®¾ç½®ackä½
    ack = (u_char)recvBuf[ACK_BITS_START] + ((u_char)recvBuf[ACK_BITS_START + 1] << 8)
        + ((u_char)recvBuf[ACK_BITS_START + 2] << 16) + ((u_char)recvBuf[ACK_BITS_START + 3] << 24) + 1;
    header[ACK_BITS_START] = (u_char)(ack & 0xFF);
    header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
    header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
    header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
    // è®¾ç½®ACKä½
    header[FLAG_BIT_POSITION] = 0b100;
    checksum = checkSum(header, HEADERSIZE);
    // è®¾ç½®checksumä½
    header[CHECKSUM_BITS_START] = (u_char)(checksum & 0xFF);
    header[CHECKSUM_BITS_START + 1] = (u_char)(checksum >> 8);
    sendto(sendSocket, header, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
    cout << "Send the Forth Wavehand message!" << endl;

    cout << "Wavehand successfully!" << endl;
    return;
}

int main() {
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
        cout << "WSAStartup error = " << WSAGetLastError() << endl;
        exit(1);
    }
    else {
        cout << "Start success" << endl;
    }

    sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sendSocket == SOCKET_ERROR) {
        cout << "Socket error = " << WSAGetLastError() << endl;
        closesocket(sendSocket);
        WSACleanup();
        exit(1);
    }
    else {
        cout << "Socket success" << endl;
>>>>>>> 3e47a6b2561d05b8b079c9439f659d5a50a4cc84
    }
    fileIN.close();


<<<<<<< HEAD
    //·¢ËÍÎÄ¼şÃû
    send(server, severAddr, len, (char*)(inputFile.c_str()), inputFile.length());
    clock_t start1 = clock();
    //·¢ËÍÎÄ¼şÄÚÈİ£¨ÔÚbufferÀï£©
    send(server, severAddr, len, buffer, i);
    clock_t end1 = clock();
=======
    if (handshake()) {
        while (true) {
            string str;
            cout << "Please input the file name(or q to quit sending): ";
            cin >> str;
            if (str == "q") {
                THREAD_END = true;
                break;
            }
            else {
                s = clock();
                sendfile(str.c_str());
                l = clock();
                totalTime += (double)(l - s) / CLOCKS_PER_SEC;
            }
        }
        wavehand();
        cout << endl << "send time: " << totalTime << " s." << endl;
        cout << "total size: " << totalLength << " Bytes." << endl;
        cout << "throughput: " << (double)((totalLength * 8 / 1000) / totalTime) << " kbps." << endl;
    }
>>>>>>> 3e47a6b2561d05b8b079c9439f659d5a50a4cc84


    cout << "[out] ´«Êä×ÜÊ±¼äÎª:" << (end1 - start1) / CLOCKS_PER_SEC << "s" << endl;
    cout << "[out] ÍÌÍÂÂÊÎª:" << (((double)i) / ((end1 - start1) / CLOCKS_PER_SEC)) << "byte/s" << endl;
    disConnect(server, severAddr, len);

    system("pause");

}
