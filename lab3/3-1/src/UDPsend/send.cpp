#include <iostream>
#include <WINSOCK2.h>
#include <time.h>
#include <fstream>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)
using namespace std;

const int MAXSIZE = 1024;//传输缓冲区最大长度
const unsigned char SYN = 0x1; //001—— FIN = 0 ACK = 0 SYN = 1
const unsigned char ACK = 0x2; //010—— FIN = 0 ACK = 1 SYN = 0
const unsigned char ACK_SYN = 0x3;//011—— FIN = 0 ACK = 1 SYN = 1
const unsigned char FIN = 0x4;//100—— FIN = 1 ACK = 0 SYN = 0
const unsigned char FIN_ACK = 0x5;//101—— FIN = 1 ACK = 0 SYN = 1
const unsigned char OVER = 0x7;//结束标志 111—— FIN = 1 ACK = 1 SYN = 1
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
    u_short sum = 0;//校验和 16位
    u_short datasize = 0;//所包含数据长度 16位
    //八位，使用后四位，排列是FIN ACK SYN 
    unsigned char flag = 0;
    //八位，传输的序列号，0~255，超过后mod
    unsigned char SEQ = 0;
    HEADER() {
        sum = 0;//校验和 16位
        datasize = 0;//所包含数据长度 16位
        flag = 0;//8位，使用后四位，排列是FIN ACK SYN 
        SEQ = 0;//8位
    }
};

<<<<<<< HEAD
int Connect(SOCKET& socketClient, SOCKADDR_IN& servAddr, int& servAddrlen)//三次握手建立连接
{
    HEADER header;
    char* Buffer = new char[sizeof(header)];

    u_short sum;

    //第一次握手
    header.flag = SYN;
    header.sum = 0;//校验和置0
    //计算校验和
    header.sum = 0;
    header.sum = cksum((u_short*)&header, sizeof(header));
    //将数据头放入buffer
    memcpy(Buffer, &header, sizeof(header));
    if (sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen) == -1)
    {
        return -1;
    }
    else
    {
        cout << "[info]成功发送第一次握手数据" << endl;
    }
    clock_t start = clock(); //记录发送第一次握手时间

    //设置socket为非阻塞状态
    u_long mode = 1;
    ioctlsocket(socketClient, FIONBIO, &mode);



    //第二次握手
    while (recvfrom(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, &servAddrlen) <= 0)
    {
        //超时需要重传
        if (clock() - start > MAX_TIME)//超时，重新传输第一次握手
        {
            cout << "[info]第一次握手超时" << endl;
            header.flag = SYN;
            header.sum = 0;//校验和置0
            header.sum = cksum((u_short*)&header, sizeof(header));//计算校验和
            memcpy(Buffer, &header, sizeof(header));//将数据头放入Buffer
            sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
            start = clock();
            cout << "[info]已经重传" << endl;
        }
    }

    //第二次握手，收到来自接收端的ACK
    //进行校验和检验
    memcpy(&header, Buffer, sizeof(header));
    if (header.flag == ACK && cksum((u_short*)&header, sizeof(header)) == 0)
    {
        cout << "[info]接收到第二次握手数据" << endl;
    }
    else
    {
        //cout << "[info]错误数据，请重试" << endl;
        return -1;
    }

    //进行第三次握手
    header.flag = ACK_SYN;
    header.sum = 0;
    header.sum = cksum((u_short*)&header, sizeof(header));//计算校验和
    if (sendto(socketClient, (char*)&header, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen) == -1)
    {
        return -1;
    }
    else
    {
        cout << "[info]成功发送第三次握手数据" << endl;
    }
    cout << "[info]服务器成功连接！可以发送数据" << endl;
    return 1;
}

void send_package(SOCKET& socketClient, SOCKADDR_IN& servAddr, int& servAddrlen, char* message, int len, int& order)
{
    HEADER header;
    char* buffer = new char[MAXSIZE + sizeof(header)];
    header.datasize = len;
    header.SEQ = unsigned char(order);//序列号
    memcpy(buffer, &header, sizeof(header));//将计算好校验和的header重新赋值给buffer，此时的buffer可以发送了
    memcpy(buffer + sizeof(header), message, len);//buffer为header+message
    header.sum = cksum((u_short*)buffer, sizeof(header) + len);//计算校验和
    //发送
    sendto(socketClient, buffer, len + sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
    cout << "[info]发送了" << len << " 字节，" << " flag:" << int(header.flag) 
        << " SEQ:" << int(header.SEQ) << " SUM:" << int(header.sum) << endl;
    clock_t start = clock();//记录发送时间

    //接收ack等信息
    while (1)
    {
        u_long mode = 1;
        ioctlsocket(socketClient, FIONBIO, &mode);//将套接口状态改为非阻塞


        while (recvfrom(socketClient, buffer, MAXSIZE, 0, (sockaddr*)&servAddr, &servAddrlen)<=0) {
            //超时重传
            if (clock() - start > MAX_TIME)
            {
                cout << "[info]发送数据超时" << endl;
                header.datasize = len;
                header.SEQ = u_char(order);//序列号
                header.flag = u_char(0x0);
                memcpy(buffer, &header, sizeof(header));
                memcpy(buffer + sizeof(header), message, sizeof(header) + len);
                u_short check = cksum((u_short*)buffer, sizeof(header) + len);//计算校验和
                header.sum = check;
                memcpy(buffer, &header, sizeof(header));
                sendto(socketClient, buffer, len + sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);//发送
                cout << "[info]重新发送数据" << endl;
                start = clock();//记录发送时间
            }
            else break;
        }
        memcpy(&header, buffer, sizeof(header));//缓冲区接收到信息，放入header，此时header中是收到的数据
        //u_short check = cksum((u_short*)&header, sizeof(header));
        if (cksum((u_short*)&header, sizeof(header)) == 0 && header.SEQ == u_short(order) && header.flag == ACK)
        {
            cout << "[info] 已确认收到 - Flag:" << int(header.flag) 
                << " SEQ:" << int(header.SEQ) << " SUM:" << int(header.sum) << endl;
            break;
        }
        else
        {
=======
    // 鍙戦�佺涓�娆℃彙鎵嬭姹傛姤鏂�
    memset(header, 0, HEADERSIZE);
    // 璁剧疆seq浣�
    int seq = rand();
    header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
    header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
    header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
    header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
    // 璁剧疆SYN浣�
    header[FLAG_BIT_POSITION] = 0b010; // SYN鍦╤eader[8]鐨勭浜屼綅锛屾墍浠ヨ繖涓�琛岃〃绀篠YN == 1
    checksum = checkSum(header, HEADERSIZE);
    // 璁剧疆checksum浣�
    header[CHECKSUM_BITS_START] = (u_char)(checksum & 0xFF);
    header[CHECKSUM_BITS_START + 1] = (u_char)(checksum >> 8);
    sendto(sendSocket, header, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
    cout << "Send the First Handshake message!" << endl;

    // 鎺ュ彈绗簩娆℃彙鎵嬪簲绛旀姤鏂�
    char recvBuf[HEADERSIZE] = { 0 };
    int recvResult = 0;
    while (true) {
        recvResult = recvfrom(sendSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, &len);
        // 鎺ュ彈ack
        int ack = recvBuf[ACK_BITS_START] + (recvBuf[ACK_BITS_START + 1] << 8)
            + (recvBuf[ACK_BITS_START + 2] << 16) + (recvBuf[ACK_BITS_START + 3] << 24);
        if ((ack == seq + 1) && (recvBuf[FLAG_BIT_POSITION] == 0b110)) { // 0b110浠ｈ〃ACK SYN FIN == 110
            cout << "Successfully received the Second Handshake message!" << endl;
            break;
        }
        else {
            cout << "Failed to received the correct Second Handshake message, Handshake failed!" << endl;
            return false;
        }
    }

    // 鍙戦�佺涓夋鎻℃墜璇锋眰鎶ユ枃
    memset(header, 0, HEADERSIZE);
    // 璁剧疆ack浣嶏紝ack = seq of message2 + 1
    int ack = (u_char)recvBuf[SEQ_BITS_START] + ((u_char)recvBuf[SEQ_BITS_START + 1] << 8)
        + ((u_char)recvBuf[SEQ_BITS_START + 2] << 16) + ((u_char)recvBuf[SEQ_BITS_START + 3] << 24) + 1;
    header[ACK_BITS_START] = (u_char)(ack & 0xFF);
    header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
    header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
    header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
    // 璁剧疆ACK浣�
    header[FLAG_BIT_POSITION] = 0b100;
    checksum = checkSum(header, HEADERSIZE);
    // 璁剧疆checksum浣�
    header[CHECKSUM_BITS_START] = (u_char)(checksum & 0xFF);
    header[CHECKSUM_BITS_START + 1] = (u_char)(checksum >> 8);
    sendto(sendSocket, header, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
    cout << "Send the Third Handshake message!" << endl;

    cout << "Handshake successfully!" << endl;
    return true;

}

int hasSent = 0; // 宸插彂閫佺殑鏂囦欢澶у皬
int fileSize = 0;
int sendResult = 0; // 姣忔sendto鍑芥暟鐨勮繑鍥炵粨鏋�
int sendSize = 0; // 姣忔瀹為檯鍙戦�佺殑鎶ユ枃鎬婚暱搴�
int seq = 1, ack = 0; // 鍙戦�佸寘鏃剁殑seq, ack
int base = 1; // 婊戝姩绐楀彛璧峰
int seq_opp = 0, ack_opp = 0; // 鏀跺埌鐨勫闈㈢殑seq, ack
int dataLength = 0; // 姣忔瀹為檯鍙戦�佺殑鏁版嵁閮ㄥ垎闀垮害(= sendSize - HEADERSIZE)
u_short checksum = 0; // 鏍￠獙鍜�
bool resend = false; // 閲嶄紶鏍囧織
char recvBuf[HEADERSIZE] = { 0 }; // 鎺ュ彈鍝嶅簲鎶ユ枃鐨勭紦鍐插尯
int recvResult = 0; // 鎺ュ彈鍝嶅簲鎶ユ枃鐨勮繑鍥炲��
bool finishSend = false; // 鏄惁缁撴潫浜嗕竴涓枃浠剁殑鍙戦��
// char sendWindow[PACKETSIZE * SEND_WINDOW_SIZE] = {0}; // 婊戝姩绐楀彛

bool THREAD_END = false; // 閫氳繃杩欎釜鍙橀噺鍛婅瘔recvRespondThread鍜宼imerThread閫�鍑�
int THREAD_CREAT_FLAG = 1;
int index = 0; // 鐢ㄤ簬鎷晳receive鍙戣繃鏉ョ殑鏈�鍚庝竴涓‘璁ゅ寘涓㈠け锛宻end绔崱鍦╤asSent == fileSize鍐呯殑鍑轰笉鏉ョ殑鍙橀噺

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
    // 鎺ュ彈鍝嶅簲鎶ユ枃鐨勭嚎绋�
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
    ioctlsocket(socketClient, FIONBIO, &mode);//改回阻塞模式
}

<<<<<<< HEAD
void send(SOCKET& socketClient, SOCKADDR_IN& servAddr, int& servAddrlen, char* message, int len)
{
    //需要传送的数据包个数
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
    //发送结束信息
    HEADER header;
    char* Buffer = new char[sizeof(header)];
    header.flag = OVER;
    header.sum = cksum((u_short*)&header, sizeof(header));
    memcpy(Buffer, &header, sizeof(header));
    sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
    cout << "[info]发送OVER信号" << endl;
    clock_t start = clock();
    while (1)
    {
        u_long mode = 1;
        ioctlsocket(socketClient, FIONBIO, &mode);
        //收到包，存在Buffer中
        while (recvfrom(socketClient, Buffer, MAXSIZE, 0, (sockaddr*)&servAddr, &servAddrlen) <= 0)
        {
            //超时，把“send end”数据包重传
            if (clock() - start > MAX_TIME)
            {
                cout << "[info]发送OVER信号超时" << endl;
                char* Buffer = new char[sizeof(header)];
                header.flag = OVER;
                header.sum = cksum((u_short*)&header, sizeof(header));
                memcpy(Buffer, &header, sizeof(header));
                sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
                cout << "[info]已经重发OVER信号" << endl;
                start = clock();
            }
            else break;
        }
        memcpy(&header, Buffer, sizeof(header));//缓冲区接收到信息，读取Buffer里的信息
        u_short check = cksum((u_short*)&header, sizeof(header));
        //收到的数据包为OVER则已经成功接受文件
        if (header.flag == OVER && check == 0)
        {
            cout << "[info]对方已成功接收文件" << endl;
=======
void sendfile(const char* filename) {
    // 璇诲叆鏂囦欢
    ifstream is(filename, ifstream::in | ios::binary);
    is.seekg(0, is.end);
    fileSize = is.tellg();
    is.seekg(0, is.beg);
    char* filebuf;
    filebuf = (char*)calloc(fileSize, sizeof(char));
    is.read(filebuf, fileSize);
    is.close();

    // 鍙戦�佹枃浠跺悕
    memset(sendBuf, 0, PACKETSIZE);
    header[FLAG_BIT_POSITION] = 0b1000;
    strcat((char*)memcpy(sendBuf, header, HEADERSIZE) + HEADERSIZE, filename);
    sendto(sendSocket, sendBuf, PACKETSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));

    // 鍙戦�佹枃浠跺ぇ灏�
    memset(sendBuf, 0, PACKETSIZE);
    header[FLAG_BIT_POSITION] = 0b10000;
    strcat((char*)memcpy(sendBuf, header, HEADERSIZE) + HEADERSIZE, to_string(fileSize).c_str());
    sendto(sendSocket, sendBuf, PACKETSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));

    hasSent = 0; // 宸插彂閫佺殑鏂囦欢澶у皬
    seq = 1, ack = 0; // 鍙戦�佸寘鏃剁殑seq, ack
    base = 1; // 婊戝姩绐楀彛璧峰
    seq_opp = 0, ack_opp = 0;
    resend = false; // 閲嶄紶鏍囧織
    finishSend = false; // 缁撴潫鍙戦�佹爣蹇�

    // 鍙戦�佹枃浠�
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

        // 濡傛灉涓嶉渶瑕侀噸浼狅紝鍒欓渶瑕侀鍏堟鏌ユ粦鍔ㄧ獥鍙ｆ槸鍚︽弧
        if (seq < base + SEND_WINDOW_SIZE) {
            if (hasSent < fileSize) {
                // 濡傛灉娌℃弧锛屽垯璁剧疆header
                // seq = 鍗冲皢鍙戦�佺殑packet搴忓彿
                // ack 涓嶉渶瑕佽缃�

                // 璁剧疆seq浣�
                header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
                header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
                header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
                header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);

                // 璁剧疆ACK浣�
                header[FLAG_BIT_POSITION] = 0b100;

                // 璁剧疆data length浣�
                dataLength = sendSize - HEADERSIZE;
                header[DATA_LENGTH_BITS_START] = dataLength & 0xFF;
                header[DATA_LENGTH_BITS_START + 1] = dataLength >> 8;

                // file涓娆¤琚彂閫佺殑鏁版嵁->dataSegment
                memcpy(dataSegment, filebuf + hasSent, sendSize - HEADERSIZE);
                // header->sendBuf
                memcpy(sendBuf, header, HEADERSIZE);
                // dataSegment->sendBuf锛堜粠sendBuf[10]寮�濮嬶級
                memcpy(sendBuf + HEADERSIZE, dataSegment, sendSize - HEADERSIZE);
                // 璁剧疆checksum浣�
                checksum = checkSum(sendBuf, sendSize);
                header[CHECKSUM_BITS_START] = sendBuf[CHECKSUM_BITS_START] = checksum & 0xFF;
                header[CHECKSUM_BITS_START + 1] = sendBuf[CHECKSUM_BITS_START + 1] = checksum >> 8;
                sendResult = sendto(sendSocket, sendBuf, sendSize, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));

                // 鍙戦�佸畬姣曞悗锛屽鏋渂ase = seq锛岃鏄庡彂鐨勬槸婊戝姩绐楀彛鍐呯殑绗竴涓猵acket锛屽垯鍚姩璁℃椂
                if (base == seq) {
                    start = clock();
                }

                // 鏇存柊鍙戦�侀暱搴﹀拰seq
                hasSent += sendSize - HEADERSIZE;
                seq++;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(TEST_STOPTIME));
        }

        // 濡傛灉hasSent == fileSize锛岃鏄庝笉鐢ㄥ啀鍙戜簡锛屼絾鏄繕涓嶈兘缁撴潫鍙戦�併�傜粨鏉熷彂閫佺殑鏍囧織鍙兘鐢辨帴鏀剁嚎绋嬪憡鐭ワ紝闇�瑕佺‘璁ゆ敹鍒颁簡瀵规柟鐨勬墍鏈堿CK鎵嶈兘缁撴潫鍙戦��
        if (hasSent == fileSize) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            index++;
            if (index == 10) // 濡傛灉浜х敓鍗佹hasSent == fileSize涓旀病鏈夌粨鏉熷彂閫侊紝璇存槑receive绔粨鏉熶簡鎺ユ敹packet鍜屽彂閫乤ck锛屼笖鏈�鍚庝竴涓猘ck涓㈠け浜�
                finishSend = true;
        }

>>>>>>> 3e47a6b2561d05b8b079c9439f659d5a50a4cc84
    }
    u_long mode = 0;
    ioctlsocket(socketClient, FIONBIO, &mode);//改回阻塞模式
}


<<<<<<< HEAD
int disConnect(SOCKET& socketClient, SOCKADDR_IN& servAddr, int& servAddrlen)
{
    HEADER header;
    char* Buffer = new char[sizeof(header)];

    u_short sum;

    //进行第一次挥手
    header.flag = FIN;
    header.sum = 0;//校验和置0
    header.sum = cksum((u_short*)&header, sizeof(header));//计算校验和
    memcpy(Buffer, &header, sizeof(header));//将首部放入缓冲区
    if (sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen) == -1)
    {
        return -1;
    }
    else
    {
        cout << "[info]成功发送第一次挥手数据" << endl;
    }
    clock_t start = clock(); //记录发送第一次挥手时间


    u_long mode = 1;
    ioctlsocket(socketClient, FIONBIO, &mode);//FIONBIO为命令，允许1/禁止0套接口s的非阻塞1/阻塞0模式。

    //接收第二次挥手
    while (recvfrom(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, &servAddrlen) <= 0)
    {
        //超时，重新传输第一次挥手
        if (clock() - start > MAX_TIME)
        {
            cout << "[info]第一次挥手超时" << endl;
            header.flag = FIN;
            header.sum = 0;//校验和置0
            header.sum = cksum((u_short*)&header, sizeof(header));//计算校验和
            memcpy(Buffer, &header, sizeof(header));//将首部放入缓冲区
            sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
            start = clock();
            cout << "[info]已重传第一次挥手数据" << endl;
        }
    }

    //进行校验和检验
    memcpy(&header, Buffer, sizeof(header));
    if (header.flag == ACK && cksum((u_short*)&header, sizeof(header) == 0))
    {
        cout << "[info]接收到第二次挥手数据" << endl;
    }
    else
    {
        //cout << "[info]错误数据，请重试" << endl;
        return -1;
    }

    //进行第三次挥手
    header.flag = FIN_ACK;
    header.sum = 0;
    header.sum = cksum((u_short*)&header, sizeof(header));//计算校验和
    if (sendto(socketClient, (char*)&header, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen) == -1)
    {
        return -1;
    }
    else
    {
        cout << "[info]成功发送第三次挥手数据" << endl;
    }
    start = clock();
    //接收第四次挥手
    while (recvfrom(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, &servAddrlen) <= 0)
    {
        if (clock() - start > MAX_TIME)//超时，重新传输第三次挥手
        {
            cout << "[info]第三次挥手超时" << endl;
            header.flag = FIN;
            header.sum = 0;//校验和置0
            header.sum = cksum((u_short*)&header, sizeof(header));//计算校验和
            memcpy(Buffer, &header, sizeof(header));//将首部放入缓冲区
            sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
            start = clock();
            cout << "[info]已重传第三次挥手数据" << endl;
        }
    }
    cout << "[info]四次挥手结束，连接断开！" << endl;
    return 1;
}


int main()
{
    cout << MAX_TIME << endl;

    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);

    SOCKADDR_IN severAddr;
    SOCKET server;

    severAddr.sin_family = AF_INET;//使用IPV4
    severAddr.sin_port = htons(3939);
    severAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    server = socket(AF_INET, SOCK_DGRAM, 0);
    int len = sizeof(severAddr);
    //建立连接
    if (Connect(server, severAddr, len) == -1)
    {
        return 0;
    }

    //读取文件内容到buffer
    string inputFile;//希望传输的文件名称
    cout << "请输入希望传输的文件名称" << endl;
    cin >> inputFile;
    ifstream fileIN(inputFile.c_str(), ifstream::binary);//以二进制方式打开文件
    char* buffer = new char[10000000];//文件内容
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
    // 鍙戦�佺涓�娆℃尌鎵嬭姹傛姤鏂�
    memset(header, 0, HEADERSIZE);
    // 璁剧疆seq浣�
    seq = rand();
    header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
    header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
    header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
    header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
    // 璁剧疆ACK FIN浣�
    header[FLAG_BIT_POSITION] = 0b101;
    checksum = checkSum(header, HEADERSIZE);
    // 璁剧疆checksum浣�
    header[CHECKSUM_BITS_START] = (u_char)(checksum & 0xFF);
    header[CHECKSUM_BITS_START + 1] = (u_char)(checksum >> 8);
    sendto(sendSocket, header, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
    cout << "Send the First Wavehand message!" << endl;

    // 鎺ユ敹绗簩娆℃尌鎵嬪簲绛旀姤鏂�
    char recvBuf[HEADERSIZE] = { 0 };
    int recvResult = 0;
    while (true) {
        recvResult = recvfrom(sendSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, &len);
        // 鎺ュ彈ack
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

    // 鎺ユ敹绗笁娆℃尌鎵嬭姹傛姤鏂�
    while (true) {
        recvResult = recvfrom(sendSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, &len);
        // 鎺ュ彈ack
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

    // 鍙戦�佺鍥涙鎸ユ墜搴旂瓟鎶ユ枃
    memset(header, 0, HEADERSIZE);
    // 璁剧疆seq浣�
    seq = ack;
    header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
    header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
    header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
    header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
    // 璁剧疆ack浣�
    ack = (u_char)recvBuf[ACK_BITS_START] + ((u_char)recvBuf[ACK_BITS_START + 1] << 8)
        + ((u_char)recvBuf[ACK_BITS_START + 2] << 16) + ((u_char)recvBuf[ACK_BITS_START + 3] << 24) + 1;
    header[ACK_BITS_START] = (u_char)(ack & 0xFF);
    header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
    header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
    header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
    // 璁剧疆ACK浣�
    header[FLAG_BIT_POSITION] = 0b100;
    checksum = checkSum(header, HEADERSIZE);
    // 璁剧疆checksum浣�
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
    //发送文件名
    send(server, severAddr, len, (char*)(inputFile.c_str()), inputFile.length());
    clock_t start1 = clock();
    //发送文件内容（在buffer里）
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


    cout << "[out] 传输总时间为:" << (end1 - start1) / CLOCKS_PER_SEC << "s" << endl;
    cout << "[out] 吞吐率为:" << (((double)i) / ((end1 - start1) / CLOCKS_PER_SEC)) << "byte/s" << endl;
    disConnect(server, severAddr, len);

    system("pause");

}
