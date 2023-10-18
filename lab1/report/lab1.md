

# 线性攻击算法

## （一）线性攻击算法实现

按照给定伪代码实现算法即可。下面对核心代码进行说明：

* 随机生成16000个16位二进制串，将其放入前面得到的SPN加密算法中，以`00111010100101001101011000111111`为密钥进行加密得到密文，然后将明密文对写入`pairs.txt`文件，得到本次实验的数据集。该密钥$K_5$轮密钥为`1101011000111111`。

  ```python
  import subprocess
  import random
  from tqdm import tqdm
  
  def generate_random_binary_string(length=16):
      return ''.join(random.choice('01') for _ in range(length))
  
  executable_file_path = "./exe/SPN.exe"
  num_pairs = 16000
  
  with open("./data/pairs.txt", "w") as file:
      for _ in tqdm(range(num_pairs), desc="Generating Pairs", unit="pair"):
          input_data = generate_random_binary_string()
          result = subprocess.run([executable_file_path], input=input_data.encode(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
          output_data = result.stdout.decode().strip()
  
          file.write(input_data + "\n")
          file.write(output_data + "\n")
  
  print(f"Generated {num_pairs} pairs and saved to pairs.txt.")
  ```

* 全局部分定义了`S`盒、明文与密文数组及计数器。

  ```c++
  int S[16] = { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 };
  int x[16] = {};//明文
  int y[16] = {};//密文
  int Count[16][16] = {};//计数器
  ```

* 接着定义了一个函数`DecimalToBinary`，用于将十进制数转换成四位二进制，并将其按照给定的索引位存入数组。

  ```c++
  void DecimalToBinary(int decimal, int* binary, int num)
  {
      int i = num - 3;//最低索引位
      while (decimal > 0)
      {
          binary[num] = decimal % 2;
          decimal /= 2;
          num--;
      }
      while (num >= i)//空位补0
      {
          binary[num] = 0;
          num--;
      }
  }
  ```

* 下面便是`main`函数部分。

  * 首先对`S`盒进行逆运算，存入数组`S1`。

    ```c++
    int S1[16] = {};//S盒的逆
    for (int i = 0; i < 16; i++)
    {
      S1[S[i]] = i;
    }
    ```

  * 接着使用文件流读入已经得到的数据集。

    ```c++
    ifstream input(".\\data\\pairs.txt");
    ```

  * 然后遍历`(0,0) to (F,F)`，按照算法内容进行计算，得到对应的计数器。

    ```c++
    for (int j = 0; j < 16; j++)
    {
        for (int k = 0; k < 16; k++)
        {
            //for(L_1,L_2) <- (0,0) to (F,F)
            DecimalToBinary(j, L1, 3);
            DecimalToBinary(k, L2, 3);
    
            //L_1与y_{<2>}异或
            v[4] = L1[0] ^ y[4];
            v[5] = L1[1] ^ y[5];
            v[6] = L1[2] ^ y[6];
            v[7] = L1[3] ^ y[7];
            //L_2与y_{<4>}异或
            v[12] = L2[0] ^ y[12];
            v[13] = L2[1] ^ y[13];
            v[14] = L2[2] ^ y[14];
            v[15] = L2[3] ^ y[15];
    
            int temp1 = v[4] * pow(2, 3) + v[5] * pow(2, 2) + v[6] * pow(2, 1) + v[7] * pow(2, 0);
            int temp2 = S1[temp1];//S盒的逆运算
            DecimalToBinary(temp2, u, 7);
    
            temp1 = v[12] * pow(2, 3) + v[13] * pow(2, 2) + v[14] * pow(2, 1) + v[15] * pow(2, 0);
            temp2 = S1[temp1];//S盒的逆运算
            DecimalToBinary(temp2, u, 15);
    
            int z = x[4] ^ x[6] ^ x[7] ^ u[5] ^ u[7] ^ u[13] ^ u[15];
    
            if (z == 0) Count[j][k]++;
        }
    }
    ```

  * 最后分析比较计数器的值，输出最大可能的轮密钥。

    ```c++
    int max = -1;
    int LL1 = 0, LL2 = 0;//记录最大的Count[i][j]对应的L1和L2
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            Count[i][j] = abs(Count[i][j] - n / 2);
            if (Count[i][j] > max)
            {
                max = Count[i][j];
                LL1 = i;
                LL2 = j;
            }
        }
    }
    
    cout << "maxkey:" << endl;
    DecimalToBinary(LL1, L1, 3);
    for (int i = 0; i < 4; i++)
    {
        cout << L1[i];
    }
    cout << " ";
    DecimalToBinary(LL2, L2, 3);
    for (int i = 0; i < 4; i++)
    {
        cout << L2[i];
    }
    ```

## （二）密钥分析

该密钥$K_5$轮密钥为`1101011000111111`，则$L_1=0110$，$L_2=1111$。

* 当使用8000对明密文对进行攻击时，可以看到输出结果正确，说明攻击成功，用时约631128微秒。

  <img src="./pic/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20231009221647.png" style="zoom:67%;" />

* 下面尝试减少明密文对数量，测试攻击成功率。

  * 使用4000对时也能得到正确结果，用时约为272263微秒。

    <img src="./pic/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20231009221901.png" style="zoom:67%;" />

  * 使用2000对时也能得到正确结果，用时约为135626微秒。

    <img src="./pic/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20231009222013.png" style="zoom:67%;" />

  * 经过测试，对于该组密钥，最少使用801组即可攻击成功（由于概率问题，每次攻击需要的数目可能会上下波动，该数值仅对下面图片负责）。

    <img src="./pic/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20231009222135.png" style="zoom: 67%;" />
  
* 修改传入密钥，得到新的数据集，测试攻击成功率。
  
  * 密钥为
  
    * `00000000000000000000000000000000`
      * 其轮密钥为`00000000`
    * `11111111111111111111111111111111`
      * 其轮密钥为`11111111`
    * `01010101010101010101010101010101`
      * 其轮密钥为`01010101`
    * `10101010101010101010101010101010`
      * 其轮密钥为`10101010`
  
  * 经过测试，当明密文对数为4000时成功率已经足够高，以上五组攻击均能成功。
  
    <img src="./pic/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20231009225406.png" style="zoom:67%;" />

* 在2得到第2、4部分轮密钥后，对于剩余的八位轮密钥进行分析。
  * 选择新的线性分析链，在第2、4部分密钥已知的基础上分析出第1、3部分的密钥。接着在已知起始密钥低16位的基础上，穷举高16位密钥对给出的8000个明密文对进行验证。由于在加密过程中进行了5轮的S代换和P置换，导致相同明文在不同密钥下得到相同密文的概率极低，因此在实际验证过程中并不需要验证8000个明密文对是否对应，而仅需验证3个即可判断出密钥是否合适。
  * 由于虽然可以选取到仅包含第5轮第1、3部分的线性分析链，但是由于偏差不大，因此代表性较差，可能导致对于1、3部分密钥可能性较大部分的遍历过多，而导致时间开销较大。
