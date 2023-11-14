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
            continue;
        }
    }
    u_long mode = 0;
    ioctlsocket(socketClient, FIONBIO, &mode);//�Ļ�����ģʽ
}

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
            break;
        }
        else
        {
            continue;
        }
    }
    u_long mode = 0;
    ioctlsocket(socketClient, FIONBIO, &mode);//�Ļ�����ģʽ
}


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
    }
    fileIN.close();


    //�����ļ���
    send(server, severAddr, len, (char*)(inputFile.c_str()), inputFile.length());
    clock_t start1 = clock();
    //�����ļ����ݣ���buffer�
    send(server, severAddr, len, buffer, i);
    clock_t end1 = clock();


    cout << "[out] ������ʱ��Ϊ:" << (end1 - start1) / CLOCKS_PER_SEC << "s" << endl;
    cout << "[out] ������Ϊ:" << (((double)i) / ((end1 - start1) / CLOCKS_PER_SEC)) << "byte/s" << endl;
    disConnect(server, severAddr, len);

    system("pause");

}
