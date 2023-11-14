
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

u_short checkSum(u_short* mes, int size) {
    int count = (size + 1) / 2;
    //buffer相当于一个元素为u_short类型的数组，每个元素16位，相当于求校验和过程中的一个元素
    u_short* buf = (u_short*)malloc(size + 1);
    memset(buf, 0, size + 1);
    memcpy(buf, mes, size);//将message读入buf
    u_long sum = 0;
    while (count--) {
        sum += *buf++;
        //如果有进位则将进位加到最低位
        if (sum & 0xffff0000) {
            sum &= 0xffff;
            sum++;
        }
    }
    //取反
    return ~(sum & 0xffff);
}

struct HEADER
{
    u_short sum = 0;//校验和 16位
    u_short datasize = 0;//所包含数据长度 16位
    unsigned char flag = 0;
    //八位，使用后三位，排列是FIN ACK SYN 
    unsigned char SEQ = 0;
    //八位，传输的序列号，0~255，超过后mod
    HEADER() {
        sum = 0;//校验和    16位
        datasize = 0;//所包含数据长度     16位
        flag = 0;//8位，使用后四位，排列是FIN ACK SYN 
        SEQ = 0;//8位
    }
};

int Connect(SOCKET& sockServ, SOCKADDR_IN& ClientAddr, int& ClientAddrLen)
{
    HEADER header;
    char* Buffer = new char[sizeof(header)];

<<<<<<< HEAD
    //接收第一次握手信息
    while (1)
    {
        //通过绑定的socket传递、接收数据
        if (recvfrom(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, &ClientAddrLen) == -1)
        {
            return -1;
        }
        memcpy(&header, Buffer, sizeof(header));
        if (header.flag == SYN && checkSum((u_short*)&header, sizeof(header)) == 0){
            cout << "[info]接收到第一次握手数据 " << endl;
            break;
        }
    }
    //发送第二次握手信息
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
        cout << "[info]成功发送第二次握手数据 " << endl;
    }
    clock_t start = clock();//记录第二次握手发送时间
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

	// 鍙戦�佺浜屾鎻℃墜搴旂瓟鎶ユ枃
	memset(header, 0, HEADERSIZE);
	// 璁剧疆ack浣嶏紝ack = seq of message 1 + 1
	ack = seq + 1;
	header[ACK_BITS_START] = (u_char)(ack & 0xFF);
	header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
	header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
	header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
	// 璁剧疆seq浣�
	seq = rand() % 65535;
	header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
	header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
	header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
	header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
	// 璁剧疆ACK SYN浣�
	header[FLAG_BIT_POSITION] = 0b110;
	sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));
	cout << "Send the Second Handshake message!" << endl;
>>>>>>> 3e47a6b2561d05b8b079c9439f659d5a50a4cc84

    //接收第三次握手
    while (recvfrom(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, &ClientAddrLen) <= 0)
    {
        //超时重传
        if (clock() - start > MAX_TIME)
        {
            cout << "[info]第二次握手超时 " << endl;
            header.flag = ACK;
            header.sum = 0;
            header.flag = checkSum((u_short*)&header, sizeof(header));
            memcpy(Buffer, &header, sizeof(header));
            if (sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen) == -1)
            {
                return -1;
            }
            cout << "[info]已经重传 " << endl;
        }
    }

<<<<<<< HEAD
    //解析收到的第三次握手的数据包
    HEADER temp1;
    memcpy(&temp1, Buffer, sizeof(header));
    
    if (temp1.flag == ACK_SYN && checkSum((u_short*)&temp1, sizeof(temp1)) == 0)
    {
        cout << "[info]接收到第三次握手数据" << endl;
        cout << "[info]成功连接" << endl;
    }
    else
    {
        //cout << "[info]错误数据，请重试" << endl;
    }
    return 1;
}
int RecvMessage(SOCKET& sockServ, SOCKADDR_IN& ClientAddr, int& ClientAddrLen, char* message)
{
    long int fileLength = 0;//文件长度
    HEADER header;
    char* Buffer = new char[MAXSIZE + sizeof(header)];
    int seq = 0;
    int index = 0;

    while (1)
    {
        int length = recvfrom(sockServ, Buffer, sizeof(header) + MAXSIZE, 0, (sockaddr*)&ClientAddr, &ClientAddrLen);//接收报文长度
        //cout << length << endl;
        memcpy(&header, Buffer, sizeof(header));
        //判断是否是结束
        if (header.flag == OVER && checkSum((u_short*)&header, sizeof(header)) == 0)
        {
            cout << "[info]文件传输结束" << endl;
            break;
        }
        if (header.flag == unsigned char(0) && checkSum((u_short*)Buffer, length - sizeof(header)))
        {
            //如果收到的序列号出错，则重新返回ACK，此时seq没有变化
            if (checkSum((u_short*)Buffer, length - sizeof(header)) == 0 && seq != int(header.SEQ))
            {
                //说明出了问题，返回ACK
                //header.flag = ACK;
                //header.datasize = 0;
                //header.SEQ = (unsigned char)seq;
                //header.sum = checkSum((u_short*)&header, sizeof(header));
                //memcpy(Buffer, &header, sizeof(header));
                ////重发该包的ACK
                //sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen);
                //cout << "[info]重发上一个回复包 - ACK:" << (int)header.SEQ << " SEQ:" << (int)header.SEQ << endl;
                continue;//丢弃该数据包
            }
            seq = int(header.SEQ);
            if (seq > 255)
            {
                seq = seq - 256;
            }

            cout << "[info]收到了 " << length - sizeof(header) << " 字节 - Flag:" << int(header.flag) << " SEQ : " << int(header.SEQ) << " SUM:" << int(header.sum) << endl;
            char* temp = new char[length - sizeof(header)];
            memcpy(temp, Buffer + sizeof(header), length - sizeof(header));
            //cout << "size" << sizeof(message) << endl;
            memcpy(message + fileLength, temp, length - sizeof(header));
            fileLength = fileLength + int(header.datasize);

            //返回ACK
            header.flag = ACK;
            header.datasize = 0;
            header.SEQ = (unsigned char)seq;
            header.sum = 0;
            header.sum = checkSum((u_short*)&header, sizeof(header));
            memcpy(Buffer, &header, sizeof(header));
            //返回客户端ACK，代表成功收到了
            sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen);
            cout << "[info]回复客户端 - flag:" << (int)header.flag << " SEQ:" << (int)header.SEQ << " SUM:" << int(header.sum) << endl;
            seq++;
            if (seq > 255)
            {
                seq = seq - 256;
            }
        }
    }
    //发送OVER信息
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
        int length = recvfrom(sockServ, Buffer, sizeof(header) + MAXSIZE, 0, (sockaddr*)&ClientAddr, &ClientAddrLen);//接收报文长度
        memcpy(&header, Buffer, sizeof(header));
        if (header.flag == FIN && checkSum((u_short*)&header, sizeof(header)) == 0)
        {
            cout << "[info]接收到第一次挥手数据 " << endl;
            break;
        }
    }
    //发送第二次挥手信息
    header.flag = ACK;
    header.sum = 0;
    header.sum = checkSum((u_short*)&header, sizeof(header));
    memcpy(Buffer, &header, sizeof(header));
    if (sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen) == -1)
    {
        cout << "[info]发送第二次挥手失败" << endl;
        return -1;
    }
    else
    {
        cout << "[info]成功发送第二次挥手数据" << endl;
    }
    clock_t start = clock();//记录第二次挥手发送时间

    //接收第三次挥手
    while (recvfrom(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, &ClientAddrLen) <= 0)
    {
        //发送第二次挥手等待第三次挥手过程中超时，重传第二次挥手
        if (clock() - start > MAX_TIME)
        {
            cout << "[info]第二次挥手超时 " << endl;
            header.flag = ACK;
            header.sum = 0;
            header.sum = checkSum((u_short*)&header, sizeof(header));
            memcpy(Buffer, &header, sizeof(header));
            if (sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen) == -1)
            {
                return -1;
            }
            cout << "[info]已重传第二次挥手数据 " << endl;
        }
    }
    //解析收到的第三次挥手
    HEADER temp1;
    memcpy(&temp1, Buffer, sizeof(header));
    if (temp1.flag == FIN_ACK && checkSum((u_short*)&temp1, sizeof(temp1) == 0))
    {
        cout << "[info]接收到第三次挥手数据 " << endl;
    }
    else
    {
        //cout << "[info]错误数据，请重试" << endl;
        return -1;
    }

<<<<<<< HEAD
    //发送第四次挥手信息
    header.flag = FIN_ACK;
    header.sum = 0;
    header.sum = checkSum((u_short*)&header, sizeof(header));
    memcpy(Buffer, &header, sizeof(header));
    if (sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen) == -1)
    {
        cout << "[info]第四次挥手发送失败 " << endl;
        return -1;
    }
    else
    {
        cout << "[info]成功发送第四次挥手数据 " << endl;
    }
    cout << "[info]四次挥手结束，连接断开！ " << endl;
    return 1;
=======

	// 鍙戦�佺浜屾鎸ユ墜搴旂瓟鎶ユ枃
	// 璁剧疆ack浣�
	ack = (u_char)header[SEQ_BITS_START] + ((u_char)header[SEQ_BITS_START + 1] << 8)
		+ ((u_char)header[SEQ_BITS_START + 2] << 16) + ((u_char)header[SEQ_BITS_START + 3] << 24) + 1;
	header[ACK_BITS_START] = (u_char)(ack & 0xFF);
	header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
	header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
	header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
	// 璁剧疆seq浣�
	seq = rand();
	header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
	header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
	header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
	header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
	// 璁剧疆ACK浣�
	header[FLAG_BIT_POSITION] = 0b100;
	sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));
	cout << "Send the Second Wavehand message!" << endl;

	// 鍙戦�佺涓夋鎸ユ墜璇锋眰鎶ユ枃
	// 璁剧疆seq浣�
	seq = rand();
	header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
	header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
	header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
	header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
	// ack鍜屼笂涓�涓姤鏂囦竴鏍�
	// 璁剧疆ACK FIN浣�
	header[FLAG_BIT_POSITION] = 0b101;
	sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));
	cout << "Send the Third Wavehand message!" << endl;

	// 鎺ユ敹绗洓娆℃尌鎵嬪簲绛旀姤鏂�
	while (true) {
		recvResult = recvfrom(recvSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, &len);
		// 妫�鏌hecksum
		checksum = checkSum(recvBuf, HEADERSIZE);
		// 鎻愬彇ack of message 4
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

    serverAddr.sin_family = AF_INET;//使用IPV4
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    server = socket(AF_INET, SOCK_DGRAM, 0);
    cout << "[info]成功建立socket " << endl;
    bind(server, (SOCKADDR*)&serverAddr, sizeof(serverAddr));//绑定套接字，进入监听状态
    cout << "[info]进入监听状态，等待客户端上线 " << endl;

    int len = sizeof(serverAddr);
    //建立连接
    Connect(server, serverAddr, len);
    cout << "[info]成功建立连接，正在等待接收文件 " << endl;

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
    cout << "文件已成功下载到本地 " << endl;
    system("pause");
=======
		// 妫�鏌ユ槸鍚︽槸鎸ユ墜鎶ユ枃
		if (recvBuf[FLAG_BIT_POSITION] == 0b101) {
			// 璁板綍涓�涓媠eq
			for (int i = 0; i < 4; i++) {
				header[SEQ_BITS_START + i] = recvBuf[SEQ_BITS_START + i];
			}
			cout << "Successfully received the Fisrt Wavehand message!" << endl;
			wavehand();
			return;
		}

		// 妫�鏌ユ槸鍚︽槸鏂囦欢鍚�
		if (recvBuf[FLAG_BIT_POSITION] != 0b1000) {
			continue; // 鍥犱负鍙兘浼氭敹鍒板湪send绔粨鏉熷彂閫佸悗鐨勮繕鍦ㄨ矾涓婄殑鎶ユ枃锛岃繃婊ゆ帀瀹冧滑
		}

		// 鎻愬彇header
		memcpy(header, recvBuf, HEADERSIZE);

		// 鎻愬彇鏂囦欢鍚�
		if (header[FLAG_BIT_POSITION] == 0b1000) {
			memcpy(filename, recvBuf + HEADERSIZE, FILE_NAME_MAX_LENGTH);
		}

		// 鎺ユ敹鏂囦欢澶у皬
		recvResult = recvfrom(recvSocket, recvBuf, PACKETSIZE, 0, (SOCKADDR*)&sendAddr, &len);

		// 鎻愬彇header
		memcpy(header, recvBuf, HEADERSIZE);

		// 鎻愬彇鏂囦欢澶у皬
		if (header[FLAG_BIT_POSITION] == 0b10000) {
			filesize = atoi(recvBuf + HEADERSIZE);
		}
		cout << "[Begin]: Begin to receive a file, filename: " << filename << ", filesize: " << filesize << " bytes." << endl;

		// 鎺ユ敹鏂囦欢鍐呭
		int hasReceived = 0; // 宸叉帴鏀跺瓧鑺傛暟
		int seq_opp = 0, ack_opp = 0; // 瀵规柟鍙戦�佹姤鏂囦腑鐨剆eq, ack
		int seq = 0, ack = 0; // 瑕佸彂閫佺殑鍝嶅簲鎶ユ枃涓殑seq, ack
		int expectedSeq = ack + 1; // 鏈熷緟鏀跺埌鐨刾acket seq
		int dataLength = 0; // 鎺ユ敹鍒扮殑鏁版嵁娈甸暱搴�(= recvResult - HEADERSIZE)
		u_short checksum = 0; // 鏍￠獙鍜岋紙涓�0鏃舵纭級

		ofstream out;
		out.open(filename, ios::out | ios::binary | ios::app);
		while (true) {
			expectedSeq = ack + 1; // receive绔敮涓�涓�涓帴鏀剁獥鍙�

			memset(recvBuf, 0, PACKETSIZE);
			// memset(header, 0, HEADERSIZE);
			recvResult = recvfrom(recvSocket, recvBuf, PACKETSIZE, 0, (SOCKADDR*)&sendAddr, &len);
			if (recvResult == SOCKET_ERROR) {
				cout << "[Error]: Receive error! sleep!" << endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(2000));
				continue;
			}

			// 妫�鏌ユ牎楠屽拰 and ACK浣�
			checksum = checkSum(recvBuf, recvResult);
			if (checksum == 0 && recvBuf[FLAG_BIT_POSITION] == 0b100) {
				seq_opp = (u_char)recvBuf[SEQ_BITS_START] + ((u_char)recvBuf[SEQ_BITS_START + 1] << 8)
					+ ((u_char)recvBuf[SEQ_BITS_START + 2] << 16) + ((u_char)recvBuf[SEQ_BITS_START + 3] << 24);
				ack_opp = (u_char)recvBuf[ACK_BITS_START] + ((u_char)recvBuf[ACK_BITS_START + 1] << 8)
					+ ((u_char)recvBuf[ACK_BITS_START + 2] << 16) + ((u_char)recvBuf[ACK_BITS_START + 3] << 24);
				if (seq_opp == expectedSeq) { // 妫�鏌ユ敹鍒扮殑鍖呯殑seq(鍗硈eq_opp)鏄惁鏄湡寰呯殑seq
					// 濡傛灉鏀跺埌浜嗘纭殑鍖咃紝閭ｅ氨鎻愬彇鍐呭 + 鍥炲
					dataLength = recvResult - HEADERSIZE;
					// 鎻愬彇鏁版嵁
					memcpy(dataSegment, recvBuf + HEADERSIZE, dataLength);
					out.write(dataSegment, dataLength);

					// 璁剧疆seq浣嶏紝鏈崗璁腑涓轰簡纭鏂逛究锛屽氨鎶婂搷搴旀姤鏂囩殑seq缃负鏀跺埌鎶ユ枃鐨剆eq
					seq = seq_opp;
					header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
					header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
					header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
					header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
					// 璁剧疆ack浣�, = seq_opp锛岃〃绀虹‘璁ゆ帴鏀跺埌浜嗚繖涔嬪墠鐨勫叏閮ㄥ唴瀹癸紝骞舵湡寰呮敹鍒拌繖涔嬪悗鐨勫唴瀹�
					ack = seq_opp;
					header[ACK_BITS_START] = (u_char)(ack & 0xFF);
					header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
					header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
					header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
					// 璁剧疆ACK浣�
					header[FLAG_BIT_POSITION] = 0b100;
					// 鍝嶅簲鎶ユ枃涓殑data length涓�0锛屽氨涓嶇敤璁剧疆浜�

					hasReceived += recvResult - HEADERSIZE;
					cout << "[Recv]: Received " << hasReceived << " bytes, ack = " << ack << ", seq = " << seq << endl;

					// 妯℃嫙寤舵椂
					if (dis(gen) < DELAY_RATE)
						std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_TIME));

					// 妯℃嫙涓㈠寘
					if (dis(gen) > DISCARD_RATE)
						sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));
				}
				else {
					// 璇存槑缃戠粶寮傚父锛屼涪浜嗗寘鎴栬�呮湁寤惰繜锛屾墍浠ヤ笉鐢ㄦ洿鏀癸紝鐩存帴閲嶅彂鏀跺埌鐨勬渶鏂板寘鐨刟ck鍗冲彲

					// 妯℃嫙寤舵椂
					if (dis(gen) < DELAY_RATE)
						std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_TIME));

					// 妯℃嫙涓㈠寘
					if (dis(gen) > DISCARD_RATE)
						sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));

					cout << "[Error]: Expected seq = " << expectedSeq << ". Received seq = " << seq_opp << endl;
				}
			}
			else {
				// 鏍￠獙鍜屾垨ACK浣嶅紓甯革紝閲嶅彂涓婁竴涓寘鐨刟ck
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

	recvAddr.sin_family = AF_INET; // 鍗忚鐗堟湰
	recvAddr.sin_addr.S_un.S_addr = inet_addr(IP); // ip鍦板潃锛宨net_addr鎶婃暟鐐规牸寮忚浆鎹负鏁存暟
	recvAddr.sin_port = htons(PORT); // 绔彛鍙凤紝0-65535
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