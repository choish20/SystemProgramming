#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <stdbool.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>



#define WORDMAX 50
#define FILEBUFFER 1000
#define WORDINSTAGE 20
#define DROPSPEED 1

#define WORD_LEN 20

void game_main();
void CreateFile_main();
void *output(void * m);
void gotoxy(int x, int y);
int read_word();
int getch();
void input_clear(char* input);
void screen_clear();
void fail_screen();
void clear_screen();


struct word
{
	int x, y;   //�ܾ��� x,y��
	bool is_print;   //�ܾ� ��� ����
	char text[WORD_LEN]; //�ܾ� ����
};

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
struct word playword[WORDINSTAGE]; // �������� ����� �ܾ� �迭
int life = 5;            // ���ӿ��� ����� ���
int score = 0;
int check = 0;

int main()
{
	int user_select = 0;
	int mode = 0;

	while (user_select != 4) {
		while (1) {
			system("clear");
			gotoxy(0, 0);
			printf("�޴� : 1.�꼺�� Ÿ�ڿ��� 2.Ÿ�ڿ��� ���� �߰� 3. ����\n");
			printf(">");
			scanf("%d", &user_select);
			getchar();
			if (user_select >= 1 && user_select <= 4)
				break;
			else {
				printf("�˸��� ���� �Է��ϼ���\n");

			}
		}

		switch (user_select) {
		case 1:// �꼺��Ÿ�ڿ���
			system("clear");
			game_main();
			score = 0;
			life = 5;
			check = 0;
			break;
		case 2: // Ÿ�ڿ��������߰�
			CreateFile_main();
			break;
		case 3: //����
			return 0;
			break;
		default:
			break;
		}

	}
	return 0;
}
void game_main() {
	pthread_t th;
	int x = 10;
	int y = 25;
	char input[WORD_LEN];
	int cur_len = 0;


	system("clear");
	if (!read_word()) return;
	gotoxy(71, 8);
	printf("LIFE: %3d", life);
	gotoxy(71, 10);
	printf("SCORE: %3d", score);

	pthread_create(&th, NULL, output, NULL);

	while (life > 0 && check < WORDINSTAGE) {
		gotoxy(0, y - 1);
		printf("-----------------------------------------------------------");
		gotoxy(x, y);
		printf("�Է�: ");
		x += 6;
		// �ݺ���
		while (true) {
			int c = getch();
			if (c == 10) { //enter�� ��
				x = 10;
				gotoxy(x, y);
				printf("                                        ");
				input[cur_len] = '\0';
				cur_len = 0;
				for (int i = 0; i < WORDINSTAGE; i++) {
					if (!strcmp(input, playword[i].text)) {
						playword[i].is_print = false;
						pthread_mutex_lock(&lock);
						check++;
						pthread_mutex_unlock(&lock);
						input_clear(input);
						score += 10;
						gotoxy(71, 10);
						printf("SCORE: %3d", score);
						break;
					}
				}
				break;
			}
			if (c == 127 && cur_len > 0) {
				gotoxy(x, y);
				printf("\b \b");
				x--;
				cur_len--;
			}
			if (((c >= 'a'&& c <= 'z') || (c >= 'A'&& c <= 'Z')) && cur_len < 20) {
				gotoxy(x, y);

				printf("%c", c);
				input[cur_len] = c;
				x++;
				cur_len++;
			}
		}
	}
	// ������ ����
	pthread_join(th, NULL);
}
void CreateFile_main()//�����߰� ����
{
	char fileName[55] = { 0, }; //���� �̸� ��� ����
	char *txt = ".txt";
	char context[FILEBUFFER] = { 0, };
	int len = 0;
	int fd;

	printf("�߰��� ���� �̸��� �Է��ϼ���.(����. �ִ� 50��) : ");
	scanf("%s", fileName);
	len = strlen(fileName);
	for (int j = 0; j<4; j++) {
		fileName[len + j] = txt[j];
		// printf("s");
	}
	fd = open(fileName, O_RDWR | O_CREAT, S_IRWXU);//������ �а� ���� ����.

	if (fd == -1) {
		printf("������ �������� �ʾҽ��ϴ�.");
		printf("���θ޴��� ���ư��ϴ�.\n");
		sleep(1);
		return;
	}
	getchar();
	//printf("%s",fileName);
	printf("�ܾ �Է��ϼ���.(����� �������ּ���. 20�ܾ� �̻� 100�ܾ� ����)\n");
	//WORDINSTAGE 20�� ����.
	scanf("%[^\n]s", context);
	write(fd, context, 1000);
	// printf("%s",context);
	close(fd);

}
void *output(void * m)
{

	int start_time; //���� ���� ���� �ð�
	int current_time;

	int print_num = 0;   //��¼���
	int i = 0;


	/*
	���� ��������� �ܾ� ��� �޾Ƽ� ��ū���� �����Ͽ� ����
	*/

	print_num = 0;

	start_time = time(0); //����ð� �ҷ�����
	if (start_time == -1)
	{
		printf("�����߻�\n");
		return;
	}

	// ���� ����
	while (1)
	{

		if (life == 0)
		{
			fail_screen();
			break;
		}
		if (check >= WORDINSTAGE)
		{
			clear_screen();
			break;
		}
		//printf("%d",print_num);
		current_time = time(0);
		if (current_time == -1)
		{
			printf("�����߻�\n");
			return;
		}

		if ((current_time - start_time) % DROPSPEED == 0)
		{
			//printf("%d,%d",current_time - start_time,(current_time - start_time) % DROPSPEED);

			screen_clear();
			//ȭ�� �����
			//printf("����");
			//printf("%d",print_num);
			if (print_num < WORDINSTAGE * 2)
			{
				playword[print_num / 2].is_print = true;
			}

			//���� ����
			for (i = 0; i < WORDINSTAGE; i++)
			{
				if (playword[i].is_print == true)
				{
					playword[i].y++;
				}
			}

			for (i = 0; i < WORDINSTAGE; i++)
			{
				if (playword[i].is_print == true)
				{

					gotoxy(playword[i].x, playword[i].y);
					printf("%s", playword[i].text);
					fflush(stdout);
				}
			}

			for (i = 0; i < WORDINSTAGE; i++)
			{
				if (playword[i].is_print && playword[i].y == 22)
				{
					pthread_mutex_lock(&lock);
					life--;
					check++;
					pthread_mutex_unlock(&lock);
					gotoxy(71, 8);
					printf("LIFE: %3d", life);
					playword[i].is_print = false;
				}
			}

			print_num++;
			usleep(200000);
		}
	}
}
void gotoxy(int x, int y) {
	printf("\033[%d;%df", y, x);
	fflush(stdout);
}

int getch()
{
	int c;
	struct termios oldattr, newattr;

	tcgetattr(STDIN_FILENO, &oldattr);           // ���� �͹̳� ���� ����
	newattr = oldattr;
	newattr.c_lflag &= ~(ICANON | ECHO);         // CANONICAL�� ECHO ��
	newattr.c_cc[VMIN] = 1;                      // �ּ� �Է� ���� ���� 1�� ����
	newattr.c_cc[VTIME] = 0;                     // �ּ� �б� ��� �ð��� 0���� ����
	tcsetattr(STDIN_FILENO, TCSANOW, &newattr);  // �͹̳ο� ���� �Է�
	c = getchar();                               // Ű���� �Է� ����
	tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);  // ������ �������� ����
	return c;
}


void screen_clear() {
	for (int i = 0; i < 23; i++) {
		gotoxy(0, i);
		printf("                                                                     ");
	}
}

void input_clear(char* input) {
	for (int i = 0; i < strlen(input); i++) {
		input[i] = '\0';
	}
}

int read_word() {
	int fd;
	char fileBuffer[1000];
	char fileName[55] = { 0, };
	int len;
	char *txt = ".txt";


	printf("Ÿ�� ������ ���� �̸��� �Է��ϼ���.(����. �ִ� 50��) : ");
	scanf("%s", fileName);
	len = strlen(fileName);
	for (int j = 0; j<4; j++) {
		fileName[len + j] = txt[j];
		// printf("s");
	}
	fd = open(fileName, O_RDONLY);
	if (fd == -1)
	{
		printf("�ش� ������ �����ϴ�!\n");
		printf("���θ޴��� ���ư��ϴ�..\n");
		sleep(1);
		return 0;
	}
	else
	{
		read(fd, fileBuffer, 1000);
	}
	close(fd);

	char wordList[101][WORD_LEN];
	char *nextContext = NULL;
	char *ptr = strtok_r(fileBuffer, " ", &nextContext);
	int word_max;

	for (int i = 0; i < 101; i++)
	{
		if (ptr == NULL) {
			word_max = i;
			break;
		}
		strcpy(wordList[i], ptr);
		ptr = strtok_r(NULL, " ", &nextContext);
	}
	// for(int i = 0; i < 100; i++)
	//    printf("%s  ", wordList[i]);

	//���� ������������ ����� �ܾ� ���� ����

	srand((unsigned int)time(NULL));
	for (int i = 0; i < WORDINSTAGE; i++)
	{
		strcpy(playword[i].text, wordList[rand() % word_max]);
		for (int j = 0; j < i; j++)
		{
			if (strcmp(playword[i].text, playword[j].text) == 0)
			{
				i--;
				break;
			}
		}

		playword[i].is_print = false;
		playword[i].x = (rand() % 56) + 1; // ���� x ��ǥ ����
		playword[i].y = 1;
		//printf("%d.%s ",i,playword[i].text);
		fflush(stdout);
	}
	return 1;
}

void fail_screen() {
	gotoxy(10, 7);
	printf("������������������������<FAIL>����������������������������");
	gotoxy(10, 8);
	printf("��     SCORE  :  %8d       ��", score);
	gotoxy(10, 9);
	printf("��                              ��");
	gotoxy(10, 10);
	printf("��    Press [ENTER] to quit     ��");
	gotoxy(10, 11);
	printf("��                              ��");
	gotoxy(10, 12);
	printf("����������������������������������������������������������������");
	gotoxy(10, 13);
}

void clear_screen() {
	gotoxy(10, 7);
	printf("������������������������<CLEAR>��������������������������");
	gotoxy(10, 8);
	printf("��       Congratulations!!!     ��");
	gotoxy(10, 9);
	printf("��     SCORE  :  %8d       ��", score);
	gotoxy(10, 10);
	printf("��                              ��");
	gotoxy(10, 11);
	printf("��    Press [ENTER] to quit     ��");
	gotoxy(10, 12);
	printf("��                              ��");
	gotoxy(10, 13);
	printf("����������������������������������������������������������������");
	gotoxy(10, 14);
}
