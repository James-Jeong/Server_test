#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include "/usr/include/mysql/mysql.h"

using namespace std;

#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASS "scabox"
#define DB_NAME "scabox"

#define BUF_MAX_LEN 20
#define SERVER_PORT 5000

int main(int argc, char* argv[]){
	MYSQL* connection = NULL;
	MYSQL conn;
	MYSQL_RES* res;
	MYSQL_ROW row;
	int query_stat;
	char name[20];

	int is_Enable = 0;
	FILE* fp = NULL;

	struct tm *t; time_t timer;
	char now[20];
	timer = time(NULL);
	t = localtime(&timer);
	sprintf(now, "%04d-%02d-%02d %02d:%02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, 
							t->tm_hour, t->tm_min, t->tm_sec);

	mysql_init(&conn);
	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);

	if(connection == NULL){
		if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
		fputs("{ MariaDB connection error! }\n", fp);
		fclose(fp);
		exit(0);
	}
	else{
		if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
		fputs("[ MariaDB connection Success ]\n", fp);
		fclose(fp);
	}

	if(mysql_select_db(connection, DB_NAME) != 0){
		mysql_close(connection);
		if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
		fputs("{ MariaDB select database FAIL! }\n", fp);
		fclose(fp);
		exit(0);
	}
	else{
		if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
		fputs("[ MariaDB select database SUCCESS ]\n", fp);
		fclose(fp);
	}

	char buf[BUF_MAX_LEN];
	struct sockaddr_in server_addr, client_addr;
	char temp_str[20];
	int server_sn, client_sn;
	socklen_t size_of_client_addr = 0; int msg_size = 0;
	if((server_sn = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
		fputs("{ Server socket open error! }\n", fp);
		fclose(fp);
		exit(0);
	}
	memset(&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);

	if(bind(server_sn, (struct sockaddr*)(&server_addr), sizeof(server_addr)) < 0){
		if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
		fputs("{ Server bind error! }\n", fp);
		fclose(fp);
		exit(0);
	}

	if(listen(server_sn, 5) < 0){
		if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
		fputs("{ Server listen error! }\n", fp);
		fclose(fp);
		exit(0);
	}
	size_of_client_addr = sizeof(client_addr);

	while(1){
		memset(buf, 0x00, sizeof(buf));
		if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
		fputs("**********************\n", fp);
		fprintf(fp, "[ %s ]\n", now);
		fputs("[ Server is waiting for request from raspberry_pi host... ]\n", fp);
		fclose(fp);

		is_Enable = 0;
		char query[100];
		int res_size = 0;
		client_sn = accept(server_sn, (struct sockaddr*)(&client_addr), &size_of_client_addr);
		if(client_sn < 0){
			if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
			fputs("{ Server accept error! }", fp);
			fclose(fp);
			exit(0);
		}
		inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, temp_str, sizeof(temp_str));
		if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
		fprintf(fp, "[ Server is connected with %s ]\n", temp_str);
		fclose(fp);

		if((msg_size = read(client_sn, buf, BUF_MAX_LEN)) <= 0){
			if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
			fputs("{ Message size is 0! }\n", fp);
			fclose(fp);
			write(client_sn, "MS0", 3);
			close(client_sn);
			continue;
		}

		if(query_stat = mysql_query(connection, "select distinct ID from USER")){
			if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
			fputs("{ MariaDB USER Query FAIL! }\n", fp);
			fclose(fp);
			exit(0);
		}
		else{
			if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
			fputs("[ MariaDB USER Query SUCCESS ]\n", fp);
			fclose(fp);
		}

		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		if(row == NULL){
			if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
			fputs("{ No ID here! }\n", fp);
			fclose(fp);
			is_Enable = -1;
			write(client_sn, "NO_ID", 5); // Warning : absence of ID
			close(client_sn);
			break;
		}

		if(is_Enable == 0){
			if(query_stat = mysql_query(connection, "select distinct Serial_number, ID from DEVICE")){
				if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
				fputs("{ MariaDB DEVICE Query FAIL! }\n", fp);
				fclose(fp);
				exit(0);
			}
			else{
				if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
				fputs("[ MariaDB DEVICE Query SUCCESS ]\n", fp);
				fclose(fp);
			}

			res_size = mysql_num_rows(res);
			MYSQL_RES* res2 = mysql_store_result(connection);
			MYSQL_ROW row2; int sn = 0; int count = 0;
			row2 = mysql_fetch_row(res2);
			if(row == NULL){
				if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
				fputs("{ No Serial Number here! }\n", fp);
				fclose(fp);
				write(client_sn, "NO_SN", 5);
				close(client_sn);
				break;
			}

			sn = atoi(row2[0]);

			for(int i = 0; i < res_size; i++){
				//if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
				//fprintf(fp, "[ row[0] : %s ]\n", row[0]);
				//fprintf(fp, "[ row2[0] : %s, row2[1] : %s ]\n", row2[0], row2[1]);
				//fclose(fp);
				if(strncmp(row[0], row2[1], strlen(row[0])) == 0){
					count++;
				}
				row = mysql_fetch_row(res);
			}
			if(count == 0){ // check error of unmatch ID
				if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
				fputs("{ Unmatch ID }\n", fp);
				fputs("{ Reconnecting... }\n", fp);
				fclose(fp);
				write(client_sn, "UNMATCH", 7);
				close(client_sn);
				continue;
			}

			vector<char*> val; char* ptr = strtok(buf, " "); int num_of_fields = 0;
			while(ptr != NULL){
				val.push_back(ptr);
				ptr = strtok(NULL, " ");
			}

			// except sunlight value
			sprintf(query, "insert into DATA values ('%d', '%s', '%s', '%s', '%s', '%s', '%s')", sn, now, val.at(0), val.at(1), val.at(2), val.at(3), "123"); // '%s' val.at(4)
			//sprintf(query, "insert into DATA values ('%d', '%s', '%s', '%s', '%s', '%s', '%s')", sn, now, val.at(0), val.at(1), val.at(2), val.at(3), val.at(4));
			if(query_stat = mysql_query(connection, query) != 0){
				if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
				fputs("{ Insert Query Error! }\n", fp);
				fclose(fp);
				exit(0);
			}

			if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
			fputs("[ Server success to receive data from Raspberry_pi! ]\n", fp);
			fclose(fp);

			/*if(query_stat = mysql_query(connection, "select * from DATA")){
				if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
				fputs("{ MariaDB Query FAIL! }\n", fp);
				fclose(fp);
				exit(0);
			}
			else{
				if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
				fputs("[ MariaDB Query SUCCESS ]\n", fp);
				fclose(fp);
			}

			if((fp = fopen("Server_v1_log.txt", "a")) == NULL){ perror("{ Fail to open Server_v1.log }"); exit(0); }
			fputs("********************\n", fp);
			res = mysql_store_result(connection);
			num_of_fields = mysql_num_fields(res);
			fputs("[ (SN) (TIME) (TH) (UV) (CO2) (AIR) ]\n", fp);
			while((row = mysql_fetch_row(res)) != NULL){
				fputs("[ ", fp);
				for(int i = 0; i < num_of_fields; i++){
					fprintf(fp, "%s ", row[i]);
				}
				fputs("]\n", fp);
			}
			fputs("********************\n", fp);*/

			write(client_sn, "OK", 2);
			close(client_sn);
			fputs("[ Server terminated this connection Successfully! ]\n\n", fp);
			fclose(fp);
		}
	}
	close(server_sn);
	mysql_close(connection);
}
