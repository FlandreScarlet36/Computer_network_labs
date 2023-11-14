#include <iostream>
#include <WINSOCK2.h>
#include <time.h>
#include <fstream>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)
using namespace std;

const int MAXSIZE = 1024;//���仺������󳤶�
const unsigned char SYN = 0x1; //001���� FIN = 0 ACK = 0 SYN = 1
const unsigned char ACK = 0x2; //010���� FIN = 0 ACK = 1 SYN = 0
const unsigned char ACK_SYN = 0x3;//011���� FIN = 0 ACK = 1 SYN = 1
const unsigned char FIN = 0x4;//100���� FIN = 1 ACK = 0 SYN = 0
const unsigned char FIN_ACK = 0x5;//101���� FIN = 1 ACK = 0 SYN = 1
const unsigned char OVER = 0x7;//������־ 111���� FIN = 1 ACK = 1 SYN = 1
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
    u_short sum = 0;//У��� 16λ
    u_short datasize = 0;//���������ݳ��� 16λ
    //��λ��ʹ�ú���λ��������FIN ACK SYN 
    unsigned char flag = 0;
    //��λ����������кţ�0~255��������mod
    unsigned char SEQ = 0;
    HEADER() {
        sum = 0;//У��� 16λ
        datasize = 0;//���������ݳ��� 16λ
        flag = 0;//8λ��ʹ�ú���λ��������FIN ACK SYN 
        SEQ = 0;//8λ
    }
};

<<<<<<< HEAD
int Connect(SOCKET& socketClient, SOCKADDR_IN& servAddr, int& servAddrlen)//�������ֽ�������
{
    HEADER header;
    char* Buffer = new char[sizeof(header)];

    u_short sum;

    //��һ������
    header.flag = SYN;
    header.sum = 0;//У�����0
    //����У���
    header.sum = 0;
    header.sum = cksum((u_short*)&header, sizeof(header));
    //������ͷ����buffer
    memcpy(Buffer, &header, sizeof(header));
    if (sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen) == -1)
    {
        return -1;
    }
    else
    {
        cout << "[info]�ɹ����͵�һ����������" << endl;
    }
    clock_t start = clock(); //��¼���͵�һ������ʱ��

    //����socketΪ������״̬
    u_long mode = 1;
    ioctlsocket(socketClient, FIONBIO, &mode);



    //�ڶ�������
    while (recvfrom(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, &servAddrlen) <= 0)
    {
        //��ʱ��Ҫ�ش�
        if (clock() - start > MAX_TIME)//��ʱ�����´����һ������
        {
            cout << "[info]��һ�����ֳ�ʱ" << endl;
            header.flag = SYN;
            header.sum = 0;//У�����0
            header.sum = cksum((u_short*)&header, sizeof(header));//����У���
            memcpy(Buffer, &header, sizeof(header));//������ͷ����Buffer
            sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
            start = clock();
            cout << "[info]�Ѿ��ش�" << endl;
        }
    }

    //�ڶ������֣��յ����Խ��ն˵�ACK
    //����У��ͼ���
    memcpy(&header, Buffer, sizeof(header));
    if (header.flag == ACK && cksum((u_short*)&header, sizeof(header)) == 0)
    {
        cout << "[info]���յ��ڶ�����������" << endl;
    }
    else
    {
        //cout << "[info]�������ݣ�������" << endl;
        return -1;
    }

    //���е���������
    header.flag = ACK_SYN;
    header.sum = 0;
    header.sum = cksum((u_short*)&header, sizeof(header));//����У���
    if (sendto(socketClient, (char*)&header, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen) == -1)
    {
        return -1;
    }
    else
    {
        cout << "[info]�ɹ����͵�������������" << endl;
    }
    cout << "[info]�������ɹ����ӣ����Է�������" << endl;
    return 1;
}

void send_package(SOCKET& socketClient, SOCKADDR_IN& servAddr, int& servAddrlen, char* message, int len, int& order)
{
    HEADER header;
    char* buffer = new char[MAXSIZE + sizeof(header)];
    header.datasize = len;
    header.SEQ = unsigned char(order);//���к�
    memcpy(buffer, &header, sizeof(header));//�������У��͵�header���¸�ֵ��buffer����ʱ��buffer���Է�����
    memcpy(buffer + sizeof(header), message, len);//bufferΪheader+message
    header.sum = cksum((u_short*)buffer, sizeof(header) + len);//����У���
    //����
    sendto(socketClient, buffer, len + sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
    cout << "[info]������" << len << " �ֽڣ�" << " flag:" << int(header.flag) 
        << " SEQ:" << int(header.SEQ) << " SUM:" << int(header.sum) << endl;
    clock_t start = clock();//��¼����ʱ��

    //����ack����Ϣ
    while (1)
    {
        u_long mode = 1;
        ioctlsocket(socketClient, FIONBIO, &mode);//���׽ӿ�״̬��Ϊ������


        while (recvfrom(socketClient, buffer, MAXSIZE, 0, (sockaddr*)&servAddr, &servAddrlen)<=0) {
            //��ʱ�ش�
            if (clock() - start > MAX_TIME)
            {
                cout << "[info]�������ݳ�ʱ" << endl;
                header.datasize = len;
                header.SEQ = u_char(order);//���к�
                header.flag = u_char(0x0);
                memcpy(buffer, &header, sizeof(header));
                memcpy(buffer + sizeof(header), message, sizeof(header) + len);
                u_short check = cksum((u_short*)buffer, sizeof(header) + len);//����У���
                header.sum = check;
                memcpy(buffer, &header, sizeof(header));
                sendto(socketClient, buffer, len + sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);//����
                cout << "[info]���·�������" << endl;
                start = clock();//��¼����ʱ��
            }
            else break;
        }
        memcpy(&header, buffer, sizeof(header));//���������յ���Ϣ������header����ʱheader�����յ�������
        //u_short check = cksum((u_short*)&header, sizeof(header));
        if (cksum((u_short*)&header, sizeof(header)) == 0 && header.SEQ == u_short(order) && header.flag == ACK)
        {
            cout << "[info] ��ȷ���յ� - Flag:" << int(header.flag) 
                << " SEQ:" << int(header.SEQ) << " SUM:" << int(header.sum) << endl;
            break;
        }
        else
        {
=======
    // 发送第一次握手请求报文
    memset(header, 0, HEADERSIZE);
    // 设置seq位
    int seq = rand();
    header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
    header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
    header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
    header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
    // 设置SYN位
    header[FLAG_BIT_POSITION] = 0b010; // SYN在header[8]的第二位，所以这一行表示SYN == 1
    checksum = checkSum(header, HEADERSIZE);
    // 设置checksum位
    header[CHECKSUM_BITS_START] = (u_char)(checksum & 0xFF);
    header[CHECKSUM_BITS_START + 1] = (u_char)(checksum >> 8);
    sendto(sendSocket, header, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
    cout << "Send the First Handshake message!" << endl;

    // 接受第二次握手应答报文
    char recvBuf[HEADERSIZE] = { 0 };
    int recvResult = 0;
    while (true) {
        recvResult = recvfrom(sendSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, &len);
        // 接受ack
        int ack = recvBuf[ACK_BITS_START] + (recvBuf[ACK_BITS_START + 1] << 8)
            + (recvBuf[ACK_BITS_START + 2] << 16) + (recvBuf[ACK_BITS_START + 3] << 24);
        if ((ack == seq + 1) && (recvBuf[FLAG_BIT_POSITION] == 0b110)) { // 0b110代表ACK SYN FIN == 110
            cout << "Successfully received the Second Handshake message!" << endl;
            break;
        }
        else {
            cout << "Failed to received the correct Second Handshake message, Handshake failed!" << endl;
            return false;
        }
    }

    // 发送第三次握手请求报文
    memset(header, 0, HEADERSIZE);
    // 设置ack位，ack = seq of message2 + 1
    int ack = (u_char)recvBuf[SEQ_BITS_START] + ((u_char)recvBuf[SEQ_BITS_START + 1] << 8)
        + ((u_char)recvBuf[SEQ_BITS_START + 2] << 16) + ((u_char)recvBuf[SEQ_BITS_START + 3] << 24) + 1;
    header[ACK_BITS_START] = (u_char)(ack & 0xFF);
    header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
    header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
    header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
    // 设置ACK位
    header[FLAG_BIT_POSITION] = 0b100;
    checksum = checkSum(header, HEADERSIZE);
    // 设置checksum位
    header[CHECKSUM_BITS_START] = (u_char)(checksum & 0xFF);
    header[CHECKSUM_BITS_START + 1] = (u_char)(checksum >> 8);
    sendto(sendSocket, header, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
    cout << "Send the Third Handshake message!" << endl;

    cout << "Handshake successfully!" << endl;
    return true;

}

int hasSent = 0; // 已发送的文件大小
int fileSize = 0;
int sendResult = 0; // 每次sendto函数的返回结果
int sendSize = 0; // 每次实际发送的报文总长度
int seq = 1, ack = 0; // 发送包时的seq, ack
int base = 1; // 滑动窗口起始
int seq_opp = 0, ack_opp = 0; // 收到的对面的seq, ack
int dataLength = 0; // 每次实际发送的数据部分长度(= sendSize - HEADERSIZE)
u_short checksum = 0; // 校验和
bool resend = false; // 重传标志
char recvBuf[HEADERSIZE] = { 0 }; // 接受响应报文的缓冲区
int recvResult = 0; // 接受响应报文的返回值
bool finishSend = false; // 是否结束了一个文件的发送
// char sendWindow[PACKETSIZE * SEND_WINDOW_SIZE] = {0}; // 滑动窗口

bool THREAD_END = false; // 通过这个变量告诉recvRespondThread和timerThread退出
int THREAD_CREAT_FLAG = 1;
int index = 0; // 用于拯救receive发过来的最后一个确认包丢失，send端卡在hasSent == fileSize内的出不来的变量

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
    // 接受响应报文的线程
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
    ioctlsocket(socketClient, FIONBIO, &mode);//�Ļ�����ģʽ
}

<<<<<<< HEAD
void send(SOCKET& socketClient, SOCKADDR_IN& servAddr, int& servAddrlen, char* message, int len)
{
    //��Ҫ���͵����ݰ�����
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
    //���ͽ�����Ϣ
    HEADER header;
    char* Buffer = new char[sizeof(header)];
    header.flag = OVER;
    header.sum = cksum((u_short*)&header, sizeof(header));
    memcpy(Buffer, &header, sizeof(header));
    sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
    cout << "[info]����OVER�ź�" << endl;
    clock_t start = clock();
    while (1)
    {
        u_long mode = 1;
        ioctlsocket(socketClient, FIONBIO, &mode);
        //�յ���������Buffer��
        while (recvfrom(socketClient, Buffer, MAXSIZE, 0, (sockaddr*)&servAddr, &servAddrlen) <= 0)
        {
            //��ʱ���ѡ�send end�����ݰ��ش�
            if (clock() - start > MAX_TIME)
            {
                cout << "[info]����OVER�źų�ʱ" << endl;
                char* Buffer = new char[sizeof(header)];
                header.flag = OVER;
                header.sum = cksum((u_short*)&header, sizeof(header));
                memcpy(Buffer, &header, sizeof(header));
                sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
                cout << "[info]�Ѿ��ط�OVER�ź�" << endl;
                start = clock();
            }
            else break;
        }
        memcpy(&header, Buffer, sizeof(header));//���������յ���Ϣ����ȡBuffer�����Ϣ
        u_short check = cksum((u_short*)&header, sizeof(header));
        //�յ������ݰ�ΪOVER���Ѿ��ɹ������ļ�
        if (header.flag == OVER && check == 0)
        {
            cout << "[info]�Է��ѳɹ������ļ�" << endl;
=======
void sendfile(const char* filename) {
    // 读入文件
    ifstream is(filename, ifstream::in | ios::binary);
    is.seekg(0, is.end);
    fileSize = is.tellg();
    is.seekg(0, is.beg);
    char* filebuf;
    filebuf = (char*)calloc(fileSize, sizeof(char));
    is.read(filebuf, fileSize);
    is.close();

    // 发送文件名
    memset(sendBuf, 0, PACKETSIZE);
    header[FLAG_BIT_POSITION] = 0b1000;
    strcat((char*)memcpy(sendBuf, header, HEADERSIZE) + HEADERSIZE, filename);
    sendto(sendSocket, sendBuf, PACKETSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));

    // 发送文件大小
    memset(sendBuf, 0, PACKETSIZE);
    header[FLAG_BIT_POSITION] = 0b10000;
    strcat((char*)memcpy(sendBuf, header, HEADERSIZE) + HEADERSIZE, to_string(fileSize).c_str());
    sendto(sendSocket, sendBuf, PACKETSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));

    hasSent = 0; // 已发送的文件大小
    seq = 1, ack = 0; // 发送包时的seq, ack
    base = 1; // 滑动窗口起始
    seq_opp = 0, ack_opp = 0;
    resend = false; // 重传标志
    finishSend = false; // 结束发送标志

    // 发送文件
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

        // 如果不需要重传，则需要首先检查滑动窗口是否满
        if (seq < base + SEND_WINDOW_SIZE) {
            if (hasSent < fileSize) {
                // 如果没满，则设置header
                // seq = 即将发送的packet序号
                // ack 不需要设置

                // 设置seq位
                header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
                header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
                header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
                header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);

                // 设置ACK位
                header[FLAG_BIT_POSITION] = 0b100;

                // 设置data length位
                dataLength = sendSize - HEADERSIZE;
                header[DATA_LENGTH_BITS_START] = dataLength & 0xFF;
                header[DATA_LENGTH_BITS_START + 1] = dataLength >> 8;

                // file中此次要被发送的数据->dataSegment
                memcpy(dataSegment, filebuf + hasSent, sendSize - HEADERSIZE);
                // header->sendBuf
                memcpy(sendBuf, header, HEADERSIZE);
                // dataSegment->sendBuf（从sendBuf[10]开始）
                memcpy(sendBuf + HEADERSIZE, dataSegment, sendSize - HEADERSIZE);
                // 设置checksum位
                checksum = checkSum(sendBuf, sendSize);
                header[CHECKSUM_BITS_START] = sendBuf[CHECKSUM_BITS_START] = checksum & 0xFF;
                header[CHECKSUM_BITS_START + 1] = sendBuf[CHECKSUM_BITS_START + 1] = checksum >> 8;
                sendResult = sendto(sendSocket, sendBuf, sendSize, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));

                // 发送完毕后，如果base = seq，说明发的是滑动窗口内的第一个packet，则启动计时
                if (base == seq) {
                    start = clock();
                }

                // 更新发送长度和seq
                hasSent += sendSize - HEADERSIZE;
                seq++;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(TEST_STOPTIME));
        }

        // 如果hasSent == fileSize，说明不用再发了，但是还不能结束发送。结束发送的标志只能由接收线程告知，需要确认收到了对方的所有ACK才能结束发送
        if (hasSent == fileSize) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            index++;
            if (index == 10) // 如果产生十次hasSent == fileSize且没有结束发送，说明receive端结束了接收packet和发送ack，且最后一个ack丢失了
                finishSend = true;
        }

>>>>>>> 3e47a6b2561d05b8b079c9439f659d5a50a4cc84
    }
    u_long mode = 0;
    ioctlsocket(socketClient, FIONBIO, &mode);//�Ļ�����ģʽ
}


<<<<<<< HEAD
int disConnect(SOCKET& socketClient, SOCKADDR_IN& servAddr, int& servAddrlen)
{
    HEADER header;
    char* Buffer = new char[sizeof(header)];

    u_short sum;

    //���е�һ�λ���
    header.flag = FIN;
    header.sum = 0;//У�����0
    header.sum = cksum((u_short*)&header, sizeof(header));//����У���
    memcpy(Buffer, &header, sizeof(header));//���ײ����뻺����
    if (sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen) == -1)
    {
        return -1;
    }
    else
    {
        cout << "[info]�ɹ����͵�һ�λ�������" << endl;
    }
    clock_t start = clock(); //��¼���͵�һ�λ���ʱ��


    u_long mode = 1;
    ioctlsocket(socketClient, FIONBIO, &mode);//FIONBIOΪ�������1/��ֹ0�׽ӿ�s�ķ�����1/����0ģʽ��

    //���յڶ��λ���
    while (recvfrom(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, &servAddrlen) <= 0)
    {
        //��ʱ�����´����һ�λ���
        if (clock() - start > MAX_TIME)
        {
            cout << "[info]��һ�λ��ֳ�ʱ" << endl;
            header.flag = FIN;
            header.sum = 0;//У�����0
            header.sum = cksum((u_short*)&header, sizeof(header));//����У���
            memcpy(Buffer, &header, sizeof(header));//���ײ����뻺����
            sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
            start = clock();
            cout << "[info]���ش���һ�λ�������" << endl;
        }
    }

    //����У��ͼ���
    memcpy(&header, Buffer, sizeof(header));
    if (header.flag == ACK && cksum((u_short*)&header, sizeof(header) == 0))
    {
        cout << "[info]���յ��ڶ��λ�������" << endl;
    }
    else
    {
        //cout << "[info]�������ݣ�������" << endl;
        return -1;
    }

    //���е����λ���
    header.flag = FIN_ACK;
    header.sum = 0;
    header.sum = cksum((u_short*)&header, sizeof(header));//����У���
    if (sendto(socketClient, (char*)&header, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen) == -1)
    {
        return -1;
    }
    else
    {
        cout << "[info]�ɹ����͵����λ�������" << endl;
    }
    start = clock();
    //���յ��Ĵλ���
    while (recvfrom(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, &servAddrlen) <= 0)
    {
        if (clock() - start > MAX_TIME)//��ʱ�����´�������λ���
        {
            cout << "[info]�����λ��ֳ�ʱ" << endl;
            header.flag = FIN;
            header.sum = 0;//У�����0
            header.sum = cksum((u_short*)&header, sizeof(header));//����У���
            memcpy(Buffer, &header, sizeof(header));//���ײ����뻺����
            sendto(socketClient, Buffer, sizeof(header), 0, (sockaddr*)&servAddr, servAddrlen);
            start = clock();
            cout << "[info]���ش������λ�������" << endl;
        }
    }
    cout << "[info]�Ĵλ��ֽ��������ӶϿ���" << endl;
    return 1;
}


int main()
{
    cout << MAX_TIME << endl;

    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);

    SOCKADDR_IN severAddr;
    SOCKET server;

    severAddr.sin_family = AF_INET;//ʹ��IPV4
    severAddr.sin_port = htons(3939);
    severAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    server = socket(AF_INET, SOCK_DGRAM, 0);
    int len = sizeof(severAddr);
    //��������
    if (Connect(server, severAddr, len) == -1)
    {
        return 0;
    }

    //��ȡ�ļ����ݵ�buffer
    string inputFile;//ϣ��������ļ�����
    cout << "������ϣ��������ļ�����" << endl;
    cin >> inputFile;
    ifstream fileIN(inputFile.c_str(), ifstream::binary);//�Զ����Ʒ�ʽ���ļ�
    char* buffer = new char[10000000];//�ļ�����
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
    // 发送第一次挥手请求报文
    memset(header, 0, HEADERSIZE);
    // 设置seq位
    seq = rand();
    header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
    header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
    header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
    header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
    // 设置ACK FIN位
    header[FLAG_BIT_POSITION] = 0b101;
    checksum = checkSum(header, HEADERSIZE);
    // 设置checksum位
    header[CHECKSUM_BITS_START] = (u_char)(checksum & 0xFF);
    header[CHECKSUM_BITS_START + 1] = (u_char)(checksum >> 8);
    sendto(sendSocket, header, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
    cout << "Send the First Wavehand message!" << endl;

    // 接收第二次挥手应答报文
    char recvBuf[HEADERSIZE] = { 0 };
    int recvResult = 0;
    while (true) {
        recvResult = recvfrom(sendSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, &len);
        // 接受ack
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

    // 接收第三次挥手请求报文
    while (true) {
        recvResult = recvfrom(sendSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, &len);
        // 接受ack
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

    // 发送第四次挥手应答报文
    memset(header, 0, HEADERSIZE);
    // 设置seq位
    seq = ack;
    header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
    header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
    header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
    header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
    // 设置ack位
    ack = (u_char)recvBuf[ACK_BITS_START] + ((u_char)recvBuf[ACK_BITS_START + 1] << 8)
        + ((u_char)recvBuf[ACK_BITS_START + 2] << 16) + ((u_char)recvBuf[ACK_BITS_START + 3] << 24) + 1;
    header[ACK_BITS_START] = (u_char)(ack & 0xFF);
    header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
    header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
    header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
    // 设置ACK位
    header[FLAG_BIT_POSITION] = 0b100;
    checksum = checkSum(header, HEADERSIZE);
    // 设置checksum位
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
    //�����ļ���
    send(server, severAddr, len, (char*)(inputFile.c_str()), inputFile.length());
    clock_t start1 = clock();
    //�����ļ����ݣ���buffer�
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


    cout << "[out] ������ʱ��Ϊ:" << (end1 - start1) / CLOCKS_PER_SEC << "s" << endl;
    cout << "[out] ������Ϊ:" << (((double)i) / ((end1 - start1) / CLOCKS_PER_SEC)) << "byte/s" << endl;
    disConnect(server, severAddr, len);

    system("pause");

}
