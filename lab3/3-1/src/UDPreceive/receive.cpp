
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

<<<<<<< HEAD
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
=======
		if (checksum == 0 && recvBuf[FLAG_BIT_POSITION] == 0b010) {
			cout << "Successfully received the First Handshake message!" << endl;
			break;
		}
		else {
			cout << "FAailed to received the correct First Handshake message, Handshake failed!" << endl;
			return false;
		}
	}

	// 发送第二次握手应答报文
	memset(header, 0, HEADERSIZE);
	// 设置ack位，ack = seq of message 1 + 1
	ack = seq + 1;
	header[ACK_BITS_START] = (u_char)(ack & 0xFF);
	header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
	header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
	header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
	// 设置seq位
	seq = rand() % 65535;
	header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
	header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
	header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
	header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
	// 设置ACK SYN位
	header[FLAG_BIT_POSITION] = 0b110;
	sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));
	cout << "Send the Second Handshake message!" << endl;
>>>>>>> 3e47a6b2561d05b8b079c9439f659d5a50a4cc84

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

<<<<<<< HEAD
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
=======
		if (checksum == 0 && ack == seq + 1 && recvBuf[FLAG_BIT_POSITION] == 0b100) {
			cout << "Successfully received the Third Handshake message!" << endl;
			break;
		}
		else {
			cout << "Failed to received the correct Third Handshake message, Handshake failed!" << endl;
			return false;
		}
	}
	cout << "Handshake successfully!" << endl;
	return true;
>>>>>>> 3e47a6b2561d05b8b079c9439f659d5a50a4cc84
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

<<<<<<< HEAD
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
=======

	// 发送第二次挥手应答报文
	// 设置ack位
	ack = (u_char)header[SEQ_BITS_START] + ((u_char)header[SEQ_BITS_START + 1] << 8)
		+ ((u_char)header[SEQ_BITS_START + 2] << 16) + ((u_char)header[SEQ_BITS_START + 3] << 24) + 1;
	header[ACK_BITS_START] = (u_char)(ack & 0xFF);
	header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
	header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
	header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
	// 设置seq位
	seq = rand();
	header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
	header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
	header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
	header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
	// 设置ACK位
	header[FLAG_BIT_POSITION] = 0b100;
	sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));
	cout << "Send the Second Wavehand message!" << endl;

	// 发送第三次挥手请求报文
	// 设置seq位
	seq = rand();
	header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
	header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
	header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
	header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
	// ack和上一个报文一样
	// 设置ACK FIN位
	header[FLAG_BIT_POSITION] = 0b101;
	sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));
	cout << "Send the Third Wavehand message!" << endl;

	// 接收第四次挥手应答报文
	while (true) {
		recvResult = recvfrom(recvSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, &len);
		// 检查checksum
		checksum = checkSum(recvBuf, HEADERSIZE);
		// 提取ack of message 4
		ack = (u_char)recvBuf[ACK_BITS_START] + ((u_char)recvBuf[ACK_BITS_START + 1] << 8)
			+ ((u_char)recvBuf[ACK_BITS_START + 2] << 16) + ((u_char)recvBuf[ACK_BITS_START + 3] << 24);
		if (checksum == 0 && recvBuf[FLAG_BIT_POSITION] == 0b100) {
			cout << "Successfully received the Forth Wavehand message!" << endl;
			break;
		}
		else {
			cout << "Failed to received the correct Forth Wavehand message, Handshake failed!" << endl;
			return;
		}
	}

	cout << "Wavehand successfully!" << endl;
	return;
>>>>>>> 3e47a6b2561d05b8b079c9439f659d5a50a4cc84
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

<<<<<<< HEAD
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
=======
		// 检查是否是挥手报文
		if (recvBuf[FLAG_BIT_POSITION] == 0b101) {
			// 记录一下seq
			for (int i = 0; i < 4; i++) {
				header[SEQ_BITS_START + i] = recvBuf[SEQ_BITS_START + i];
			}
			cout << "Successfully received the Fisrt Wavehand message!" << endl;
			wavehand();
			return;
		}

		// 检查是否是文件名
		if (recvBuf[FLAG_BIT_POSITION] != 0b1000) {
			continue; // 因为可能会收到在send端结束发送后的还在路上的报文，过滤掉它们
		}

		// 提取header
		memcpy(header, recvBuf, HEADERSIZE);

		// 提取文件名
		if (header[FLAG_BIT_POSITION] == 0b1000) {
			memcpy(filename, recvBuf + HEADERSIZE, FILE_NAME_MAX_LENGTH);
		}

		// 接收文件大小
		recvResult = recvfrom(recvSocket, recvBuf, PACKETSIZE, 0, (SOCKADDR*)&sendAddr, &len);

		// 提取header
		memcpy(header, recvBuf, HEADERSIZE);

		// 提取文件大小
		if (header[FLAG_BIT_POSITION] == 0b10000) {
			filesize = atoi(recvBuf + HEADERSIZE);
		}
		cout << "[Begin]: Begin to receive a file, filename: " << filename << ", filesize: " << filesize << " bytes." << endl;

		// 接收文件内容
		int hasReceived = 0; // 已接收字节数
		int seq_opp = 0, ack_opp = 0; // 对方发送报文中的seq, ack
		int seq = 0, ack = 0; // 要发送的响应报文中的seq, ack
		int expectedSeq = ack + 1; // 期待收到的packet seq
		int dataLength = 0; // 接收到的数据段长度(= recvResult - HEADERSIZE)
		u_short checksum = 0; // 校验和（为0时正确）

		ofstream out;
		out.open(filename, ios::out | ios::binary | ios::app);
		while (true) {
			expectedSeq = ack + 1; // receive端唯一一个接收窗口

			memset(recvBuf, 0, PACKETSIZE);
			// memset(header, 0, HEADERSIZE);
			recvResult = recvfrom(recvSocket, recvBuf, PACKETSIZE, 0, (SOCKADDR*)&sendAddr, &len);
			if (recvResult == SOCKET_ERROR) {
				cout << "[Error]: Receive error! sleep!" << endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(2000));
				continue;
			}

			// 检查校验和 and ACK位
			checksum = checkSum(recvBuf, recvResult);
			if (checksum == 0 && recvBuf[FLAG_BIT_POSITION] == 0b100) {
				seq_opp = (u_char)recvBuf[SEQ_BITS_START] + ((u_char)recvBuf[SEQ_BITS_START + 1] << 8)
					+ ((u_char)recvBuf[SEQ_BITS_START + 2] << 16) + ((u_char)recvBuf[SEQ_BITS_START + 3] << 24);
				ack_opp = (u_char)recvBuf[ACK_BITS_START] + ((u_char)recvBuf[ACK_BITS_START + 1] << 8)
					+ ((u_char)recvBuf[ACK_BITS_START + 2] << 16) + ((u_char)recvBuf[ACK_BITS_START + 3] << 24);
				if (seq_opp == expectedSeq) { // 检查收到的包的seq(即seq_opp)是否是期待的seq
					// 如果收到了正确的包，那就提取内容 + 回复
					dataLength = recvResult - HEADERSIZE;
					// 提取数据
					memcpy(dataSegment, recvBuf + HEADERSIZE, dataLength);
					out.write(dataSegment, dataLength);

					// 设置seq位，本协议中为了确认方便，就把响应报文的seq置为收到报文的seq
					seq = seq_opp;
					header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
					header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
					header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
					header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
					// 设置ack位, = seq_opp，表示确认接收到了这之前的全部内容，并期待收到这之后的内容
					ack = seq_opp;
					header[ACK_BITS_START] = (u_char)(ack & 0xFF);
					header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
					header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
					header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
					// 设置ACK位
					header[FLAG_BIT_POSITION] = 0b100;
					// 响应报文中的data length为0，就不用设置了

					hasReceived += recvResult - HEADERSIZE;
					cout << "[Recv]: Received " << hasReceived << " bytes, ack = " << ack << ", seq = " << seq << endl;

					// 模拟延时
					if (dis(gen) < DELAY_RATE)
						std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_TIME));

					// 模拟丢包
					if (dis(gen) > DISCARD_RATE)
						sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));
				}
				else {
					// 说明网络异常，丢了包或者有延迟，所以不用更改，直接重发收到的最新包的ack即可

					// 模拟延时
					if (dis(gen) < DELAY_RATE)
						std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_TIME));

					// 模拟丢包
					if (dis(gen) > DISCARD_RATE)
						sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));

					cout << "[Error]: Expected seq = " << expectedSeq << ". Received seq = " << seq_opp << endl;
				}
			}
			else {
				// 校验和或ACK位异常，重发上一个包的ack
				cout << "[Error]: Checksum ERROR or ACK ERROR!" << endl;
				continue;
			}

			if (hasReceived == filesize) {
				cout << "[Fin]: Receive file " << filename << " successfully! total " << hasReceived << " bytes." << endl;
				out.close();
				cout << "Ready to receive the next file!" << endl;
				break;
			}
		}
	}
}

int main() {
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		cout << "WSAStartup error = " << WSAGetLastError() << endl;
		exit(1);
	}
	else {
		cout << "Start success!" << endl;
	}

	recvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (recvSocket == SOCKET_ERROR) {
		cout << "Socket error = " << WSAGetLastError() << endl;
		closesocket(recvSocket);
		WSACleanup();
		exit(1);
	}
	else {
		cout << "Socket success!" << endl;
	}

	recvAddr.sin_family = AF_INET; // 协议版本
	recvAddr.sin_addr.S_un.S_addr = inet_addr(IP); // ip地址，inet_addr把数点格式转换为整数
	recvAddr.sin_port = htons(PORT); // 端口号，0-65535
	sendAddr.sin_family = AF_INET;
	sendAddr.sin_port = htons(PORT);
	sendAddr.sin_addr.s_addr = inet_addr(IP);

	if (bind(recvSocket, (SOCKADDR*)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR) {
		cout << "Bind error = " << WSAGetLastError() << endl;
		closesocket(recvSocket);
		WSACleanup();
		exit(1);
	}
	else {
		cout << "Bind success!" << endl;
	}

	if (handshake()) {
		thread recvfile_thread(recvfile);
		recvfile_thread.join();
	}

	closesocket(recvSocket);
	WSACleanup();
	return 0;
>>>>>>> 3e47a6b2561d05b8b079c9439f659d5a50a4cc84
}