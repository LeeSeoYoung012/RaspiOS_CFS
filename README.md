# RaspiOS_CFS

https://github.com/s-matyukevich/raspberry-pi-os.git의 lesson4 기존코드에 있는 간단한 스케줄링을 CFS 스케줄러로 바꾸었다. 

This is the code that transformed the scheduler of the rpi-os Lesson4 code(https://github.com/s-matyukevich/raspberry-pi-os.git ) into the CFS scheduler.

# 사용방법

**1.qemu 설치**

`wget https://download.qemu.org/qemu-4.2.0.tar.xz`
  
`tar xvJf qemu-4.2.0.tar.xz`

`cd qemu-4.2.0`

`./configure`

`make`

`sudo make install`

**2.raspi3 지원 확인**

`qemu-system-aarch64 -machine help`

**3.코드실행**

`make`

`qemu-system-aarch64 -m 128 -M raspi3 -serial null -serial mon:stdio -nographic -kernel kernel8.img`

# 실행결과

task[0]은 init process, task[1]은 12345 출력, task[2]는 abcde 출력 

![cfs_](https://user-images.githubusercontent.com/60209292/87318066-cb824300-c562-11ea-8d78-089e2cd26ad0.png)

