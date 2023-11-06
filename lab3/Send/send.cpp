#include <stdio.h>
#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <ws2tcpip.h>


using namespace std;

#define PORT 15000
#define IP "127.0.0.1"
#define PACKETSIZE 1500
#define HEADERSIZE 14
#define DATASIZE (PACKETSIZE-HEADERSIZE)
#define FILE_NAME_MAX_LENGTH 64

// һЩheader�еı�־λ
#define SEQ_BITS_START 0
#define ACK_BITS_START 4
#define FLAG_BIT_POSITION 8
#define DATA_LENGTH_BITS_START 10
#define CHECKSUM_BITS_START 12

WSAData wsd;
SOCKET sendSocket = INVALID_SOCKET;
SOCKADDR_IN recvAddr = { 0 }; // ���ն˵�ַ
SOCKADDR_IN sendAddr = { 0 }; // ���Ͷ˵�ַ
int len = sizeof(recvAddr);

clock_t s, l;
int totalLength = 0;
double totalTime = 0;

// α�ײ�14 byte��Լ����
// 0 1 2 3--32λseq��0--��8λ��3--��8λ����ͬ��
// 4 5 6 7--32λack
// 8--��־λ������λ�ֱ����ACK SYN FIN������λ������λ��ʱ����Թ��ܣ�����˴η��͵����ļ������ļ���С
// 9--���ţ�ȫ0
// 10 11--���ݲ��ֳ���
// 12 13--У���
char header[HEADERSIZE] = { 0 };

char dataSegment[DATASIZE] = { 0 }; // �������ݲ���

char sendBuf[PACKETSIZE] = { 0 }; // header + data

u_short checkSum(const char* input, int length) {
    int count = (length + 1) / 2; // �ж�����16 bits
    u_short* buf = new u_short[count]{ 0 };
    for (int i = 0; i < count; i++) {
        buf[i] = (u_char)input[2 * i] + ((2 * i + 1 < length) ? (u_char)input[2 * i + 1] << 8 : 0);
        // ��������Ԫ���ʽ��Ϊ�˱����ڼ���buf���һλʱ������input[length]��Խ�����
    }

    register u_long sum = 0;
    while (count--) {
        sum += *buf++;
        // ���sum�н�λ�����λ�ع�
        if (sum & 0xFFFF0000) {
            sum &= 0xFFFF;
            sum++;
        }
    }
    return ~(sum & 0xFFFF);
}

bool handshake() {
    u_short checksum = 0;

    // ���͵�һ������������
    memset(header, 0, HEADERSIZE);
    // ����seqλ
    int seq = rand();
    header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
    header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
    header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
    header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
    // ����SYNλ
    header[FLAG_BIT_POSITION] = 0b010; // SYN��header[8]�ĵڶ�λ��������һ�б�ʾSYN == 1
    checksum = checkSum(header, HEADERSIZE);
    // ����checksumλ
    header[CHECKSUM_BITS_START] = (u_char)(checksum & 0xFF);
    header[CHECKSUM_BITS_START + 1] = (u_char)(checksum >> 8);
    sendto(sendSocket, header, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
    cout << "send the First Handshake message!" << endl;

    // ���ܵڶ�������Ӧ����
    char recvBuf[HEADERSIZE] = { 0 };
    int recvResult = 0;
    while (true) {
        recvResult = recvfrom(sendSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, &len);
        // ����ack
        int ack = recvBuf[ACK_BITS_START] + (recvBuf[ACK_BITS_START + 1] << 8)
            + (recvBuf[ACK_BITS_START + 2] << 16) + (recvBuf[ACK_BITS_START + 3] << 24);
        if ((ack == seq + 1) && (recvBuf[FLAG_BIT_POSITION] == 0b110)) { // 0b110����ACK SYN FIN == 110
            cout << "successfully received the Second Handshake message!" << endl;
            break;
        }
        else {
            cout << "failed to received the correct Second Handshake message, Handshake failed!" << endl;
            return false;
        }
    }

    // ���͵���������������
    memset(header, 0, HEADERSIZE);
    // ����ackλ��ack = seq of message2 + 1
    int ack = (u_char)recvBuf[SEQ_BITS_START] + ((u_char)recvBuf[SEQ_BITS_START + 1] << 8)
        + ((u_char)recvBuf[SEQ_BITS_START + 2] << 16) + ((u_char)recvBuf[SEQ_BITS_START + 3] << 24) + 1;
    header[ACK_BITS_START] = (u_char)(ack & 0xFF);
    header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
    header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
    header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
    // ����ACKλ
    header[FLAG_BIT_POSITION] = 0b100;
    checksum = checkSum(header, HEADERSIZE);
    // ����checksumλ
    header[CHECKSUM_BITS_START] = (u_char)(checksum & 0xFF);
    header[CHECKSUM_BITS_START + 1] = (u_char)(checksum >> 8);
    sendto(sendSocket, header, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
    cout << "send the Third Handshake message!" << endl;

    cout << "Handshake successfully!" << endl;
    return true;

}
void sendfile(const char* filename) {
    // �����ļ�
    ifstream is(filename, ifstream::in | ios::binary);
    is.seekg(0, is.end);
    int fileSize = is.tellg();
    is.seekg(0, is.beg);
    char* filebuf;
    filebuf = (char*)calloc(fileSize, sizeof(char));
    is.read(filebuf, fileSize);
    is.close();

    // �����ļ���
    memset(sendBuf, 0, PACKETSIZE);
    header[FLAG_BIT_POSITION] = 0b1000;
    strcat((char*)memcpy(sendBuf, header, HEADERSIZE) + HEADERSIZE, filename);
    sendto(sendSocket, sendBuf, PACKETSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));

    // �����ļ���С
    memset(sendBuf, 0, PACKETSIZE);
    header[FLAG_BIT_POSITION] = 0b10000;
    strcat((char*)memcpy(sendBuf, header, HEADERSIZE) + HEADERSIZE, to_string(fileSize).c_str());
    sendto(sendSocket, sendBuf, PACKETSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));

    int hasSent = 0; // �ѷ��͵��ļ���С
    int sendResult = 0; // ÿ��sendto�����ķ��ؽ��
    int sendSize = 0; // ÿ��ʵ�ʷ��͵ı����ܳ���
    int seq = 0, ack = 0; // ���Ͱ�ʱ��seq, ack
    int seq_opp = 0, ack_opp = 0; // �յ��Ķ����seq, ack
    int dataLength = 0; // ÿ��ʵ�ʷ��͵����ݲ��ֳ���(= sendSize - HEADERSIZE)
    u_short checksum = 0; // У���
    bool resend = false; // �ش���־
    char recvBuf[HEADERSIZE] = { 0 }; // ������Ӧ���ĵĻ�����
    int recvResult = 0; // ������Ӧ���ĵķ���ֵ

    // �����ļ�
    while (true) {
        // ��ʼ��ͷ�������ݶ�
        memset(header, 0, HEADERSIZE);
        memset(dataSegment, 0, DATASIZE);
        memset(sendBuf, 0, PACKETSIZE);
        // ���ñ��η��ͳ���
        sendSize = min(PACKETSIZE, fileSize - hasSent + HEADERSIZE);

        if (!resend) {
            // ��������ش�����Ҫ����header
            // seq = �յ��İ���ack����ʾ������Ҫ�����ֽ�λ��
            // ack = �յ��İ���seq + �յ���data length��Ȼ��receive����data length��ԶΪ0��
            // ����seqλ
            seq = ack_opp;
            header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
            header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
            header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
            header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
            // ����ackλ
            ack = seq_opp;
            header[ACK_BITS_START] = (u_char)(ack & 0xFF);
            header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
            header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
            header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
            // ����ACKλ
            header[FLAG_BIT_POSITION] = 0b100;
            // ����data lengthλ
            dataLength = sendSize - HEADERSIZE;
            header[DATA_LENGTH_BITS_START] = dataLength & 0xFF;
            header[DATA_LENGTH_BITS_START + 1] = dataLength >> 8;

            // file�д˴�Ҫ�����͵�����->dataSegment
            memcpy(dataSegment, filebuf + hasSent, sendSize - HEADERSIZE);
            // header->sendBuf
            memcpy(sendBuf, header, HEADERSIZE);
            // dataSegment->sendBuf����sendBuf[10]��ʼ��
            memcpy(sendBuf + HEADERSIZE, dataSegment, sendSize - HEADERSIZE);
            // ����checksumλ
            checksum = checkSum(sendBuf, sendSize);
            header[CHECKSUM_BITS_START] = sendBuf[CHECKSUM_BITS_START] = checksum & 0xFF;
            header[CHECKSUM_BITS_START + 1] = sendBuf[CHECKSUM_BITS_START + 1] = checksum >> 8;

            sendResult = sendto(sendSocket, sendBuf, sendSize, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
        }
        else {
            // ������ش�������Ҫ����header���ٷ�һ�μ���
            sendResult = sendto(sendSocket, sendBuf, sendSize, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
        }

        // ����packet�������Ӧ����
        while (true) {
            // TODO: �����ʱ��û�յ���Ӧ���ģ���break���ش�
            recvResult = recvfrom(sendSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, &len);
            if (recvResult == SOCKET_ERROR) {
                cout << "receive error! sleep!" << endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                continue;
            }
            seq_opp = (u_char)recvBuf[SEQ_BITS_START] + ((u_char)recvBuf[SEQ_BITS_START + 1] << 8)
                + ((u_char)recvBuf[SEQ_BITS_START + 2] << 16) + ((u_char)recvBuf[SEQ_BITS_START + 3] << 24);
            ack_opp = (u_char)recvBuf[ACK_BITS_START] + ((u_char)recvBuf[ACK_BITS_START + 1] << 8)
                + ((u_char)recvBuf[ACK_BITS_START + 2] << 16) + ((u_char)recvBuf[ACK_BITS_START + 3] << 24);

            if (recvBuf[FLAG_BIT_POSITION] == 0b100 && seq == seq_opp) {
                // ��Ϊ���շ�û��Ҫ���͵����ݣ�������Ӧ�������seq����ͳTCP/UDPЭ����˵һֱΪ0�������ҵ�Э�������Ӧ���ĵ�seq�óɷ��ͱ��ĵ�seq������ȷ��
                // �Է���ȷ�յ������packet
                resend = false;
                hasSent += sendSize - HEADERSIZE;
                // cout << "send seq = " << seq << " packet successfully!" << endl;
                break;
            }
            else {
                resend = true;
                cout << "failed to send seq = " << seq << " packet! This packet will be resent." << endl;
                break;
            }
        }
        // std::this_thread::sleep_for(std::chrono::microseconds(500));

        if (hasSent == fileSize) {
            cout << "send successfully, send " << fileSize << " bytes." << endl;
            totalLength += fileSize;
            break;
        }
    }
}

void wavehand() {
    int seq = 0, ack = 0;
    u_short checksum = 0;
    // ���͵�һ�λ���������
    memset(header, 0, HEADERSIZE);
    // ����seqλ
    seq = rand();
    header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
    header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
    header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
    header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
    // ����ACK FINλ
    header[FLAG_BIT_POSITION] = 0b101;
    checksum = checkSum(header, HEADERSIZE);
    // ����checksumλ
    header[CHECKSUM_BITS_START] = (u_char)(checksum & 0xFF);
    header[CHECKSUM_BITS_START + 1] = (u_char)(checksum >> 8);
    sendto(sendSocket, header, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
    cout << "send the First Wavehand message!" << endl;

    // ���յڶ��λ���Ӧ����
    char recvBuf[HEADERSIZE] = { 0 };
    int recvResult = 0;
    while (true) {
        recvResult = recvfrom(sendSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, &len);
        // ����ack
        ack = recvBuf[ACK_BITS_START] + (recvBuf[ACK_BITS_START + 1] << 8)
            + (recvBuf[ACK_BITS_START + 2] << 16) + (recvBuf[ACK_BITS_START + 3] << 24);
        if ((ack == seq + 1) && (recvBuf[FLAG_BIT_POSITION] == 0b100)) {
            cout << "successfully received the Second Wavehand message!" << endl;
            break;
        }
        else {
            cout << "failed to received the correct Second Wavehand message, Wavehand failed!" << endl;
            return;
        }
    }

    // ���յ����λ���������
    while (true) {
        recvResult = recvfrom(sendSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, &len);
        // ����ack
        ack = recvBuf[ACK_BITS_START] + (recvBuf[ACK_BITS_START + 1] << 8)
            + (recvBuf[ACK_BITS_START + 2] << 16) + (recvBuf[ACK_BITS_START + 3] << 24);
        if ((ack == seq + 1) && (recvBuf[FLAG_BIT_POSITION] == 0b101)) {
            cout << "successfully received the Third Wavehand message!" << endl;
            break;
        }
        else {
            cout << "failed to received the correct Third Wavehand message, Wavehand failed!" << endl;
            return;
        }
    }

    // ���͵��Ĵλ���Ӧ����
    memset(header, 0, HEADERSIZE);
    // ����seqλ
    seq = ack;
    header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
    header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
    header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
    header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
    // ����ackλ
    ack = (u_char)recvBuf[ACK_BITS_START] + ((u_char)recvBuf[ACK_BITS_START + 1] << 8)
        + ((u_char)recvBuf[ACK_BITS_START + 2] << 16) + ((u_char)recvBuf[ACK_BITS_START + 3] << 24) + 1;
    header[ACK_BITS_START] = (u_char)(ack & 0xFF);
    header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
    header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
    header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
    // ����ACKλ
    header[FLAG_BIT_POSITION] = 0b100;
    checksum = checkSum(header, HEADERSIZE);
    // ����checksumλ
    header[CHECKSUM_BITS_START] = (u_char)(checksum & 0xFF);
    header[CHECKSUM_BITS_START + 1] = (u_char)(checksum >> 8);
    sendto(sendSocket, header, HEADERSIZE, 0, (SOCKADDR*)&recvAddr, sizeof(SOCKADDR));
    cout << "send the Forth Wavehand message!" << endl;

    cout << "Wavehand successfully!" << endl;
    return;
}

int main() {
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
        cout << "WSAStartup error = " << WSAGetLastError() << endl;
        exit(1);
    }
    else {
        cout << "start success" << endl;
    }

    sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sendSocket == SOCKET_ERROR) {
        cout << "socket error = " << WSAGetLastError() << endl;
        closesocket(sendSocket);
        WSACleanup();
        exit(1);
    }
    else {
        cout << "socket success" << endl;
    }

    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(PORT);
    recvAddr.sin_addr.s_addr = inet_addr(IP);
    sendAddr.sin_family = AF_INET;
    sendAddr.sin_port = htons(PORT);
    sendAddr.sin_addr.s_addr = inet_addr(IP);

    if (handshake()) {
        while (true) {
            string str;
            cout << "please input the file name(or q to quit sending): ";
            cin >> str;
            if (str == "q") {
                break;
            }
            else {
                s = clock();
                thread sendfile_thread(sendfile, str.c_str());
                sendfile_thread.join();
                l = clock();
                totalTime += (double)(l - s) / CLOCKS_PER_SEC;
            }
        }
        wavehand();
        cout << endl << "send time: " << totalTime << " s." << endl;
        cout << "total size: " << totalLength << " Bytes." << endl;
        cout << "throughput: " << (double)((totalLength * 8 / 1000) / totalTime) << " kbps." << endl;
    }

    closesocket(sendSocket);
    WSACleanup();
    return 0;
}