Ŀ�ģ�

1. ����Ӧ�ó���ͨ�� block��nonblock���ַ�ʽ�����豸������ο�LDD3 �� scull example
2. ��ʾ���ʵ�����̵߳���pthread_cancelȥcancelһ������ blocking read�� thread �������
driver�� read callback�е��� wait_event_interruptible����wait״̬����ʱ�����߳�cancel���̡߳����Գɹ�cancel����blocking���̡߳�


ʹ�÷�����

��rw_wake_drv.ko��led_test/read_led�����������׼��Ϻ�����װ��led.ko������
	insmod rw_wake_drv.ko

����������Ϻ�����led_test���ɣ�
    ./led_test -> for point 1
    ./read_led -> for point 2
