#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>//이메일 포맷확인을 위한해더파일을 임폴드

#define MAX_NAME_LENGTH 256// 최대 255bytes
#define MAX_EMAIL_LENGTH 256
#define MAX_MEMBERS 100

typedef struct {
    char name[MAX_NAME_LENGTH];
    int age;
    char email[MAX_EMAIL_LENGTH];
} Member;


int EmailFormat(const char* email)/*이멜일 포맷 확인 */ {
    regex_t regex;
    int result = regcomp(&regex, "^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$", REG_EXTENDED);
    if (result != 0) {
        printf("이메일 형식 검사를 위한 정규식을 컴파일할 수 없습니다.\n");
        return 0;
    }

    result = regexec(&regex, email, 0, NULL, 0);
    regfree(&regex);

    return result == 0;
}


void saveMember()/*선택 1 회원정보 저장*/ {
    FILE* file = fopen("members.txt", "a"); // 회원정보 추가를 위해 "a" 모드로 파일 열기. 파일이 없으면 생성됨
    if (file == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return;
    }

    char answer = 'Y'; // 연속 입력을 위한 변수 선언
    while (answer == 'Y' || answer == 'y') {
        Member member;
        memset(&member, 0, sizeof(Member));

        printf("이름: ");
        fgets(member.name, MAX_NAME_LENGTH, stdin);
        member.name[strcspn(member.name, "\n")] = '\0';

        while (1) {
            printf("나이: ");
            scanf("%d", &member.age);
            getchar(); // 입력 버퍼 비우기

            if (member.age > 200) {
                printf("나이는 최대 200세까지입니다. 올바른 나이를 입력해주세요.\n");
            } else {
                break; // 올바른 나이가 입력되면 반복문 탈출
            }
        }

        while (1) {
            printf("이메일: ");
            fgets(member.email, MAX_EMAIL_LENGTH, stdin);
            member.email[strcspn(member.email, "\n")] = '\0';

            if (!EmailFormat(member.email)) {
                printf("올바른 이메일 형식이 아닙니다. 다시 입력해주세요.\n");
            } else {
                break; // 올바른 이메일 형식이 입력되면 반복문 탈출
            }
        }

        fprintf(file, "%s/%d/%s\n", member.name, member.age, member.email);

        printf("계속 입력하시겠습니까? (Y/N): ");
        scanf(" %c", &answer);
        getchar(); // 입력 버퍼 비우기
    }

    fclose(file);
}

void updateMember()/*선택 2 회원정보 수정*/ {
    char name[MAX_NAME_LENGTH];
    printf("수정할 회원의 이름을 입력하세요: ");
    fgets(name, MAX_NAME_LENGTH, stdin);
    name[strcspn(name, "\n")] = '\0';  // 개행 문자 제거

    FILE* file = fopen("members.txt", "r");// 수정할회원을 찾기위해 읽기모드로 파일 열기
    if (file == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return;
    }

    FILE* tempFile = fopen("temp.txt", "w");//수정을 위한 파일 생성
    if (tempFile == NULL) {
        printf("파일을 열 수 없습니다.\n");
        fclose(file);
        return;
    }

    char line[MAX_NAME_LENGTH + MAX_EMAIL_LENGTH + 10];//라인단위로 읽어오는데 사용
    char currentName[MAX_NAME_LENGTH];
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^/]", currentName);
        if (strcmp(currentName, name) == 0) {
            printf("새로운 회원 정보를 입력하세요.\n");
            Member updatedMember;
            printf("이름: ");
            fgets(updatedMember.name, MAX_NAME_LENGTH, stdin);
            updatedMember.name[strcspn(updatedMember.name, "\n")] = '\0';

            while (1) {
                printf("나이: ");
                scanf("%d", &updatedMember.age);
                getchar();  // 버퍼 비우기

                if (updatedMember.age > 200) {
                    printf("나이는 최대 200세까지입니다. 올바른 나이를 입력해주세요.\n");
                }
							 	else {
                    break;
                }
            }

            while (1) {
                printf("이메일: ");
                fgets(updatedMember.email, MAX_EMAIL_LENGTH, stdin);
                updatedMember.email[strcspn(updatedMember.email, "\n")] = '\0';

                if (!EmailFormat(updatedMember.email)) {
                    printf("올바른 이메일 형식이 아닙니다. 다시 입력해주세요.\n");
                }
							 	else {
                    break;
                }
            }

            fprintf(tempFile, "%s/%d/%s\n", updatedMember.name, updatedMember.age, updatedMember.email);
            printf("회원 정보가 수정되었습니다.\n");
            found = 1;
        }
			 	else {
            fprintf(tempFile, "%s", line);
        }
    }  

    fclose(file);
    fclose(tempFile);

    remove("members.txt");
    rename("temp.txt", "members.txt");//파일 교채

    if (!found) {
        printf("해당하는 회원을 찾을 수 없습니다.\n");
    }
}

void deleteMember()/*회원 정보 삭제*/ {
    char name[MAX_NAME_LENGTH];
    printf("삭제할 회원의 이름을 입력하세요: ");
    fgets(name, MAX_NAME_LENGTH, stdin);
    name[strcspn(name, "\n")] = '\0';  // 개행 문자 제거

    FILE* file = fopen("members.txt", "r");
    if (file == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return;
    }

    FILE* tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("파일을 열 수 없습니다.\n");
        fclose(file);
        return;
    }

    char line[MAX_NAME_LENGTH + MAX_EMAIL_LENGTH + 10];
    char currentName[MAX_NAME_LENGTH];
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^/]", currentName);
        if (strcmp(currentName, name) == 0){
            printf("회원이 삭제되었습니다.\n");
            found = 1;
        }
        else {
            fprintf(tempFile, "%s", line);
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("members.txt");
    rename("temp.txt", "members.txt");

    if (!found) {
        printf("해당하는 회원을 찾을 수 없습니다.\n");
    }
}


void listMembers()/*선택 4회원정보 출력*/ {
    FILE* file = fopen("members.txt", "r");
    if (file == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return;
    }

    char line[MAX_NAME_LENGTH + MAX_EMAIL_LENGTH + 10];

    while (fgets(line, sizeof(line), file)) {//반복문을 이용하여 회원정보 출력
        printf("%s", line);
    }

    fclose(file);
}

int main() {
    int choice;
    Member member;

    while (1) {
        printf("\n1. 신규회원 저장\n");
        printf("2. 회원정보 수정\n");
        printf("3. 회원 삭제\n");
        printf("4. 모든 회원 리스트\n");
        printf("5. 종료\n");
        printf("입력하세요: ");
        scanf("%d", &choice);
        getchar();  // 버퍼 비우기

        switch (choice) {
        case 1:
            saveMember(member);
            break;
        case 2:
            updateMember();
            break;
        case 3:
            deleteMember();
            break;
        case 4:
            listMembers();
            break;
        case 5:
            printf("종료합니다.\n");
            return 0;
        default:
            printf("올바른 메뉴를 선택하세요.\n");
            break;
        }
    }

    return 0;
}
