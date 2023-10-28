###  2110688 史文天

# 网页抓包分析

（1）搭建Web服务器（自由选择系统），并制作简单的Web页面，包含简单文本信息（至少包含专业、学号、姓名）、自己的LOGO、自我介绍的音频信息。页面不要太复杂，包含要求的基本信息即可。

（2）通过浏览器获取自己编写的Web页面，使用Wireshark捕获浏览器与Web服务器的交互过程，并进行简单的分析说明。

（3）使用HTTP，不要使用HTTPS。

# 实验过程

## 搭建服务器
- 使用Node.js在本地部署服务器。

  ```javascript
  const express = require('express');
  const app = express();
  const path = require('path');
  const os = require('os');
  
  app.use(express.static(path.join(__dirname, './')));
  
  const port = 6200;
  
  app.listen(port, '0.0.0.0', () => {
      const networkInterfaces = os.networkInterfaces();
      let ipAddress = '127.0.0.1';
  
      // 遍历网络接口，找到非 localhost 下的 IPv4 地址
      
      Object.keys(networkInterfaces).forEach((interfaceName) => {
          networkInterfaces[interfaceName].forEach((networkInterface) => {
              if (!networkInterface.internal && networkInterface.family === 'IPv4') {
                  ipAddress = networkInterface.address;
              }
          });
      });
      console.log(`Availble on:`);
      console.log(`http://localhost:${port}`);
      console.log(`http://${ipAddress}:${port}`);
  });
  ```

  在文件夹下打开命令行窗口，运行Node命令即可部署服务器。

## 网页HTML设计
- 网页包括文本信息，logo，音频信息，使用JavaScript实现了音频播放部分。省略了控制风格的css部分，具体参考html文件。

  ```html
  <!DOCTYPE html>
  <html lang="en">
  
  <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>2110951自我介绍</title>
      <style>
      </style>
  </head>
  
  <body>
      <div id="app">
          <div class="container">
              <div class="logo">
                  <img src="icon.jpg" alt="logo" class="icon">
              </div>
              <div class="info">
                  <p class="title">我的主页</p>
                  <p>史文天</p>
                  <p>2110688</p>
                  <p>信息安全二班</p>
              </div>
              <a class="github" href="https://github.com/FlandreScarlet36" target="_blank">
                  访问我的Github
              </a>
              <div class="audio">
                  <p>音频介绍</p>
                  <audio id="audioElement" src="audio.wav"></audio>
                  <button id="playButton" onclick="togglePlay()"></button>
                  <div class="progressBar">
                      <div class="progressBarFill" :style="{ width: progress + '%' }"></div>
                  </div>
              </div>
          </div>
      </div>
      <script>
          var audioElement = document.getElementById('audioElement');
          audioElement.addEventListener('timeupdate', updateProgressBar);
          audioElement.addEventListener('ended', resetAudio);
  
          var isPlaying = false;
          var playButton = document.getElementById('playButton');
          playButton.textContent = isPlaying ? '⏸' : '▶️';
          var progress = 0;
  
          function togglePlay() {
              if (isPlaying) {
                  audioElement.pause();
                  playButton.textContent = '▶️';
              } else {
                  audioElement.play();
                  playButton.textContent = '⏸';
              }
              isPlaying = !isPlaying;
          }
  
          function updateProgressBar() {
              var progressBarFill = document.querySelector('.progressBarFill');
              progress = (audioElement.currentTime / audioElement.duration) * 100;
              progressBarFill.style.width = progress + '%';
          }
  
          function resetAudio() {
              audioElement.currentTime = 0;
              progress = 0;
              isPlaying = false;
              playButton.textContent = '▶️';
          }
          var audioElement = new Audio('audio.wav');
          audioElement.addEventListener('timeupdate', updateProgressBar);
          audioElement.addEventListener('ended', resetAudio);
          var isPlaying = false;
          var progress = 0;
      </script>
  </body>
  
  </html>
  ```

效果展示

![image-20231027231530564](C:\Users\25265\AppData\Roaming\Typora\typora-user-images\image-20231027231530564.png)

## 抓包分析

使用ip地址和tcp.port条件作为筛选器，只留下该网页与服务器的通讯记录。

![image-20231027223236688](C:\Users\25265\AppData\Roaming\Typora\typora-user-images\image-20231027223236688.png)

### 三次握手分析

![image-20231027223611905](C:\Users\25265\AppData\Roaming\Typora\typora-user-images\image-20231027223611905.png)

第一次握手由客户端向服务器发送请求，可以看到Seq=0，代表初次建立连接，这次握手将标志位SYN置为1，表示请求建立连接。

![image-20231027224001355](C:\Users\25265\AppData\Roaming\Typora\typora-user-images\image-20231027224001355.png)

第二次握手由服务器向客户端发送反馈，SYN和ACK为1，表示服务器同意进行连接。

![image-20231027224234674](C:\Users\25265\AppData\Roaming\Typora\typora-user-images\image-20231027224234674.png)

第三次握手看到Seq=1，表示并非第一次建立连接。ACK为1，表示收到了服务器的回复。

![image-20231027224640261](C:\Users\25265\AppData\Roaming\Typora\typora-user-images\image-20231027224640261.png)

### HTTP部分

![image-20231027225101683](C:\Users\25265\AppData\Roaming\Typora\typora-user-images\image-20231027225101683.png)

第一个GET请求用于获取HTML界面。状态码304表示客户端有最新的缓存资源，并与服务器上的资源相同，因此服务器发送一个空包和304状态码，告诉客户端可以使用本地缓存资源，用于减少网络流量的浪费。

获取HTML界面后继续发送GET请求，获取icon.jpg，同样返回304状态码。

获取audio.MP3，返回206状态码，这种响应是在客户端表明自己只需要目标URL上的部分资源的时候返回的，是由于客户端在加载一个较大的文件，无法用一个包发送所有文件，因此采用断点续传发送。

### 四次挥手

![image-20231027230109073](C:\Users\25265\AppData\Roaming\Typora\typora-user-images\image-20231027230109073.png)

第一次挥手客户端向服务器发送带有FIN和ACK标志位的包，FIN表示客户端要求关闭连接，ACK=2244表示确认了服务器发送的序号为2244的数据，查找后发现是audio.MP3的数据。

![image-20231027230508895](C:\Users\25265\AppData\Roaming\Typora\typora-user-images\image-20231027230508895.png)

第二次挥手服务器向客户端发送带有ACK标志位的包，表示对客户端请求的确认。

第三次挥手客户端收到服务器的确认信息，发送带有FIN和ACK标志位的包，表示客户端准备关闭连接，在客户端发来ACK包后就会关闭连接。

第四次挥手服务器向客户端发送带有ACK标志位的包，关闭服务器连接，同时客户端服务器也准备关闭连接。

四次挥手后，等待2MSL后，客户端也关闭连接，TCP连接成功断开。

