#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <termio.h> // 헤더

#define N 11 // 이름 배열 값 지정

char map_f[30][30][6]; // 맵파일 저장, [x좌표][y좌표][undo 순서]
char empty[30][30]; // 상자 자리 위치 저장을 위해 원본 맵 저장할 배열 선언
int box = 0, rm_pl = 0; // box : 보물상자 갯수, rm_pl : 상자 자리 갯수
int p_x, p_y;  // player 좌표
int com_y; // COMMAN 줄의 Y축 좌표
char name[N]; // 이름
int m_n = 1; // 맵 넘버
int cnt_u = 0; // undo 횟수
char b_key[5]; // back key : 이전에 눌렀던 방향키 저장, undo를 통한 플레이어 좌표값 이동에 이용
int k_mv = 0; // 플레이어 이동 횟수 : undo 오류 방지
time_t start, end; // 시간 시작과 끝 지점
int dif = 0; // 시간 차이
int line_n[5] = { 0,0,0,0,0 }; // ranking 줄 카운트
unsigned int rk_t[5][5]; // 랭크 시간
char rk_n[5][5][11]; // 랭크 이름

void name_w(); // 이름 저장
void mv_cur(int x, int y); // 커서 위치 변경
void map(int m_num); // 맵 출력, 배열에 저장
int getch(); // getch
void key(); //키보드 입력
void com_cur(); // 커맨드 위치 커서 이동
void dis(); // 설명서
void move(char c); // 플레이어 이동
void undo_a(); // undo act
void undo_s(); // undo save
void undo_k(char c); //undo key save
void chk_p(); // place check
int clr(); // 맵 클리어 체크
void save(); // 세이브 파일 생성
void load(); // 세이브 파일 로드
void rank_r(); // 랭크 파일 읽기
void rank_m(); // 랭크 파일 생성
void rank_i(); // 랭크 삽입
void rank_p(char op); // 랭크 출력, op : 0 ~ 5 (0 : 전체, 1 ~ 5 : 해당 맵)

int main() {
	name_w(); //이름 저장
	rank_r(); //랭크파일 읽기
	map(m_n++); // 다음 맵을 출력하기 위해 m_n++ 이용
	com_cur();
	time(&start); // 맵 출력 후 시작 시간 지정
	while (1) {
		key(); 
		if (m_n == 6) // 5번째 맵 이후 while문 종료
			break;
		system("clear"); // 다음 맵 출력 위해 화면 정리
		map(m_n++);
		k_mv = 0; // 키보드 움직임 횟수 초기화
		cnt_u = 0; // 언도 횟수 초기화
		com_cur(); // 커서 위치로 이동
		dif = 0; // 총 시간 초기화
		time(&start); // 클리어 후 다시 시작 시간 초기화
	}
	system("clear");
	mv_cur(0, 0);
	printf("\n\n\n       Congratulation!!!\n\n\n"); // 축하 메세지
	return 0;
}

void name_w() {
	int i = 0, c;

	system("clear");
	printf("Start...\n"); // 시작
	printf("input name : "); // 이름 입력
	while (1) { 
		c = getchar();
		if ((c == ' ') || (i == N)) { // 10글자 이상이거나 공백이있으면 재입력
			if (c != '\n') // 11글자 입력시 생기는 오류에대한 수정
				while ((c = getchar()) != '\n');
			printf("이름을 다시 입력하세요 : "); // 출력
			i = 0;
			continue; // 반복문 처음으로 이동
		}
		if ((c == '\n') || (c == EOF)) { // 엔터까지 입력
			name[i++] = '\0';  // 마지막을 널문자로 대체
			break; // 반복문 종료
		}
		name[i++] = c; // 이름의 배열에 입력
	}
}

void com_cur() {
	mv_cur(11, com_y); // 커서 위치 이동
	printf("                                    ");  // 써놓은건 삭제를 위한 공백 출력
	mv_cur(11, com_y); // 커서 위치 재 이동
}

void map(int m_num) { // m_num : 1 ~ 5
	FILE *fp = NULL; // 맵 파일 변수 지정
	if (!fopen("map.txt", "r")) { // 맵 파일이 존재하는지에 대한 확인
		printf("Map File Doesn't exist.");
		exit(1);
	}
	fp = fopen("map.txt", "r"); // 맵 파일 입력
	char c; // 입력 받을 변수 설정
	int i = 0, j = 0;  // 좌표
	int num1 = 0, num2 = 0; // 원하는 맵을 출력하기 위한 조건의 변수
	box = 0; // 박스 갯수
	rm_pl = 0; // 박스가 들어갈 공간의 갯수
	for (int i = 0; i < 30; i++)
		for (int j = 0; j < 30; j++) { // 반복문을 이용한 맵 파일 비우기
				empty[i][j] = '\0'; // 원본 파일을 비움
			for (int k = 0; k < 6; k++) // 언도를 위해 저장한 맵파일까지 비우기 위한 반복문 설정
				map_f[i][j][k] = '\0'; // \0 = 존재하지않는 공간, NULL
		}
	while (1) {
		fscanf(fp, "%c", &c); // 파일 스캔
		if (c == 'm') 
			num1++;
		if (c == 'p')
			num2++; // m과 p를 이용해서 원하는 맵파일 출력
		if (num1 == m_num && num2 == m_num && c != 'p' && c != 'e') { // 원하는 번호의 맵파일을 출력하고 끝을 지정하기위해 조건 추가
			map_f[i][j][0] = c; // 입력
			empty[i][j] = c; // 원본 파일 저장
			if (c == '@') { // 플레이어 위치 좌표 저장
				p_x = i;
				p_y = j;
			}
			else if (c == 'O') // 박스 넣어야하는 공간 갯수
				rm_pl++;
			else if (c == '$') // 박스 갯수
				box++;
			if (c == '\n') { // 개행시 y축의 좌표 증가 및 x축 좌표 초기화
				j++;
				i = 0;
			}
			else 
				i++; // x축 좌표값 증가
		}
		if (num1 > m_num || c == 'e') {
			com_y = j + 3; // (Command)의 y축 좌표 저장
			break; // 반복문 탈출
		}
	}
	system("clear");
	mv_cur(0, 0);
	printf("\n      Hello %s\n\n", name);
	if (box != rm_pl)
		printf("Map%d File Error", m_num); // 박스의 갯수와 박스의 공간의 갯수의 비교
	else
		for (int a = 1; a < j; a++)
			for (int b = 0; b < 30; b++)
				printf("%c", map_f[b][a][0]); // 맵 출력
	printf("(Command) "); // 커맨드 출력
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

void mv_cur(int x, int y) { // 리눅스 환경설정을 통한 커서의 좌표 이동
	printf("\033[%dG\033[%dd", x, y);
}

void key() { // 누르는 키보드 값에 대한 명령 수행
	char c, bef, r_num; // 변수들 지정
	while (c = getch()) { //while
		switch (c) {
			case 'h' :
			case 'j' :
			case 'k' :
			case 'l' : // 좌우상하의 방향키
				com_cur();
				move(c); // 따로 함수를 통해 방향에 대한 함수 설정
				break;
			case 10 : // 엔터(Enter)키의 아스키 코드값 : 10
				com_cur();
				switch (bef) { // 방향키를 제외한 입력 받은 값을 넣어준 후, 그에 따른 실행을 위한 switch문 설정
					case 'e': // exit
						time(&end);  // 정보 저장을 위해 시간 종료 값 저장
						dif += (end - start); // 플레이 시간 저장
						save(); // 저장 함수
						system("clear"); 
						mv_cur(0, 0);
						printf("\nSEE YOU %s . . . .\n", name);
						exit(1); // 프로그램 종료
						break;
					case 'r': // replay
						mv_cur(0, 0); 
						map(m_n - 1); // 후위 연산자를 통해 이미 1의 값이 증가된 상태이기에 1을 감소시킨 후 맵을 출력
						cnt_u = 0; 
						k_mv = 0; // 카운트 횟수와 이동 횟수 초기화
						break;
					case 'n': // new game
						mv_cur(0, 0);
						m_n = 2; // 후위 연산자 사용을 위해 2로 지정
						map(1); // 맵을 처음부터 다시 시작
						cnt_u = 0;
						k_mv = 0; // 카운트 횟수와 이동 횟수 초기화
						time(&start); // 시간 시작 지점 초기화
						break;
					case 'd': // display help
						dis();  // 따로 함수 지정
						break;
					case 'u': // undo
						if (cnt_u < 5 && k_mv != 0) { // 언도 카운트가 한도를 넘었거나, 이동하지 않았을 경우를 배제
							switch (b_key[0]) { // 최근 눌렀던 키를 이용한 플레이어 좌표 변환
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
							for (int i = 0; i < 4; i++) // 키보드 이동 백업 값을 재 정렬
								b_key[i] = b_key[i + 1];
							b_key[4] = '\0'; // 마지막 칸을 비움
							undo_a(); // undo 실행
							cnt_u++; // undo 횟수 증가
							k_mv--; // 키보드 이동 횟수 값 감소
							com_cur();
						}
						else if (k_mv == 0) { // 움직인적이 없을 시 오류 출력
							com_cur();
							printf("Please move first.");
						}
						else {
							com_cur();
							printf("You cannot use undo more."); // undo 횟수 초과시 오류 출력
						}
						break;
					case 's': // save
						time(&end); // 정보 저장을 위해 시간 종료 값 저장
						dif += (end - start); // 시간 저장
						save(); // 플레이 상황 저장
						com_cur(); 
						break;
					case 'f': // load file
						load(); // 함수 지정
						time(&start); // 시간 시작 지점 초기화
						break;
					case 't': // top
						rank_p(r_num); // 지정된 숫자에 따라 랭크 목록 출력
						break;
				}
				bef = 32;
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5': // top에서 원하는 map의 랭크 목록만을 보기 위한 숫자 지정
				if (bef == 't') { // t를 입력한 후에만 입력 가능
					mv_cur(12, com_y);
					printf("%c", c);
					r_num = c; // rank_p함수를 이용하기 위한 숫자 입력
				}
				else {
					com_cur();
					printf("%c", c);
					bef = c;
				}
				break;	
			default : // 방향키, 엔터, t이후의 숫자를 제외한 나머지 명령어 입력
				com_cur();
				printf("%c", c); // 받은 입력키 출력
				bef = c; // 엔터 입력시 원하는 명령 실행을 위한 키보드 값의 저장
				if (bef == 't')
					r_num = '0'; // t를 입력시 rank_p에서 전체 랭크 목록 출력을 위한 0의 값 저장
				break;
		}
		if (clr()) { // 클리어 조건 함수 지정, if문을 이용하여 리턴값에 따른 실행
			com_cur();
			time(&end); // 시간 종료값 지정
			dif += (end - start); // 시간 저장
			rank_i(); // 현재 랭크값 정렬
			rank_m(); // 랭크 파일 생성
			break;
		}
	}
}

void dis() { // display help(설명서 출력)
	system("clear");
	printf("      Hello %s\n", name);
	printf("---------------------------명령---------------------------\n\n");
	printf("u(undo) : 최대 5번 할 수 있습니다.\n\n");
	printf("r(replay) : 현재 맵을 처음부터 다시 시작합니다.\n\n");
	printf("n(new) : 첫 번째 맵부터 다시 시작합니다.\n\n");
	printf("e(exit) : 게임을 종료합니다.\n\n");
	printf("s(save) : 현재 상태를 저장합니다.\n\n");
	printf("f(file load) : 저장된 시점부터 이어서 게임합니다.\n\n");
	printf("d(display help) : 명령 내용을 보여줍니다.\n\n");
	printf("t(top) : 게임 순위를 보여줍니다.\n");
	printf("        t 다음 숫자가 오면 해당 맵의 순위를 보여줍니다.\n\n");
	printf("----------------------press any key-----------------------\n");
	getch(); // 아무키나 눌렀을시 이전 화면으로 돌아감
	system("clear");
	printf("\n      Hello %s\n\n", name);
	for (int a = 1; a < 30; a++)
		for (int b = 0; b < 30; b++)
			printf("%c", map_f[b][a][0]); // 저장해 놓았던 현재 상황 맵 출력
	printf("(Command) ");
}

void undo_a() { // undo 실행
	mv_cur(0, 4);
	for (int i = 1; i < 30; i++)
		for (int j = 0; j < 30; j++)
			printf("%c", map_f[j][i][1]); // 맵 파일의 첫번째 undo 저장값 출력
	for (int k = 0; k < 5; k++) 
		for (int i = 0; i < 30; i++)
			for (int j = 0; j < 30; j++)
				map_f[i][j][k] = map_f[i][j][k + 1]; // undo 이후 저장된 맵 파일의 재정렬
}

void undo_s() { // undo 맵 파일 생성
	for(int k = 5; k > 0; k--) // undo 맵 파일들을 뒤로 한칸 씩 땡겨주기위한 for문의 이용
		for (int j = 0; j < 30; j++)
			for (int i = 0; i < 30; i++)
				map_f[i][j][k] = map_f[i][j][k - 1];
}

void undo_k(char c) { // undo시 키보드 입력값의 백업 저장
	for (int i = 4; i > 0; i--)
		b_key[i] = b_key[i - 1];
	b_key[0] = c; // undo 입력 후에 플레이어의 좌표값을 다시 이동시켜주기 위한 키보드값 백업
}

void chk_p() { // 플레이어나 박스가 박스의 공간을 지웠을시 생기는 오류에 대한 수정
	for (int i = 0; i < 30; i++)
		for (int j = 0; j < 30; j++)
			if (map_f[i][j][0] == 32 && empty[i][j] == 'O') {
				mv_cur(i + 1, j + 3);
				printf("O");
				map_f[i][j][0] = 'O'; // 반복적으로 플레이어나 박스가 존재하지않는 박스 공간 자리에 박스 공간을 재 출력 
			}
}

void move(char c) { // 방향키 이동
	switch (c) {
	case 'h': // 왼쪽
		if (map_f[p_x - 1][p_y][0] != '#') { // 움직이는 방향에 벽이 없을경우만 코드 실행
			if (map_f[p_x - 1][p_y][0] == '$') { // 움직이는 방향에 박스가 있는지 없는지 상황 판단
				if (map_f[p_x - 2][p_y][0] == 32 || map_f[p_x - 2][p_y][0] == 'O') {
					undo_s(); // undo를 위한 파일 저장 
					map_f[p_x - 2][p_y][0] = '$'; // 맵 파일 배열에서 박스 위치 변경
					map_f[p_x - 1][p_y][0] = '@'; // 플레이어 위치 변경
					map_f[p_x][p_y][0] = 32; // ASCII 코드 32 = ' ', 플레이어 위치에 공백 저장
					p_x--; // 좌표를 왼쪽으로 한칸 이동
					if (!clr()) { // 클리어시 출력은 하지 않음
						mv_cur(p_x, p_y + 3);
						printf("$@ "); // 박스,플레이어 순서로 출력하고 플레이어가 원래 있던 위치에 공백 출력
					}
					undo_k(c); // 키보드 백업
					k_mv++; // 키보드 이동횟수 증가
				}
			}
			else { // 박스가 없을 경우 
				undo_s();
				map_f[p_x - 1][p_y][0] = '@'; // 맵 파일 배열에서 플레이어 위치 변경
				map_f[p_x][p_y][0] = 32; // 플레이어 있던 위치에 공백 저장
				p_x--; // 좌표를 왼쪽으로 한칸 이동
				mv_cur(p_x + 1, p_y + 3); // 커서 이동
				printf("@ "); // 플레이어 출력하고, 플레이어 원래 있던 위치에 공백 출력
				undo_k(c); // 키보드 백업
				k_mv++; // 키보드 이동횟수 증가
			}
		}
		break;
	case 'j': // 아래
		if (map_f[p_x][p_y + 1][0] != '#') { // 움직이는 방향에 벽이 없을경우만 코드 실행
			if (map_f[p_x][p_y + 1][0] == '$') { // 움직이는 방향에 박스가 있는지 없는지 상황 판단
				if (map_f[p_x][p_y + 2][0] == 32 || map_f[p_x][p_y + 2][0] == 'O') {
					undo_s(); // undo를 위한 파일 저장
					map_f[p_x][p_y + 2][0] = '$'; // 맵 파일 배열에서 위치 변경
					map_f[p_x][p_y + 1][0] = '@'; // 플레이어 위치 변경
					map_f[p_x][p_y][0] = 32; // ASCII 코드 32 = ' ', 플레이어 위치에 공백 저장
					p_y++; // 좌표를 아래쪽으로 한칸 이동
					if (!clr()) { // 클리어시 출력은 하지 않음
						mv_cur(p_x + 1, p_y + 2);
						printf(" ");
						mv_cur(p_x + 1, p_y + 3);
						printf("@");
						mv_cur(p_x + 1, p_y + 4);
						printf("$");  // 움직인 플레이어 위치를 중심으로 위에 공백, 아래에 박스를 출력
					}
					undo_k(c); // 키보드 백업
					k_mv++; // 키보드 이동횟수 증가
				}
			}
			else { // 박스가 없을 경우
				undo_s(); // undo를 위한 파일 저장
				map_f[p_x][p_y + 1][0] = '@'; // 맵 파일 배열에서 플레이어 위치 변경
				map_f[p_x][p_y][0] = 32; // 플레이어 위치에 공백 저장
				p_y++; // 좌표를 아래쪽으로 한칸 이동
				mv_cur(p_x + 1, p_y + 2);
				printf(" ");
				mv_cur(p_x + 1, p_y + 3);
				printf("@"); // 움직인 플레이어 위치를 중심으로 위에 공백을 출력
				undo_k(c); // 키보드 백업
				k_mv++; // 키보드 이동횟수 증가
			}
		}
		break;
	case 'k': // 위쪽
		if (map_f[p_x][p_y - 1][0] != '#') { // 움직이는 방향에 벽이 없을경우만 코드 실행
			if (map_f[p_x][p_y - 1][0] == '$') { // 움직이는 방향에 박스가 있는지 없는지 상황 판단
				if (map_f[p_x][p_y - 2][0] == 32 || map_f[p_x][p_y - 2][0] == 'O') {
					undo_s();// undo를 위한 파일 저장 
					map_f[p_x][p_y - 2][0] = '$'; // 맵 파일 배열에서 박스 위치 변경
					map_f[p_x][p_y - 1][0] = '@'; // 플레이어 위치 변경
					map_f[p_x][p_y][0] = 32; // ASCII 코드 32 = ' '
					p_y--;
					if (!clr()) {
						mv_cur(p_x + 1, p_y + 2);
						printf("$");
						mv_cur(p_x + 1, p_y + 3);
						printf("@");
						mv_cur(p_x + 1, p_y + 4);
						printf(" "); // 움직인 플레이어 위치를 중심으로 위에 박스, 아래 공백 출력
					}
					undo_k(c);// 키보드 백업
					k_mv++;// 키보드 이동횟수 증가
				}
			}
			else {
				undo_s();
				map_f[p_x][p_y - 1][0] = '@';// 맵 파일 배열에서 플레이어 위치 변경
				map_f[p_x][p_y][0] = 32; // 플레이어 있던 위치에 공백 저장
				p_y--;
				mv_cur(p_x + 1, p_y + 3);
				printf("@");
				mv_cur(p_x + 1, p_y + 4);
				printf(" ");// 플레이어가 있던자리에 공백을 출력하고 위에는 플레이어를 출력
				undo_k(c); // 키보드 백업
				k_mv++;// 키보드 이동횟수 증가
			}
		}
		break;
	case 'l': // 오른쪽
		if (map_f[p_x + 1][p_y][0] != '#') { // 움직이는 방향에 벽이 없을경우만 코드 실행
			if (map_f[p_x + 1][p_y][0] == '$') { // 움직이는 방향에 박스가 있는지 없는지 상황 판단
				if (map_f[p_x + 2][p_y][0] == 32 || map_f[p_x + 2][p_y][0] == 'O') {
					undo_s(); // undo를 위한 파일 저장 
					map_f[p_x + 2][p_y][0] = '$'; // 맵 파일 배열에서 박스 위치 변경
					map_f[p_x + 1][p_y][0] = '@'; // 플레이어 위치 변경
					map_f[p_x][p_y][0] = 32; // ASCII 코드 32 = ' '
					p_x++;
					if (!clr()) {
						mv_cur(p_x, p_y + 3);
						printf(" @$");// 공백, 플레이어, 박스 순으로 출력
					}
					undo_k(c); // 키보드 백업
					k_mv++;// 키보드 이동횟수 증가
				}
			}
			else {
				undo_s();
				map_f[p_x + 1][p_y][0] = '@';// 맵 파일 배열에서 플레이어 위치 변경
				map_f[p_x][p_y][0] = 32; // 플레이어 있던 위치에 공백 저장
				p_x++;
				mv_cur(p_x, p_y + 3);
				printf(" @");// 공백, 플레이어 순으로 출력
				undo_k(c); // 키보드 백업
				k_mv++;// 키보드 이동횟수 증가
			}
		}
		break;
	}
	chk_p();
	com_cur();
}

int clr() { // 게임 클리어 조건 설정
	int cnt = 0;
	for (int i = 0; i < 30; i++)
		for (int j = 0; j < 30; j++)
			if (map_f[i][j][0] == '$' && empty[i][j] == 'O')
				cnt++; // 박스 공간만을 저장한 배열과 맵 현재파일의 배열을 비교하여 박스 위치와 박스 공간 위치가 같은지 비교
	if (cnt == rm_pl) // 같을 시 1, 다를 시 0을 리턴
		return 1;
	else
		return 0;
}

void save() { // file save
	FILE *sf = fopen("sokoban", "w"); // sokoban 파일 생성 또는 덮어쓰기
	fprintf(sf, "%s %d %d %d %d\n", name, dif, m_n - 1, cnt_u, k_mv); // 이름 시간 맵번호 undo횟수 키보드이동횟수 저장
	for (int i = 0; i < k_mv; i++) 
		fprintf(sf, "%c", b_key[i]); // 키보드의 이동값 백업 저장
	fprintf(sf, "\n");
	for (int k = 0; k < 6; k++) {
		for (int j = 0; j < 30; j++)
			for (int i = 0; i < 30; i++) {
				fprintf(sf, "%c", map_f[i][j][k]); // 반복문을 통한 맵 파일 및 맵의 undo파일 저장
				if (map_f[i][j][k] == '\0')
					break;
			}
		fprintf(sf, "U"); //중간에 U를 이용하여 undo 및 맵 파일들을 구분
	}
	for (int i = 0; i < 30; i++)
		for (int j = 0; j < 30; j++)
			fprintf(sf, "%c", empty[j][i]); // 원본파일을 저장파일에 저장
	fprintf(sf, "S"); // 원본파일의 끝지점 저장
	fclose(sf);
}

void load() { // 파일 로드
	FILE *sf = NULL;
	if (!fopen("sokoban", "r")) { // sokoban(save file)이 존재하는지 확인
		com_cur();
		printf("Save File doesn't exist."); // 오류 메세지 출력
	}
	else {
		sf = fopen("sokoban", "r"); // sokoban 파일이 존재할시 읽기 형식으로 오픈
		fscanf(sf, "%s %d %d %d %d\n", &name, &dif, &m_n, &cnt_u, &k_mv); // sokoban 파일의 첫줄에서 이름, 시간, 맵넘버, 언도횟수, 키보드 이동횟수 저장
		m_n++;
		for (int i = 0; i < k_mv; i++) 
			fscanf(sf, "%c", &b_key[i]); // undo를 위한 키보드 백업값 저장
		fscanf(sf, "\n"); // 개행을 따로 받음으로써 처리
		for (int i = 0; i < 30; i++)
			for (int j = 0; j < 30; j++) { // 맵 파일과 맵 원본파일 배열 비움
				empty[i][j] = '\0';
				for (int k = 0; k < 6; k++)
					map_f[i][j][k] = '\0'; // \0 = 존재하지않는 공간, NULL
			}
		char c; // 파일 스캔후 입력 받을 변수 지정
		int i = 0, j = 0, k = 0;
		while (1) {
			fscanf(sf, "%c", &c); // 한글자 씩 입력
			if (i == 0 && j == 0 && k == 0) { // 맵파일 배열 첫글자에 개행문자를 입력 받고 오류 해결
				map_f[i][j][k] = '\n';
				j++;
				continue; // 반복문으로 처음으로 반환
			}
			if (c == 'U') { // 맵파일을 언도를 기준으로 나누었기 때문에 U에따른 조건문 입력
				if (k == 0)  
					com_y = j + 3; // k가 0, 맵이 현재 파일일 경우 U위치가 마지막줄을 나타내기 때문에 커맨드의 y좌표 저장
				i = 0; // x축 좌표 초기화
				j = 0; // y축 좌표 초기화
				k++; // 맵 파일 배열의 3번째 크기(0 : 현재, 1 ~ 5 : undo)를 1 증가
				if (k == k_mv + 1 || k == 6) // 키보드 이동횟수를 기준으로 반복문 종료, 키보드 이동이 언도 가능 횟수를 넘을시 6에서 종료
					break;
				continue; // U가 나올시 반복문
			}
			if (c == '\0') // 널문자는 읽지 않음
				continue;
			map_f[i][j][k] = c; // 맵파일에 저장
			if (k == 0) {
				if (c == '@') { // 플레이어 위치 좌표 저장
					p_x = i;
					p_y = j;
				}
			}
			if (c == '\n') { // 개행시 y 좌표 증가, x 좌표 초기화
				j++;
				i = 0;
			}
			else // x 좌표 증가
				i++;
		}
		i = 0;
		j = 0; // x 좌표 y 좌표 초기화
		box = 0; // 박스 갯수 초기화
		rm_pl = 0; // 박스 공간 갯수 초기화
		while (1) { // 맵 원본 파일의 스캔
			fscanf(sf, "%c", &c); 
			if (c == 'S') // 끝지점인 S의 경우 반복문 탈출
				break;
			if (c == '\0') // 널문자는 읽지 않음
				continue; 
			empty[i][j] = c; // 원본 파일 저장
			if (c == '$') // 박스 갯수
				box++;
			if (c == 'O') // 박스 공간 갯수 
				rm_pl++;
			if (c == '\n') { // 개행시 y 좌표 증가
				j++;
				i = 0;
			}
			else // x 좌표 증가
				i++;
		}
		system("clear");
		mv_cur(0, 0);
		printf("\n      Hello %s\n\n", name);
		for (int a = 1; a < 30; a++) // 로드 후 맵 파일 출력
			for (int b = 0; b < 30; b++)
				printf("%c", map_f[b][a][0]);
		printf("(Command) ");
		fclose(sf);
	}
}

void rank_r(){ // rank 파일 불러오기
	int op, k = 0; 
	int temp_t; // 시간을 받을 변수 저장
	char temp_n[11]; // 이름을 받을 변수 저장
	FILE *rf = NULL; // 랭크파일 저장 받을 변수 선언
	if (!fopen("ranking", "r")) { // 랭크파일 있는지 확인
		rf = fopen("ranking", "w"); // 없을시 생성
		fclose(rf);
	}
	else {
		rf = fopen("ranking", "r"); // 랭크 파일 존재시 파일 오픈
		while (!feof(rf)) { //파일의 끝까지 읽기
			fscanf(rf, "%s %d\n", &temp_n, &temp_t); // 이름, 시간 순서대로 스캔
			if (temp_t < 6) { // 맵을 의미하는 행을 인식하기 위한 방법
				op = temp_t;
				k = 0;
			}
			else { // 맵을 의미하지 않을경우 인식
				rk_t[op - 1][k] = temp_t;
				strcpy(rk_n[op - 1][k++], temp_n);
				line_n[op - 1]++; // 각 랭크의 행 숫자 계산
			}
		}
		fclose(rf);
	}
}

void rank_m() { // 랭크파일 생성
	FILE *rf = fopen("ranking", "w"); // 랭크 파일 자체 생성 또는 덮어쓰기와 그 파일에 대한 변수 설정
	for (int i = 0; i < 5; i++) { // 반복문을 통한 랭크 파일 생성
		fprintf(rf, "map %d\n", i + 1);
		for (int j = 0; j < line_n[i]; j++)
			fprintf(rf, "%s %d\n", rk_n[i][j], rk_t[i][j]);
	}
	fclose(rf);
}

void rank_i() { // 랭크 정렬
	if (rk_t[m_n - 2][0] > dif) { // 클리어시 시간이 다른 기록보다 제일 적은경우의 정렬
		for (int i = 4; i > 0; i--) {
			rk_t[m_n - 2][i] = rk_t[m_n - 2][i - 1];
			strcpy(rk_n[m_n - 2][i], rk_n[m_n - 2][i - 1]);
		}
		strcpy(rk_n[m_n - 2][0], name);
		rk_t[m_n - 2][0] = dif;
		if (line_n[m_n - 2] != 5)
			line_n[m_n - 2]++;
	}
	else if (rk_t[m_n - 2][0] == '\0') { // 랭크 기록이 없을경우
		rk_t[m_n - 2][0] = dif;
		strcpy(rk_n[m_n - 2][0], name);
		line_n[m_n - 2]++;
	}
	else if (rk_t[m_n - 2][line_n[m_n - 2]] < dif) { // 존재하는 기록이 4개 이하인데, 존재하는 모든 기록보다 클리어 시간이 클 경우
		rk_t[m_n - 2][line_n[m_n - 2]] = dif;
		strcpy(rk_n[m_n - 2][line_n[m_n - 2]], name);
		if (line_n[m_n - 2] != 5)
			line_n[m_n - 2]++;
	}
	else { // 기록이 랭크 사이에 있을경우의 정렬
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

void rank_p(char op) { //랭크 출력 
	int number = op - 49; // char형의 한자리 자연수를 int형으로 변환
	switch (op) { // op에 대한 switch
	case '0': // 전체 출력
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
		for (int i = 0; i < line_n[number]; i++) // 상황에 따라 저장된 배열을 출력
			printf("%-10s-%5d\n", rk_n[number][i], rk_t[number][i]);
		break;
	}
	getch(); // 아무키나 눌렀을시 랭크 파일 탈출
	mv_cur(0, 0);
	system("clear");
	printf("\n      Hello %s\n\n", name);
	for (int a = 1; a < 30; a++)
		for (int b = 0; b < 30; b++)
			printf("%c", map_f[b][a][0]);
	printf("(Command) ");
}