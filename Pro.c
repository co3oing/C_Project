#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <termio.h> // ���

#define N 11 // �̸� �迭 �� ����

char map_f[30][30][6]; // ������ ����, [x��ǥ][y��ǥ][undo ����]
char empty[30][30]; // ���� �ڸ� ��ġ ������ ���� ���� �� ������ �迭 ����
int box = 0, rm_pl = 0; // box : �������� ����, rm_pl : ���� �ڸ� ����
int p_x, p_y;  // player ��ǥ
int com_y; // COMMAN ���� Y�� ��ǥ
char name[N]; // �̸�
int m_n = 1; // �� �ѹ�
int cnt_u = 0; // undo Ƚ��
char b_key[5]; // back key : ������ ������ ����Ű ����, undo�� ���� �÷��̾� ��ǥ�� �̵��� �̿�
int k_mv = 0; // �÷��̾� �̵� Ƚ�� : undo ���� ����
time_t start, end; // �ð� ���۰� �� ����
int dif = 0; // �ð� ����
int line_n[5] = { 0,0,0,0,0 }; // ranking �� ī��Ʈ
unsigned int rk_t[5][5]; // ��ũ �ð�
char rk_n[5][5][11]; // ��ũ �̸�

void name_w(); // �̸� ����
void mv_cur(int x, int y); // Ŀ�� ��ġ ����
void map(int m_num); // �� ���, �迭�� ����
int getch(); // getch
void key(); //Ű���� �Է�
void com_cur(); // Ŀ�ǵ� ��ġ Ŀ�� �̵�
void dis(); // ����
void move(char c); // �÷��̾� �̵�
void undo_a(); // undo act
void undo_s(); // undo save
void undo_k(char c); //undo key save
void chk_p(); // place check
int clr(); // �� Ŭ���� üũ
void save(); // ���̺� ���� ����
void load(); // ���̺� ���� �ε�
void rank_r(); // ��ũ ���� �б�
void rank_m(); // ��ũ ���� ����
void rank_i(); // ��ũ ����
void rank_p(char op); // ��ũ ���, op : 0 ~ 5 (0 : ��ü, 1 ~ 5 : �ش� ��)

int main() {
	name_w(); //�̸� ����
	rank_r(); //��ũ���� �б�
	map(m_n++); // ���� ���� ����ϱ� ���� m_n++ �̿�
	com_cur();
	time(&start); // �� ��� �� ���� �ð� ����
	while (1) {
		key(); 
		if (m_n == 6) // 5��° �� ���� while�� ����
			break;
		system("clear"); // ���� �� ��� ���� ȭ�� ����
		map(m_n++);
		k_mv = 0; // Ű���� ������ Ƚ�� �ʱ�ȭ
		cnt_u = 0; // �� Ƚ�� �ʱ�ȭ
		com_cur(); // Ŀ�� ��ġ�� �̵�
		dif = 0; // �� �ð� �ʱ�ȭ
		time(&start); // Ŭ���� �� �ٽ� ���� �ð� �ʱ�ȭ
	}
	system("clear");
	mv_cur(0, 0);
	printf("\n\n\n       Congratulation!!!\n\n\n"); // ���� �޼���
	return 0;
}

void name_w() {
	int i = 0, c;

	system("clear");
	printf("Start...\n"); // ����
	printf("input name : "); // �̸� �Է�
	while (1) { 
		c = getchar();
		if ((c == ' ') || (i == N)) { // 10���� �̻��̰ų� ������������ ���Է�
			if (c != '\n') // 11���� �Է½� ����� ���������� ����
				while ((c = getchar()) != '\n');
			printf("�̸��� �ٽ� �Է��ϼ��� : "); // ���
			i = 0;
			continue; // �ݺ��� ó������ �̵�
		}
		if ((c == '\n') || (c == EOF)) { // ���ͱ��� �Է�
			name[i++] = '\0';  // �������� �ι��ڷ� ��ü
			break; // �ݺ��� ����
		}
		name[i++] = c; // �̸��� �迭�� �Է�
	}
}

void com_cur() {
	mv_cur(11, com_y); // Ŀ�� ��ġ �̵�
	printf("                                    ");  // ������� ������ ���� ���� ���
	mv_cur(11, com_y); // Ŀ�� ��ġ �� �̵�
}

void map(int m_num) { // m_num : 1 ~ 5
	FILE *fp = NULL; // �� ���� ���� ����
	if (!fopen("map.txt", "r")) { // �� ������ �����ϴ����� ���� Ȯ��
		printf("Map File Doesn't exist.");
		exit(1);
	}
	fp = fopen("map.txt", "r"); // �� ���� �Է�
	char c; // �Է� ���� ���� ����
	int i = 0, j = 0;  // ��ǥ
	int num1 = 0, num2 = 0; // ���ϴ� ���� ����ϱ� ���� ������ ����
	box = 0; // �ڽ� ����
	rm_pl = 0; // �ڽ��� �� ������ ����
	for (int i = 0; i < 30; i++)
		for (int j = 0; j < 30; j++) { // �ݺ����� �̿��� �� ���� ����
				empty[i][j] = '\0'; // ���� ������ ���
			for (int k = 0; k < 6; k++) // �𵵸� ���� ������ �����ϱ��� ���� ���� �ݺ��� ����
				map_f[i][j][k] = '\0'; // \0 = ���������ʴ� ����, NULL
		}
	while (1) {
		fscanf(fp, "%c", &c); // ���� ��ĵ
		if (c == 'm') 
			num1++;
		if (c == 'p')
			num2++; // m�� p�� �̿��ؼ� ���ϴ� ������ ���
		if (num1 == m_num && num2 == m_num && c != 'p' && c != 'e') { // ���ϴ� ��ȣ�� �������� ����ϰ� ���� �����ϱ����� ���� �߰�
			map_f[i][j][0] = c; // �Է�
			empty[i][j] = c; // ���� ���� ����
			if (c == '@') { // �÷��̾� ��ġ ��ǥ ����
				p_x = i;
				p_y = j;
			}
			else if (c == 'O') // �ڽ� �־���ϴ� ���� ����
				rm_pl++;
			else if (c == '$') // �ڽ� ����
				box++;
			if (c == '\n') { // ����� y���� ��ǥ ���� �� x�� ��ǥ �ʱ�ȭ
				j++;
				i = 0;
			}
			else 
				i++; // x�� ��ǥ�� ����
		}
		if (num1 > m_num || c == 'e') {
			com_y = j + 3; // (Command)�� y�� ��ǥ ����
			break; // �ݺ��� Ż��
		}
	}
	system("clear");
	mv_cur(0, 0);
	printf("\n      Hello %s\n\n", name);
	if (box != rm_pl)
		printf("Map%d File Error", m_num); // �ڽ��� ������ �ڽ��� ������ ������ ��
	else
		for (int a = 1; a < j; a++)
			for (int b = 0; b < 30; b++)
				printf("%c", map_f[b][a][0]); // �� ���
	printf("(Command) "); // Ŀ�ǵ� ���
}

int getch() { // getch()
	int ch;

	struct termios buf;
	struct termios save;

	tcgetattr(0, &save);
	buf = save;

	buf.c_lflag &= ~(ICANON | ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	tcsetattr(0, TCSAFLUSH, &buf);

	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &save);

	return ch;
}

void mv_cur(int x, int y) { // ������ ȯ�漳���� ���� Ŀ���� ��ǥ �̵�
	printf("\033[%dG\033[%dd", x, y);
}

void key() { // ������ Ű���� ���� ���� ��� ����
	char c, bef, r_num; // ������ ����
	while (c = getch()) { //while
		switch (c) {
			case 'h' :
			case 'j' :
			case 'k' :
			case 'l' : // �¿������ ����Ű
				com_cur();
				move(c); // ���� �Լ��� ���� ���⿡ ���� �Լ� ����
				break;
			case 10 : // ����(Enter)Ű�� �ƽ�Ű �ڵ尪 : 10
				com_cur();
				switch (bef) { // ����Ű�� ������ �Է� ���� ���� �־��� ��, �׿� ���� ������ ���� switch�� ����
					case 'e': // exit
						time(&end);  // ���� ������ ���� �ð� ���� �� ����
						dif += (end - start); // �÷��� �ð� ����
						save(); // ���� �Լ�
						system("clear"); 
						mv_cur(0, 0);
						printf("\nSEE YOU %s . . . .\n", name);
						exit(1); // ���α׷� ����
						break;
					case 'r': // replay
						mv_cur(0, 0); 
						map(m_n - 1); // ���� �����ڸ� ���� �̹� 1�� ���� ������ �����̱⿡ 1�� ���ҽ�Ų �� ���� ���
						cnt_u = 0; 
						k_mv = 0; // ī��Ʈ Ƚ���� �̵� Ƚ�� �ʱ�ȭ
						break;
					case 'n': // new game
						mv_cur(0, 0);
						m_n = 2; // ���� ������ ����� ���� 2�� ����
						map(1); // ���� ó������ �ٽ� ����
						cnt_u = 0;
						k_mv = 0; // ī��Ʈ Ƚ���� �̵� Ƚ�� �ʱ�ȭ
						time(&start); // �ð� ���� ���� �ʱ�ȭ
						break;
					case 'd': // display help
						dis();  // ���� �Լ� ����
						break;
					case 'u': // undo
						if (cnt_u < 5 && k_mv != 0) { // �� ī��Ʈ�� �ѵ��� �Ѿ��ų�, �̵����� �ʾ��� ��츦 ����
							switch (b_key[0]) { // �ֱ� ������ Ű�� �̿��� �÷��̾� ��ǥ ��ȯ
								case 'h': 
									p_x++;
									break;
								case 'j':
									p_y--;
									break;
								case 'k':
									p_y++;
									break;
								case 'l':
									p_x--;
									break;
							}
							for (int i = 0; i < 4; i++) // Ű���� �̵� ��� ���� �� ����
								b_key[i] = b_key[i + 1];
							b_key[4] = '\0'; // ������ ĭ�� ���
							undo_a(); // undo ����
							cnt_u++; // undo Ƚ�� ����
							k_mv--; // Ű���� �̵� Ƚ�� �� ����
							com_cur();
						}
						else if (k_mv == 0) { // ���������� ���� �� ���� ���
							com_cur();
							printf("Please move first.");
						}
						else {
							com_cur();
							printf("You cannot use undo more."); // undo Ƚ�� �ʰ��� ���� ���
						}
						break;
					case 's': // save
						time(&end); // ���� ������ ���� �ð� ���� �� ����
						dif += (end - start); // �ð� ����
						save(); // �÷��� ��Ȳ ����
						com_cur(); 
						break;
					case 'f': // load file
						load(); // �Լ� ����
						time(&start); // �ð� ���� ���� �ʱ�ȭ
						break;
					case 't': // top
						rank_p(r_num); // ������ ���ڿ� ���� ��ũ ��� ���
						break;
				}
				bef = 32;
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5': // top���� ���ϴ� map�� ��ũ ��ϸ��� ���� ���� ���� ����
				if (bef == 't') { // t�� �Է��� �Ŀ��� �Է� ����
					mv_cur(12, com_y);
					printf("%c", c);
					r_num = c; // rank_p�Լ��� �̿��ϱ� ���� ���� �Է�
				}
				else {
					com_cur();
					printf("%c", c);
					bef = c;
				}
				break;	
			default : // ����Ű, ����, t������ ���ڸ� ������ ������ ��ɾ� �Է�
				com_cur();
				printf("%c", c); // ���� �Է�Ű ���
				bef = c; // ���� �Է½� ���ϴ� ��� ������ ���� Ű���� ���� ����
				if (bef == 't')
					r_num = '0'; // t�� �Է½� rank_p���� ��ü ��ũ ��� ����� ���� 0�� �� ����
				break;
		}
		if (clr()) { // Ŭ���� ���� �Լ� ����, if���� �̿��Ͽ� ���ϰ��� ���� ����
			com_cur();
			time(&end); // �ð� ���ᰪ ����
			dif += (end - start); // �ð� ����
			rank_i(); // ���� ��ũ�� ����
			rank_m(); // ��ũ ���� ����
			break;
		}
	}
}

void dis() { // display help(���� ���)
	system("clear");
	printf("      Hello %s\n", name);
	printf("---------------------------���---------------------------\n\n");
	printf("u(undo) : �ִ� 5�� �� �� �ֽ��ϴ�.\n\n");
	printf("r(replay) : ���� ���� ó������ �ٽ� �����մϴ�.\n\n");
	printf("n(new) : ù ��° �ʺ��� �ٽ� �����մϴ�.\n\n");
	printf("e(exit) : ������ �����մϴ�.\n\n");
	printf("s(save) : ���� ���¸� �����մϴ�.\n\n");
	printf("f(file load) : ����� �������� �̾ �����մϴ�.\n\n");
	printf("d(display help) : ��� ������ �����ݴϴ�.\n\n");
	printf("t(top) : ���� ������ �����ݴϴ�.\n");
	printf("        t ���� ���ڰ� ���� �ش� ���� ������ �����ݴϴ�.\n\n");
	printf("----------------------press any key-----------------------\n");
	getch(); // �ƹ�Ű�� �������� ���� ȭ������ ���ư�
	system("clear");
	printf("\n      Hello %s\n\n", name);
	for (int a = 1; a < 30; a++)
		for (int b = 0; b < 30; b++)
			printf("%c", map_f[b][a][0]); // ������ ���Ҵ� ���� ��Ȳ �� ���
	printf("(Command) ");
}

void undo_a() { // undo ����
	mv_cur(0, 4);
	for (int i = 1; i < 30; i++)
		for (int j = 0; j < 30; j++)
			printf("%c", map_f[j][i][1]); // �� ������ ù��° undo ���尪 ���
	for (int k = 0; k < 5; k++) 
		for (int i = 0; i < 30; i++)
			for (int j = 0; j < 30; j++)
				map_f[i][j][k] = map_f[i][j][k + 1]; // undo ���� ����� �� ������ ������
}

void undo_s() { // undo �� ���� ����
	for(int k = 5; k > 0; k--) // undo �� ���ϵ��� �ڷ� ��ĭ �� �����ֱ����� for���� �̿�
		for (int j = 0; j < 30; j++)
			for (int i = 0; i < 30; i++)
				map_f[i][j][k] = map_f[i][j][k - 1];
}

void undo_k(char c) { // undo�� Ű���� �Է°��� ��� ����
	for (int i = 4; i > 0; i--)
		b_key[i] = b_key[i - 1];
	b_key[0] = c; // undo �Է� �Ŀ� �÷��̾��� ��ǥ���� �ٽ� �̵������ֱ� ���� Ű���尪 ���
}

void chk_p() { // �÷��̾ �ڽ��� �ڽ��� ������ �������� ����� ������ ���� ����
	for (int i = 0; i < 30; i++)
		for (int j = 0; j < 30; j++)
			if (map_f[i][j][0] == 32 && empty[i][j] == 'O') {
				mv_cur(i + 1, j + 3);
				printf("O");
				map_f[i][j][0] = 'O'; // �ݺ������� �÷��̾ �ڽ��� ���������ʴ� �ڽ� ���� �ڸ��� �ڽ� ������ �� ��� 
			}
}

void move(char c) { // ����Ű �̵�
	switch (c) {
	case 'h': // ����
		if (map_f[p_x - 1][p_y][0] != '#') { // �����̴� ���⿡ ���� ������츸 �ڵ� ����
			if (map_f[p_x - 1][p_y][0] == '$') { // �����̴� ���⿡ �ڽ��� �ִ��� ������ ��Ȳ �Ǵ�
				if (map_f[p_x - 2][p_y][0] == 32 || map_f[p_x - 2][p_y][0] == 'O') {
					undo_s(); // undo�� ���� ���� ���� 
					map_f[p_x - 2][p_y][0] = '$'; // �� ���� �迭���� �ڽ� ��ġ ����
					map_f[p_x - 1][p_y][0] = '@'; // �÷��̾� ��ġ ����
					map_f[p_x][p_y][0] = 32; // ASCII �ڵ� 32 = ' ', �÷��̾� ��ġ�� ���� ����
					p_x--; // ��ǥ�� �������� ��ĭ �̵�
					if (!clr()) { // Ŭ����� ����� ���� ����
						mv_cur(p_x, p_y + 3);
						printf("$@ "); // �ڽ�,�÷��̾� ������ ����ϰ� �÷��̾ ���� �ִ� ��ġ�� ���� ���
					}
					undo_k(c); // Ű���� ���
					k_mv++; // Ű���� �̵�Ƚ�� ����
				}
			}
			else { // �ڽ��� ���� ��� 
				undo_s();
				map_f[p_x - 1][p_y][0] = '@'; // �� ���� �迭���� �÷��̾� ��ġ ����
				map_f[p_x][p_y][0] = 32; // �÷��̾� �ִ� ��ġ�� ���� ����
				p_x--; // ��ǥ�� �������� ��ĭ �̵�
				mv_cur(p_x + 1, p_y + 3); // Ŀ�� �̵�
				printf("@ "); // �÷��̾� ����ϰ�, �÷��̾� ���� �ִ� ��ġ�� ���� ���
				undo_k(c); // Ű���� ���
				k_mv++; // Ű���� �̵�Ƚ�� ����
			}
		}
		break;
	case 'j': // �Ʒ�
		if (map_f[p_x][p_y + 1][0] != '#') { // �����̴� ���⿡ ���� ������츸 �ڵ� ����
			if (map_f[p_x][p_y + 1][0] == '$') { // �����̴� ���⿡ �ڽ��� �ִ��� ������ ��Ȳ �Ǵ�
				if (map_f[p_x][p_y + 2][0] == 32 || map_f[p_x][p_y + 2][0] == 'O') {
					undo_s(); // undo�� ���� ���� ����
					map_f[p_x][p_y + 2][0] = '$'; // �� ���� �迭���� ��ġ ����
					map_f[p_x][p_y + 1][0] = '@'; // �÷��̾� ��ġ ����
					map_f[p_x][p_y][0] = 32; // ASCII �ڵ� 32 = ' ', �÷��̾� ��ġ�� ���� ����
					p_y++; // ��ǥ�� �Ʒ������� ��ĭ �̵�
					if (!clr()) { // Ŭ����� ����� ���� ����
						mv_cur(p_x + 1, p_y + 2);
						printf(" ");
						mv_cur(p_x + 1, p_y + 3);
						printf("@");
						mv_cur(p_x + 1, p_y + 4);
						printf("$");  // ������ �÷��̾� ��ġ�� �߽����� ���� ����, �Ʒ��� �ڽ��� ���
					}
					undo_k(c); // Ű���� ���
					k_mv++; // Ű���� �̵�Ƚ�� ����
				}
			}
			else { // �ڽ��� ���� ���
				undo_s(); // undo�� ���� ���� ����
				map_f[p_x][p_y + 1][0] = '@'; // �� ���� �迭���� �÷��̾� ��ġ ����
				map_f[p_x][p_y][0] = 32; // �÷��̾� ��ġ�� ���� ����
				p_y++; // ��ǥ�� �Ʒ������� ��ĭ �̵�
				mv_cur(p_x + 1, p_y + 2);
				printf(" ");
				mv_cur(p_x + 1, p_y + 3);
				printf("@"); // ������ �÷��̾� ��ġ�� �߽����� ���� ������ ���
				undo_k(c); // Ű���� ���
				k_mv++; // Ű���� �̵�Ƚ�� ����
			}
		}
		break;
	case 'k': // ����
		if (map_f[p_x][p_y - 1][0] != '#') { // �����̴� ���⿡ ���� ������츸 �ڵ� ����
			if (map_f[p_x][p_y - 1][0] == '$') { // �����̴� ���⿡ �ڽ��� �ִ��� ������ ��Ȳ �Ǵ�
				if (map_f[p_x][p_y - 2][0] == 32 || map_f[p_x][p_y - 2][0] == 'O') {
					undo_s();// undo�� ���� ���� ���� 
					map_f[p_x][p_y - 2][0] = '$'; // �� ���� �迭���� �ڽ� ��ġ ����
					map_f[p_x][p_y - 1][0] = '@'; // �÷��̾� ��ġ ����
					map_f[p_x][p_y][0] = 32; // ASCII �ڵ� 32 = ' '
					p_y--;
					if (!clr()) {
						mv_cur(p_x + 1, p_y + 2);
						printf("$");
						mv_cur(p_x + 1, p_y + 3);
						printf("@");
						mv_cur(p_x + 1, p_y + 4);
						printf(" "); // ������ �÷��̾� ��ġ�� �߽����� ���� �ڽ�, �Ʒ� ���� ���
					}
					undo_k(c);// Ű���� ���
					k_mv++;// Ű���� �̵�Ƚ�� ����
				}
			}
			else {
				undo_s();
				map_f[p_x][p_y - 1][0] = '@';// �� ���� �迭���� �÷��̾� ��ġ ����
				map_f[p_x][p_y][0] = 32; // �÷��̾� �ִ� ��ġ�� ���� ����
				p_y--;
				mv_cur(p_x + 1, p_y + 3);
				printf("@");
				mv_cur(p_x + 1, p_y + 4);
				printf(" ");// �÷��̾ �ִ��ڸ��� ������ ����ϰ� ������ �÷��̾ ���
				undo_k(c); // Ű���� ���
				k_mv++;// Ű���� �̵�Ƚ�� ����
			}
		}
		break;
	case 'l': // ������
		if (map_f[p_x + 1][p_y][0] != '#') { // �����̴� ���⿡ ���� ������츸 �ڵ� ����
			if (map_f[p_x + 1][p_y][0] == '$') { // �����̴� ���⿡ �ڽ��� �ִ��� ������ ��Ȳ �Ǵ�
				if (map_f[p_x + 2][p_y][0] == 32 || map_f[p_x + 2][p_y][0] == 'O') {
					undo_s(); // undo�� ���� ���� ���� 
					map_f[p_x + 2][p_y][0] = '$'; // �� ���� �迭���� �ڽ� ��ġ ����
					map_f[p_x + 1][p_y][0] = '@'; // �÷��̾� ��ġ ����
					map_f[p_x][p_y][0] = 32; // ASCII �ڵ� 32 = ' '
					p_x++;
					if (!clr()) {
						mv_cur(p_x, p_y + 3);
						printf(" @$");// ����, �÷��̾�, �ڽ� ������ ���
					}
					undo_k(c); // Ű���� ���
					k_mv++;// Ű���� �̵�Ƚ�� ����
				}
			}
			else {
				undo_s();
				map_f[p_x + 1][p_y][0] = '@';// �� ���� �迭���� �÷��̾� ��ġ ����
				map_f[p_x][p_y][0] = 32; // �÷��̾� �ִ� ��ġ�� ���� ����
				p_x++;
				mv_cur(p_x, p_y + 3);
				printf(" @");// ����, �÷��̾� ������ ���
				undo_k(c); // Ű���� ���
				k_mv++;// Ű���� �̵�Ƚ�� ����
			}
		}
		break;
	}
	chk_p();
	com_cur();
}

int clr() { // ���� Ŭ���� ���� ����
	int cnt = 0;
	for (int i = 0; i < 30; i++)
		for (int j = 0; j < 30; j++)
			if (map_f[i][j][0] == '$' && empty[i][j] == 'O')
				cnt++; // �ڽ� �������� ������ �迭�� �� ���������� �迭�� ���Ͽ� �ڽ� ��ġ�� �ڽ� ���� ��ġ�� ������ ��
	if (cnt == rm_pl) // ���� �� 1, �ٸ� �� 0�� ����
		return 1;
	else
		return 0;
}

void save() { // file save
	FILE *sf = fopen("sokoban", "w"); // sokoban ���� ���� �Ǵ� �����
	fprintf(sf, "%s %d %d %d %d\n", name, dif, m_n - 1, cnt_u, k_mv); // �̸� �ð� �ʹ�ȣ undoȽ�� Ű�����̵�Ƚ�� ����
	for (int i = 0; i < k_mv; i++) 
		fprintf(sf, "%c", b_key[i]); // Ű������ �̵��� ��� ����
	fprintf(sf, "\n");
	for (int k = 0; k < 6; k++) {
		for (int j = 0; j < 30; j++)
			for (int i = 0; i < 30; i++) {
				fprintf(sf, "%c", map_f[i][j][k]); // �ݺ����� ���� �� ���� �� ���� undo���� ����
				if (map_f[i][j][k] == '\0')
					break;
			}
		fprintf(sf, "U"); //�߰��� U�� �̿��Ͽ� undo �� �� ���ϵ��� ����
	}
	for (int i = 0; i < 30; i++)
		for (int j = 0; j < 30; j++)
			fprintf(sf, "%c", empty[j][i]); // ���������� �������Ͽ� ����
	fprintf(sf, "S"); // ���������� ������ ����
	fclose(sf);
}

void load() { // ���� �ε�
	FILE *sf = NULL;
	if (!fopen("sokoban", "r")) { // sokoban(save file)�� �����ϴ��� Ȯ��
		com_cur();
		printf("Save File doesn't exist."); // ���� �޼��� ���
	}
	else {
		sf = fopen("sokoban", "r"); // sokoban ������ �����ҽ� �б� �������� ����
		fscanf(sf, "%s %d %d %d %d\n", &name, &dif, &m_n, &cnt_u, &k_mv); // sokoban ������ ù�ٿ��� �̸�, �ð�, �ʳѹ�, ��Ƚ��, Ű���� �̵�Ƚ�� ����
		m_n++;
		for (int i = 0; i < k_mv; i++) 
			fscanf(sf, "%c", &b_key[i]); // undo�� ���� Ű���� ����� ����
		fscanf(sf, "\n"); // ������ ���� �������ν� ó��
		for (int i = 0; i < 30; i++)
			for (int j = 0; j < 30; j++) { // �� ���ϰ� �� �������� �迭 ���
				empty[i][j] = '\0';
				for (int k = 0; k < 6; k++)
					map_f[i][j][k] = '\0'; // \0 = ���������ʴ� ����, NULL
			}
		char c; // ���� ��ĵ�� �Է� ���� ���� ����
		int i = 0, j = 0, k = 0;
		while (1) {
			fscanf(sf, "%c", &c); // �ѱ��� �� �Է�
			if (i == 0 && j == 0 && k == 0) { // ������ �迭 ù���ڿ� ���๮�ڸ� �Է� �ް� ���� �ذ�
				map_f[i][j][k] = '\n';
				j++;
				continue; // �ݺ������� ó������ ��ȯ
			}
			if (c == 'U') { // �������� �𵵸� �������� �������� ������ U������ ���ǹ� �Է�
				if (k == 0)  
					com_y = j + 3; // k�� 0, ���� ���� ������ ��� U��ġ�� ���������� ��Ÿ���� ������ Ŀ�ǵ��� y��ǥ ����
				i = 0; // x�� ��ǥ �ʱ�ȭ
				j = 0; // y�� ��ǥ �ʱ�ȭ
				k++; // �� ���� �迭�� 3��° ũ��(0 : ����, 1 ~ 5 : undo)�� 1 ����
				if (k == k_mv + 1 || k == 6) // Ű���� �̵�Ƚ���� �������� �ݺ��� ����, Ű���� �̵��� �� ���� Ƚ���� ������ 6���� ����
					break;
				continue; // U�� ���ý� �ݺ���
			}
			if (c == '\0') // �ι��ڴ� ���� ����
				continue;
			map_f[i][j][k] = c; // �����Ͽ� ����
			if (k == 0) {
				if (c == '@') { // �÷��̾� ��ġ ��ǥ ����
					p_x = i;
					p_y = j;
				}
			}
			if (c == '\n') { // ����� y ��ǥ ����, x ��ǥ �ʱ�ȭ
				j++;
				i = 0;
			}
			else // x ��ǥ ����
				i++;
		}
		i = 0;
		j = 0; // x ��ǥ y ��ǥ �ʱ�ȭ
		box = 0; // �ڽ� ���� �ʱ�ȭ
		rm_pl = 0; // �ڽ� ���� ���� �ʱ�ȭ
		while (1) { // �� ���� ������ ��ĵ
			fscanf(sf, "%c", &c); 
			if (c == 'S') // �������� S�� ��� �ݺ��� Ż��
				break;
			if (c == '\0') // �ι��ڴ� ���� ����
				continue; 
			empty[i][j] = c; // ���� ���� ����
			if (c == '$') // �ڽ� ����
				box++;
			if (c == 'O') // �ڽ� ���� ���� 
				rm_pl++;
			if (c == '\n') { // ����� y ��ǥ ����
				j++;
				i = 0;
			}
			else // x ��ǥ ����
				i++;
		}
		system("clear");
		mv_cur(0, 0);
		printf("\n      Hello %s\n\n", name);
		for (int a = 1; a < 30; a++) // �ε� �� �� ���� ���
			for (int b = 0; b < 30; b++)
				printf("%c", map_f[b][a][0]);
		printf("(Command) ");
		fclose(sf);
	}
}

void rank_r(){ // rank ���� �ҷ�����
	int op, k = 0; 
	int temp_t; // �ð��� ���� ���� ����
	char temp_n[11]; // �̸��� ���� ���� ����
	FILE *rf = NULL; // ��ũ���� ���� ���� ���� ����
	if (!fopen("ranking", "r")) { // ��ũ���� �ִ��� Ȯ��
		rf = fopen("ranking", "w"); // ������ ����
		fclose(rf);
	}
	else {
		rf = fopen("ranking", "r"); // ��ũ ���� ����� ���� ����
		while (!feof(rf)) { //������ ������ �б�
			fscanf(rf, "%s %d\n", &temp_n, &temp_t); // �̸�, �ð� ������� ��ĵ
			if (temp_t < 6) { // ���� �ǹ��ϴ� ���� �ν��ϱ� ���� ���
				op = temp_t;
				k = 0;
			}
			else { // ���� �ǹ����� ������� �ν�
				rk_t[op - 1][k] = temp_t;
				strcpy(rk_n[op - 1][k++], temp_n);
				line_n[op - 1]++; // �� ��ũ�� �� ���� ���
			}
		}
		fclose(rf);
	}
}

void rank_m() { // ��ũ���� ����
	FILE *rf = fopen("ranking", "w"); // ��ũ ���� ��ü ���� �Ǵ� ������ �� ���Ͽ� ���� ���� ����
	for (int i = 0; i < 5; i++) { // �ݺ����� ���� ��ũ ���� ����
		fprintf(rf, "map %d\n", i + 1);
		for (int j = 0; j < line_n[i]; j++)
			fprintf(rf, "%s %d\n", rk_n[i][j], rk_t[i][j]);
	}
	fclose(rf);
}

void rank_i() { // ��ũ ����
	if (rk_t[m_n - 2][0] > dif) { // Ŭ����� �ð��� �ٸ� ��Ϻ��� ���� ��������� ����
		for (int i = 4; i > 0; i--) {
			rk_t[m_n - 2][i] = rk_t[m_n - 2][i - 1];
			strcpy(rk_n[m_n - 2][i], rk_n[m_n - 2][i - 1]);
		}
		strcpy(rk_n[m_n - 2][0], name);
		rk_t[m_n - 2][0] = dif;
		if (line_n[m_n - 2] != 5)
			line_n[m_n - 2]++;
	}
	else if (rk_t[m_n - 2][0] == '\0') { // ��ũ ����� �������
		rk_t[m_n - 2][0] = dif;
		strcpy(rk_n[m_n - 2][0], name);
		line_n[m_n - 2]++;
	}
	else if (rk_t[m_n - 2][line_n[m_n - 2]] < dif) { // �����ϴ� ����� 4�� �����ε�, �����ϴ� ��� ��Ϻ��� Ŭ���� �ð��� Ŭ ���
		rk_t[m_n - 2][line_n[m_n - 2]] = dif;
		strcpy(rk_n[m_n - 2][line_n[m_n - 2]], name);
		if (line_n[m_n - 2] != 5)
			line_n[m_n - 2]++;
	}
	else { // ����� ��ũ ���̿� ��������� ����
		for (int i = 0; i < 5; i++) {
			if (rk_t[m_n - 2][i] <= dif && rk_t[m_n - 2][i + 1] > dif) {
				for (int j = 4; j > i + 1; j--) {
					rk_t[m_n - 2][j] = rk_t[m_n - 2][j - 1];
					strcpy(rk_n[m_n - 2][j], rk_n[m_n - 2][j - 1]);
				}
				strcpy(rk_n[m_n - 2][i + 1], name);
				rk_t[m_n - 2][i + 1] = dif;
				if (line_n[m_n - 2] != 5)
					line_n[m_n - 2]++;
				break;
			}
		}
	}
}

void rank_p(char op) { //��ũ ��� 
	int number = op - 49; // char���� ���ڸ� �ڿ����� int������ ��ȯ
	switch (op) { // op�� ���� switch
	case '0': // ��ü ���
		system("clear");
		system("cat ranking");
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
		system("clear");
		mv_cur(0, 0);
		printf("\n      Hello %s\n\n", name);
		printf("map %c\n", op);
		for (int i = 0; i < line_n[number]; i++) // ��Ȳ�� ���� ����� �迭�� ���
			printf("%-10s-%5d\n", rk_n[number][i], rk_t[number][i]);
		break;
	}
	getch(); // �ƹ�Ű�� �������� ��ũ ���� Ż��
	mv_cur(0, 0);
	system("clear");
	printf("\n      Hello %s\n\n", name);
	for (int a = 1; a < 30; a++)
		for (int b = 0; b < 30; b++)
			printf("%c", map_f[b][a][0]);
	printf("(Command) ");
}