
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

u_short checkSum(u_short* mes, int size) {
    int count = (size + 1) / 2;
    //bufferÏàµ±ÓÚÒ»¸öÔªËØÎªu_shortÀàĞÍµÄÊı×é£¬Ã¿¸öÔªËØ16Î»£¬Ïàµ±ÓÚÇóĞ£ÑéºÍ¹ı³ÌÖĞµÄÒ»¸öÔªËØ
    u_short* buf = (u_short*)malloc(size + 1);
    memset(buf, 0, size + 1);
    memcpy(buf, mes, size);//½«message¶ÁÈëbuf
    u_long sum = 0;
    while (count--) {
        sum += *buf++;
        //Èç¹ûÓĞ½øÎ»Ôò½«½øÎ»¼Óµ½×îµÍÎ»
        if (sum & 0xffff0000) {
            sum &= 0xffff;
            sum++;
        }
    }
    //È¡·´
    return ~(sum & 0xffff);
}

struct HEADER
{
    u_short sum = 0;//Ğ£ÑéºÍ 16Î»
    u_short datasize = 0;//Ëù°üº¬Êı¾İ³¤¶È 16Î»
    unsigned char flag = 0;
    //°ËÎ»£¬Ê¹ÓÃºóÈıÎ»£¬ÅÅÁĞÊÇFIN ACK SYN 
    unsigned char SEQ = 0;
    //°ËÎ»£¬´«ÊäµÄĞòÁĞºÅ£¬0~255£¬³¬¹ıºómod
    HEADER() {
        sum = 0;//Ğ£ÑéºÍ    16Î»
        datasize = 0;//Ëù°üº¬Êı¾İ³¤¶È     16Î»
        flag = 0;//8Î»£¬Ê¹ÓÃºóËÄÎ»£¬ÅÅÁĞÊÇFIN ACK SYN 
        SEQ = 0;//8Î»
    }
};

int Connect(SOCKET& sockServ, SOCKADDR_IN& ClientAddr, int& ClientAddrLen)
{
    HEADER header;
    char* Buffer = new char[sizeof(header)];

<<<<<<< HEAD
    //½ÓÊÕµÚÒ»´ÎÎÕÊÖĞÅÏ¢
    while (1)
    {
        //Í¨¹ı°ó¶¨µÄsocket´«µİ¡¢½ÓÊÕÊı¾İ
        if (recvfrom(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, &ClientAddrLen) == -1)
        {
            return -1;
        }
        memcpy(&header, Buffer, sizeof(header));
        if (header.flag == SYN && checkSum((u_short*)&header, sizeof(header)) == 0){
            cout << "[info]½ÓÊÕµ½µÚÒ»´ÎÎÕÊÖÊı¾İ " << endl;
            break;
        }
    }
    //·¢ËÍµÚ¶ş´ÎÎÕÊÖĞÅÏ¢
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
        cout << "[info]³É¹¦·¢ËÍµÚ¶ş´ÎÎÕÊÖÊı¾İ " << endl;
    }
    clock_t start = clock();//¼ÇÂ¼µÚ¶ş´ÎÎÕÊÖ·¢ËÍÊ±¼ä
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

	// å‘é€ç¬¬äºŒæ¬¡æ¡æ‰‹åº”ç­”æŠ¥æ–‡
	memset(header, 0, HEADERSIZE);
	// è®¾ç½®ackä½ï¼Œack = seq of message 1 + 1
	ack = seq + 1;
	header[ACK_BITS_START] = (u_char)(ack & 0xFF);
	header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
	header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
	header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
	// è®¾ç½®seqä½
	seq = rand() % 65535;
	header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
	header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
	header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
	header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
	// è®¾ç½®ACK SYNä½
	header[FLAG_BIT_POSITION] = 0b110;
	sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));
	cout << "Send the Second Handshake message!" << endl;
>>>>>>> 3e47a6b2561d05b8b079c9439f659d5a50a4cc84

    //½ÓÊÕµÚÈı´ÎÎÕÊÖ
    while (recvfrom(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, &ClientAddrLen) <= 0)
    {
        //³¬Ê±ÖØ´«
        if (clock() - start > MAX_TIME)
        {
            cout << "[info]µÚ¶ş´ÎÎÕÊÖ³¬Ê± " << endl;
            header.flag = ACK;
            header.sum = 0;
            header.flag = checkSum((u_short*)&header, sizeof(header));
            memcpy(Buffer, &header, sizeof(header));
            if (sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen) == -1)
            {
                return -1;
            }
            cout << "[info]ÒÑ¾­ÖØ´« " << endl;
        }
    }

<<<<<<< HEAD
    //½âÎöÊÕµ½µÄµÚÈı´ÎÎÕÊÖµÄÊı¾İ°ü
    HEADER temp1;
    memcpy(&temp1, Buffer, sizeof(header));
    
    if (temp1.flag == ACK_SYN && checkSum((u_short*)&temp1, sizeof(temp1)) == 0)
    {
        cout << "[info]½ÓÊÕµ½µÚÈı´ÎÎÕÊÖÊı¾İ" << endl;
        cout << "[info]³É¹¦Á¬½Ó" << endl;
    }
    else
    {
        //cout << "[info]´íÎóÊı¾İ£¬ÇëÖØÊÔ" << endl;
    }
    return 1;
}
int RecvMessage(SOCKET& sockServ, SOCKADDR_IN& ClientAddr, int& ClientAddrLen, char* message)
{
    long int fileLength = 0;//ÎÄ¼ş³¤¶È
    HEADER header;
    char* Buffer = new char[MAXSIZE + sizeof(header)];
    int seq = 0;
    int index = 0;

    while (1)
    {
        int length = recvfrom(sockServ, Buffer, sizeof(header) + MAXSIZE, 0, (sockaddr*)&ClientAddr, &ClientAddrLen);//½ÓÊÕ±¨ÎÄ³¤¶È
        //cout << length << endl;
        memcpy(&header, Buffer, sizeof(header));
        //ÅĞ¶ÏÊÇ·ñÊÇ½áÊø
        if (header.flag == OVER && checkSum((u_short*)&header, sizeof(header)) == 0)
        {
            cout << "[info]ÎÄ¼ş´«Êä½áÊø" << endl;
            break;
        }
        if (header.flag == unsigned char(0) && checkSum((u_short*)Buffer, length - sizeof(header)))
        {
            //Èç¹ûÊÕµ½µÄĞòÁĞºÅ³ö´í£¬ÔòÖØĞÂ·µ»ØACK£¬´ËÊ±seqÃ»ÓĞ±ä»¯
            if (checkSum((u_short*)Buffer, length - sizeof(header)) == 0 && seq != int(header.SEQ))
            {
                //ËµÃ÷³öÁËÎÊÌâ£¬·µ»ØACK
                //header.flag = ACK;
                //header.datasize = 0;
                //header.SEQ = (unsigned char)seq;
                //header.sum = checkSum((u_short*)&header, sizeof(header));
                //memcpy(Buffer, &header, sizeof(header));
                ////ÖØ·¢¸Ã°üµÄACK
                //sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen);
                //cout << "[info]ÖØ·¢ÉÏÒ»¸ö»Ø¸´°ü - ACK:" << (int)header.SEQ << " SEQ:" << (int)header.SEQ << endl;
                continue;//¶ªÆú¸ÃÊı¾İ°ü
            }
            seq = int(header.SEQ);
            if (seq > 255)
            {
                seq = seq - 256;
            }

            cout << "[info]ÊÕµ½ÁË " << length - sizeof(header) << " ×Ö½Ú - Flag:" << int(header.flag) << " SEQ : " << int(header.SEQ) << " SUM:" << int(header.sum) << endl;
            char* temp = new char[length - sizeof(header)];
            memcpy(temp, Buffer + sizeof(header), length - sizeof(header));
            //cout << "size" << sizeof(message) << endl;
            memcpy(message + fileLength, temp, length - sizeof(header));
            fileLength = fileLength + int(header.datasize);

            //·µ»ØACK
            header.flag = ACK;
            header.datasize = 0;
            header.SEQ = (unsigned char)seq;
            header.sum = 0;
            header.sum = checkSum((u_short*)&header, sizeof(header));
            memcpy(Buffer, &header, sizeof(header));
            //·µ»Ø¿Í»§¶ËACK£¬´ú±í³É¹¦ÊÕµ½ÁË
            sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen);
            cout << "[info]»Ø¸´¿Í»§¶Ë - flag:" << (int)header.flag << " SEQ:" << (int)header.SEQ << " SUM:" << int(header.sum) << endl;
            seq++;
            if (seq > 255)
            {
                seq = seq - 256;
            }
        }
    }
    //·¢ËÍOVERĞÅÏ¢
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
        int length = recvfrom(sockServ, Buffer, sizeof(header) + MAXSIZE, 0, (sockaddr*)&ClientAddr, &ClientAddrLen);//½ÓÊÕ±¨ÎÄ³¤¶È
        memcpy(&header, Buffer, sizeof(header));
        if (header.flag == FIN && checkSum((u_short*)&header, sizeof(header)) == 0)
        {
            cout << "[info]½ÓÊÕµ½µÚÒ»´Î»ÓÊÖÊı¾İ " << endl;
            break;
        }
    }
    //·¢ËÍµÚ¶ş´Î»ÓÊÖĞÅÏ¢
    header.flag = ACK;
    header.sum = 0;
    header.sum = checkSum((u_short*)&header, sizeof(header));
    memcpy(Buffer, &header, sizeof(header));
    if (sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen) == -1)
    {
        cout << "[info]·¢ËÍµÚ¶ş´Î»ÓÊÖÊ§°Ü" << endl;
        return -1;
    }
    else
    {
        cout << "[info]³É¹¦·¢ËÍµÚ¶ş´Î»ÓÊÖÊı¾İ" << endl;
    }
    clock_t start = clock();//¼ÇÂ¼µÚ¶ş´Î»ÓÊÖ·¢ËÍÊ±¼ä

    //½ÓÊÕµÚÈı´Î»ÓÊÖ
    while (recvfrom(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, &ClientAddrLen) <= 0)
    {
        //·¢ËÍµÚ¶ş´Î»ÓÊÖµÈ´ıµÚÈı´Î»ÓÊÖ¹ı³ÌÖĞ³¬Ê±£¬ÖØ´«µÚ¶ş´Î»ÓÊÖ
        if (clock() - start > MAX_TIME)
        {
            cout << "[info]µÚ¶ş´Î»ÓÊÖ³¬Ê± " << endl;
            header.flag = ACK;
            header.sum = 0;
            header.sum = checkSum((u_short*)&header, sizeof(header));
            memcpy(Buffer, &header, sizeof(header));
            if (sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen) == -1)
            {
                return -1;
            }
            cout << "[info]ÒÑÖØ´«µÚ¶ş´Î»ÓÊÖÊı¾İ " << endl;
        }
    }
    //½âÎöÊÕµ½µÄµÚÈı´Î»ÓÊÖ
    HEADER temp1;
    memcpy(&temp1, Buffer, sizeof(header));
    if (temp1.flag == FIN_ACK && checkSum((u_short*)&temp1, sizeof(temp1) == 0))
    {
        cout << "[info]½ÓÊÕµ½µÚÈı´Î»ÓÊÖÊı¾İ " << endl;
    }
    else
    {
        //cout << "[info]´íÎóÊı¾İ£¬ÇëÖØÊÔ" << endl;
        return -1;
    }

<<<<<<< HEAD
    //·¢ËÍµÚËÄ´Î»ÓÊÖĞÅÏ¢
    header.flag = FIN_ACK;
    header.sum = 0;
    header.sum = checkSum((u_short*)&header, sizeof(header));
    memcpy(Buffer, &header, sizeof(header));
    if (sendto(sockServ, Buffer, sizeof(header), 0, (sockaddr*)&ClientAddr, ClientAddrLen) == -1)
    {
        cout << "[info]µÚËÄ´Î»ÓÊÖ·¢ËÍÊ§°Ü " << endl;
        return -1;
    }
    else
    {
        cout << "[info]³É¹¦·¢ËÍµÚËÄ´Î»ÓÊÖÊı¾İ " << endl;
    }
    cout << "[info]ËÄ´Î»ÓÊÖ½áÊø£¬Á¬½Ó¶Ï¿ª£¡ " << endl;
    return 1;
=======

	// å‘é€ç¬¬äºŒæ¬¡æŒ¥æ‰‹åº”ç­”æŠ¥æ–‡
	// è®¾ç½®ackä½
	ack = (u_char)header[SEQ_BITS_START] + ((u_char)header[SEQ_BITS_START + 1] << 8)
		+ ((u_char)header[SEQ_BITS_START + 2] << 16) + ((u_char)header[SEQ_BITS_START + 3] << 24) + 1;
	header[ACK_BITS_START] = (u_char)(ack & 0xFF);
	header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
	header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
	header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
	// è®¾ç½®seqä½
	seq = rand();
	header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
	header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
	header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
	header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
	// è®¾ç½®ACKä½
	header[FLAG_BIT_POSITION] = 0b100;
	sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));
	cout << "Send the Second Wavehand message!" << endl;

	// å‘é€ç¬¬ä¸‰æ¬¡æŒ¥æ‰‹è¯·æ±‚æŠ¥æ–‡
	// è®¾ç½®seqä½
	seq = rand();
	header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
	header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
	header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
	header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
	// ackå’Œä¸Šä¸€ä¸ªæŠ¥æ–‡ä¸€æ ·
	// è®¾ç½®ACK FINä½
	header[FLAG_BIT_POSITION] = 0b101;
	sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));
	cout << "Send the Third Wavehand message!" << endl;

	// æ¥æ”¶ç¬¬å››æ¬¡æŒ¥æ‰‹åº”ç­”æŠ¥æ–‡
	while (true) {
		recvResult = recvfrom(recvSocket, recvBuf, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, &len);
		// æ£€æŸ¥checksum
		checksum = checkSum(recvBuf, HEADERSIZE);
		// æå–ack of message 4
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

    serverAddr.sin_family = AF_INET;//Ê¹ÓÃIPV4
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    server = socket(AF_INET, SOCK_DGRAM, 0);
    cout << "[info]³É¹¦½¨Á¢socket " << endl;
    bind(server, (SOCKADDR*)&serverAddr, sizeof(serverAddr));//°ó¶¨Ì×½Ó×Ö£¬½øÈë¼àÌı×´Ì¬
    cout << "[info]½øÈë¼àÌı×´Ì¬£¬µÈ´ı¿Í»§¶ËÉÏÏß " << endl;

    int len = sizeof(serverAddr);
    //½¨Á¢Á¬½Ó
    Connect(server, serverAddr, len);
    cout << "[info]³É¹¦½¨Á¢Á¬½Ó£¬ÕıÔÚµÈ´ı½ÓÊÕÎÄ¼ş " << endl;

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
    cout << "ÎÄ¼şÒÑ³É¹¦ÏÂÔØµ½±¾µØ " << endl;
    system("pause");
=======
		// æ£€æŸ¥æ˜¯å¦æ˜¯æŒ¥æ‰‹æŠ¥æ–‡
		if (recvBuf[FLAG_BIT_POSITION] == 0b101) {
			// è®°å½•ä¸€ä¸‹seq
			for (int i = 0; i < 4; i++) {
				header[SEQ_BITS_START + i] = recvBuf[SEQ_BITS_START + i];
			}
			cout << "Successfully received the Fisrt Wavehand message!" << endl;
			wavehand();
			return;
		}

		// æ£€æŸ¥æ˜¯å¦æ˜¯æ–‡ä»¶å
		if (recvBuf[FLAG_BIT_POSITION] != 0b1000) {
			continue; // å› ä¸ºå¯èƒ½ä¼šæ”¶åˆ°åœ¨sendç«¯ç»“æŸå‘é€åçš„è¿˜åœ¨è·¯ä¸Šçš„æŠ¥æ–‡ï¼Œè¿‡æ»¤æ‰å®ƒä»¬
		}

		// æå–header
		memcpy(header, recvBuf, HEADERSIZE);

		// æå–æ–‡ä»¶å
		if (header[FLAG_BIT_POSITION] == 0b1000) {
			memcpy(filename, recvBuf + HEADERSIZE, FILE_NAME_MAX_LENGTH);
		}

		// æ¥æ”¶æ–‡ä»¶å¤§å°
		recvResult = recvfrom(recvSocket, recvBuf, PACKETSIZE, 0, (SOCKADDR*)&sendAddr, &len);

		// æå–header
		memcpy(header, recvBuf, HEADERSIZE);

		// æå–æ–‡ä»¶å¤§å°
		if (header[FLAG_BIT_POSITION] == 0b10000) {
			filesize = atoi(recvBuf + HEADERSIZE);
		}
		cout << "[Begin]: Begin to receive a file, filename: " << filename << ", filesize: " << filesize << " bytes." << endl;

		// æ¥æ”¶æ–‡ä»¶å†…å®¹
		int hasReceived = 0; // å·²æ¥æ”¶å­—èŠ‚æ•°
		int seq_opp = 0, ack_opp = 0; // å¯¹æ–¹å‘é€æŠ¥æ–‡ä¸­çš„seq, ack
		int seq = 0, ack = 0; // è¦å‘é€çš„å“åº”æŠ¥æ–‡ä¸­çš„seq, ack
		int expectedSeq = ack + 1; // æœŸå¾…æ”¶åˆ°çš„packet seq
		int dataLength = 0; // æ¥æ”¶åˆ°çš„æ•°æ®æ®µé•¿åº¦(= recvResult - HEADERSIZE)
		u_short checksum = 0; // æ ¡éªŒå’Œï¼ˆä¸º0æ—¶æ­£ç¡®ï¼‰

		ofstream out;
		out.open(filename, ios::out | ios::binary | ios::app);
		while (true) {
			expectedSeq = ack + 1; // receiveç«¯å”¯ä¸€ä¸€ä¸ªæ¥æ”¶çª—å£

			memset(recvBuf, 0, PACKETSIZE);
			// memset(header, 0, HEADERSIZE);
			recvResult = recvfrom(recvSocket, recvBuf, PACKETSIZE, 0, (SOCKADDR*)&sendAddr, &len);
			if (recvResult == SOCKET_ERROR) {
				cout << "[Error]: Receive error! sleep!" << endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(2000));
				continue;
			}

			// æ£€æŸ¥æ ¡éªŒå’Œ and ACKä½
			checksum = checkSum(recvBuf, recvResult);
			if (checksum == 0 && recvBuf[FLAG_BIT_POSITION] == 0b100) {
				seq_opp = (u_char)recvBuf[SEQ_BITS_START] + ((u_char)recvBuf[SEQ_BITS_START + 1] << 8)
					+ ((u_char)recvBuf[SEQ_BITS_START + 2] << 16) + ((u_char)recvBuf[SEQ_BITS_START + 3] << 24);
				ack_opp = (u_char)recvBuf[ACK_BITS_START] + ((u_char)recvBuf[ACK_BITS_START + 1] << 8)
					+ ((u_char)recvBuf[ACK_BITS_START + 2] << 16) + ((u_char)recvBuf[ACK_BITS_START + 3] << 24);
				if (seq_opp == expectedSeq) { // æ£€æŸ¥æ”¶åˆ°çš„åŒ…çš„seq(å³seq_opp)æ˜¯å¦æ˜¯æœŸå¾…çš„seq
					// å¦‚æœæ”¶åˆ°äº†æ­£ç¡®çš„åŒ…ï¼Œé‚£å°±æå–å†…å®¹ + å›å¤
					dataLength = recvResult - HEADERSIZE;
					// æå–æ•°æ®
					memcpy(dataSegment, recvBuf + HEADERSIZE, dataLength);
					out.write(dataSegment, dataLength);

					// è®¾ç½®seqä½ï¼Œæœ¬åè®®ä¸­ä¸ºäº†ç¡®è®¤æ–¹ä¾¿ï¼Œå°±æŠŠå“åº”æŠ¥æ–‡çš„seqç½®ä¸ºæ”¶åˆ°æŠ¥æ–‡çš„seq
					seq = seq_opp;
					header[SEQ_BITS_START] = (u_char)(seq & 0xFF);
					header[SEQ_BITS_START + 1] = (u_char)(seq >> 8);
					header[SEQ_BITS_START + 2] = (u_char)(seq >> 16);
					header[SEQ_BITS_START + 3] = (u_char)(seq >> 24);
					// è®¾ç½®ackä½, = seq_oppï¼Œè¡¨ç¤ºç¡®è®¤æ¥æ”¶åˆ°äº†è¿™ä¹‹å‰çš„å…¨éƒ¨å†…å®¹ï¼Œå¹¶æœŸå¾…æ”¶åˆ°è¿™ä¹‹åçš„å†…å®¹
					ack = seq_opp;
					header[ACK_BITS_START] = (u_char)(ack & 0xFF);
					header[ACK_BITS_START + 1] = (u_char)(ack >> 8);
					header[ACK_BITS_START + 2] = (u_char)(ack >> 16);
					header[ACK_BITS_START + 3] = (u_char)(ack >> 24);
					// è®¾ç½®ACKä½
					header[FLAG_BIT_POSITION] = 0b100;
					// å“åº”æŠ¥æ–‡ä¸­çš„data lengthä¸º0ï¼Œå°±ä¸ç”¨è®¾ç½®äº†

					hasReceived += recvResult - HEADERSIZE;
					cout << "[Recv]: Received " << hasReceived << " bytes, ack = " << ack << ", seq = " << seq << endl;

					// æ¨¡æ‹Ÿå»¶æ—¶
					if (dis(gen) < DELAY_RATE)
						std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_TIME));

					// æ¨¡æ‹Ÿä¸¢åŒ…
					if (dis(gen) > DISCARD_RATE)
						sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));
				}
				else {
					// è¯´æ˜ç½‘ç»œå¼‚å¸¸ï¼Œä¸¢äº†åŒ…æˆ–è€…æœ‰å»¶è¿Ÿï¼Œæ‰€ä»¥ä¸ç”¨æ›´æ”¹ï¼Œç›´æ¥é‡å‘æ”¶åˆ°çš„æœ€æ–°åŒ…çš„ackå³å¯

					// æ¨¡æ‹Ÿå»¶æ—¶
					if (dis(gen) < DELAY_RATE)
						std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_TIME));

					// æ¨¡æ‹Ÿä¸¢åŒ…
					if (dis(gen) > DISCARD_RATE)
						sendto(recvSocket, header, HEADERSIZE, 0, (SOCKADDR*)&sendAddr, sizeof(SOCKADDR));

					cout << "[Error]: Expected seq = " << expectedSeq << ". Received seq = " << seq_opp << endl;
				}
			}
			else {
				// æ ¡éªŒå’Œæˆ–ACKä½å¼‚å¸¸ï¼Œé‡å‘ä¸Šä¸€ä¸ªåŒ…çš„ack
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

	recvAddr.sin_family = AF_INET; // åè®®ç‰ˆæœ¬
	recvAddr.sin_addr.S_un.S_addr = inet_addr(IP); // ipåœ°å€ï¼Œinet_addræŠŠæ•°ç‚¹æ ¼å¼è½¬æ¢ä¸ºæ•´æ•°
	recvAddr.sin_port = htons(PORT); // ç«¯å£å·ï¼Œ0-65535
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