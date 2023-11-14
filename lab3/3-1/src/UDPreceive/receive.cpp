
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

u_short checkSum(u_short* mes, int size) {
    int count = (size + 1) / 2;
    //buffer�൱��һ��Ԫ��Ϊu_short���͵����飬ÿ��Ԫ��16λ���൱����У��͹����е�һ��Ԫ��
    u_short* buf = (u_short*)malloc(size + 1);
    memset(buf, 0, size + 1);
    memcpy(buf, mes, size);//��message����buf
    u_long sum = 0;
    while (count--) {
        sum += *buf++;
        //����н�λ�򽫽�λ�ӵ����λ
        if (sum & 0xffff0000) {
            sum &= 0xffff;
            sum++;
        }
    }
    //ȡ��
    return ~(sum & 0xffff);
}

struct HEADER
{
    u_short sum = 0;//У��� 16λ
    u_short datasize = 0;//���������ݳ��� 16λ
    unsigned char flag = 0;
    //��λ��ʹ�ú���λ��������FIN ACK SYN 
    unsigned char SEQ = 0;
    //��λ����������кţ�0~255��������mod
    HEADER() {
        sum = 0;//У���    16λ
        datasize = 0;//���������ݳ���     16λ
        flag = 0;//8λ��ʹ�ú���λ��������FIN ACK SYN 
        SEQ = 0;//8λ
    }
};

int Connect(SOCKET& sockServ, SOCKADDR_IN& ClientAddr, int& ClientAddrLen)
{
    HEADER header;
    char* Buffer = new char[sizeof(header)];

    //���յ�һ��������Ϣ
    while (1)
    {
        //ͨ���󶨵�socket���ݡ���������
        if (recvfrom(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, &ClientAddrLen) == -1)
        {
            return -1;
        }
        memcpy(&header, Buffer, sizeof(header));
        if (header.flag == SYN && checkSum((u_short*)&header, sizeof(header)) == 0){
            cout << "[info]���յ���һ���������� " << endl;
            break;
        }
    }
    //���͵ڶ���������Ϣ
    header.flag = ACK;
    header.sum = 0;
    header.sum = checkSum((u_short*)&header, sizeof(header));
    memcpy(Buffer, &header, sizeof(header));

    if (sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen) == -1)
    {
        return -1;
    }
    else
    {
        cout << "[info]�ɹ����͵ڶ����������� " << endl;
    }
    clock_t start = clock();//��¼�ڶ������ַ���ʱ��

    //���յ���������
    while (recvfrom(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, &ClientAddrLen) <= 0)
    {
        //��ʱ�ش�
        if (clock() - start > MAX_TIME)
        {
            cout << "[info]�ڶ������ֳ�ʱ " << endl;
            header.flag = ACK;
            header.sum = 0;
            header.flag = checkSum((u_short*)&header, sizeof(header));
            memcpy(Buffer, &header, sizeof(header));
            if (sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen) == -1)
            {
                return -1;
            }
            cout << "[info]�Ѿ��ش� " << endl;
        }
    }

    //�����յ��ĵ��������ֵ����ݰ�
    HEADER temp1;
    memcpy(&temp1, Buffer, sizeof(header));
    
    if (temp1.flag == ACK_SYN && checkSum((u_short*)&temp1, sizeof(temp1)) == 0)
    {
        cout << "[info]���յ���������������" << endl;
        cout << "[info]�ɹ�����" << endl;
    }
    else
    {
        //cout << "[info]�������ݣ�������" << endl;
    }
    return 1;
}
int RecvMessage(SOCKET& sockServ, SOCKADDR_IN& ClientAddr, int& ClientAddrLen, char* message)
{
    long int fileLength = 0;//�ļ�����
    HEADER header;
    char* Buffer = new char[MAXSIZE + sizeof(header)];
    int seq = 0;
    int index = 0;

    while (1)
    {
        int length = recvfrom(sockServ, Buffer, sizeof(header) + MAXSIZE, 0, (sockaddr*)&ClientAddr, &ClientAddrLen);//���ձ��ĳ���
        //cout << length << endl;
        memcpy(&header, Buffer, sizeof(header));
        //�ж��Ƿ��ǽ���
        if (header.flag == OVER && checkSum((u_short*)&header, sizeof(header)) == 0)
        {
            cout << "[info]�ļ��������" << endl;
            break;
        }
        if (header.flag == unsigned char(0) && checkSum((u_short*)Buffer, length - sizeof(header)))
        {
            //����յ������кų��������·���ACK����ʱseqû�б仯
            if (checkSum((u_short*)Buffer, length - sizeof(header)) == 0 && seq != int(header.SEQ))
            {
                //˵���������⣬����ACK
                //header.flag = ACK;
                //header.datasize = 0;
                //header.SEQ = (unsigned char)seq;
                //header.sum = checkSum((u_short*)&header, sizeof(header));
                //memcpy(Buffer, &header, sizeof(header));
                ////�ط��ð���ACK
                //sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen);
                //cout << "[info]�ط���һ���ظ��� - ACK:" << (int)header.SEQ << " SEQ:" << (int)header.SEQ << endl;
                continue;//���������ݰ�
            }
            seq = int(header.SEQ);
            if (seq > 255)
            {
                seq = seq - 256;
            }

            cout << "[info]�յ��� " << length - sizeof(header) << " �ֽ� - Flag:" << int(header.flag) << " SEQ : " << int(header.SEQ) << " SUM:" << int(header.sum) << endl;
            char* temp = new char[length - sizeof(header)];
            memcpy(temp, Buffer + sizeof(header), length - sizeof(header));
            //cout << "size" << sizeof(message) << endl;
            memcpy(message + fileLength, temp, length - sizeof(header));
            fileLength = fileLength + int(header.datasize);

            //����ACK
            header.flag = ACK;
            header.datasize = 0;
            header.SEQ = (unsigned char)seq;
            header.sum = 0;
            header.sum = checkSum((u_short*)&header, sizeof(header));
            memcpy(Buffer, &header, sizeof(header));
            //���ؿͻ���ACK������ɹ��յ���
            sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen);
            cout << "[info]�ظ��ͻ��� - flag:" << (int)header.flag << " SEQ:" << (int)header.SEQ << " SUM:" << int(header.sum) << endl;
            seq++;
            if (seq > 255)
            {
                seq = seq - 256;
            }
        }
    }
    //����OVER��Ϣ
    header.flag = OVER;
    header.sum = 0;
    header.sum = checkSum((u_short*)&header, sizeof(header));
    memcpy(Buffer, &header, sizeof(header));
    if (sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen) == -1)
    {
        return -1;
    }
    return fileLength;
}

int disConnect(SOCKET& sockServ, SOCKADDR_IN& ClientAddr, int& ClientAddrLen)
{
    HEADER header;
    char* Buffer = new char[sizeof(header)];
    while (1) 
    {
        int length = recvfrom(sockServ, Buffer, sizeof(header) + MAXSIZE, 0, (sockaddr*)&ClientAddr, &ClientAddrLen);//���ձ��ĳ���
        memcpy(&header, Buffer, sizeof(header));
        if (header.flag == FIN && checkSum((u_short*)&header, sizeof(header)) == 0)
        {
            cout << "[info]���յ���һ�λ������� " << endl;
            break;
        }
    }
    //���͵ڶ��λ�����Ϣ
    header.flag = ACK;
    header.sum = 0;
    header.sum = checkSum((u_short*)&header, sizeof(header));
    memcpy(Buffer, &header, sizeof(header));
    if (sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen) == -1)
    {
        cout << "[info]���͵ڶ��λ���ʧ��" << endl;
        return -1;
    }
    else
    {
        cout << "[info]�ɹ����͵ڶ��λ�������" << endl;
    }
    clock_t start = clock();//��¼�ڶ��λ��ַ���ʱ��

    //���յ����λ���
    while (recvfrom(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, &ClientAddrLen) <= 0)
    {
        //���͵ڶ��λ��ֵȴ������λ��ֹ����г�ʱ���ش��ڶ��λ���
        if (clock() - start > MAX_TIME)
        {
            cout << "[info]�ڶ��λ��ֳ�ʱ " << endl;
            header.flag = ACK;
            header.sum = 0;
            header.sum = checkSum((u_short*)&header, sizeof(header));
            memcpy(Buffer, &header, sizeof(header));
            if (sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen) == -1)
            {
                return -1;
            }
            cout << "[info]���ش��ڶ��λ������� " << endl;
        }
    }
    //�����յ��ĵ����λ���
    HEADER temp1;
    memcpy(&temp1, Buffer, sizeof(header));
    if (temp1.flag == FIN_ACK && checkSum((u_short*)&temp1, sizeof(temp1) == 0))
    {
        cout << "[info]���յ������λ������� " << endl;
    }
    else
    {
        //cout << "[info]�������ݣ�������" << endl;
        return -1;
    }

    //���͵��Ĵλ�����Ϣ
    header.flag = FIN_ACK;
    header.sum = 0;
    header.sum = checkSum((u_short*)&header, sizeof(header));
    memcpy(Buffer, &header, sizeof(header));
    if (sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen) == -1)
    {
        cout << "[info]���Ĵλ��ַ���ʧ�� " << endl;
        return -1;
    }
    else
    {
        cout << "[info]�ɹ����͵��Ĵλ������� " << endl;
    }
    cout << "[info]�Ĵλ��ֽ��������ӶϿ��� " << endl;
    return 1;
}
int main()
{
    
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);

    SOCKADDR_IN serverAddr;
    SOCKET server;

    serverAddr.sin_family = AF_INET;//ʹ��IPV4
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    server = socket(AF_INET, SOCK_DGRAM, 0);
    cout << "[info]�ɹ�����socket " << endl;
    bind(server, (SOCKADDR*)&serverAddr, sizeof(serverAddr));//���׽��֣��������״̬
    cout << "[info]�������״̬���ȴ��ͻ������� " << endl;

    int len = sizeof(serverAddr);
    //��������
    Connect(server, serverAddr, len);
    cout << "[info]�ɹ��������ӣ����ڵȴ������ļ� " << endl;

    char* name = new char[20];
    char* data = new char[100000000];
    //char name[8] = "999.txt";
    //char  data[20] = "hello!";
    //int namelen = 7;
    //int datalen = 10;
    int namelen = RecvMessage(server, serverAddr, len, name);
    int datalen = RecvMessage(server, serverAddr, len, data);
    string a;
    for (int i = 0; i < namelen; i++)
    {
        a = a + name[i];
    }
    disConnect(server, serverAddr, len);
    cout <<"name:"<< a << endl;
    ofstream fout(a.c_str(), ofstream::binary);
    cout << "datalen:" << datalen << endl;
    for (int i = 0; i < datalen; i++)
    {
        fout << data[i];
    }
    fout.close();
    cout << "�ļ��ѳɹ����ص����� " << endl;
    system("pause");
}